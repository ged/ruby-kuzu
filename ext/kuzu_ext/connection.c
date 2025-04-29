/*
 *  connection.c - Kuzu::Connection class
 *
 */

#include "kuzu.h"
#include "kuzu_ext.h"
#include "ruby/thread.h"

#define CHECK_CONNECTION(self) ((rkuzu_connection *)rb_check_typeddata((self), &rkuzu_connection_type))
// #define DEBUG_GC(msg, ptr) fprintf( stderr, msg, ptr )
#define DEBUG_GC(msg, ptr)


VALUE rkuzu_cKuzuConnection;

static void rkuzu_connection_free( void * );
static void rkuzu_connection_mark( void * );

static const rb_data_type_t rkuzu_connection_type = {
	.wrap_struct_name = "Kuzu::Connection",
	.function = {
		.dfree = rkuzu_connection_free,
		.dmark = rkuzu_connection_mark,
	},
	.data = NULL,
};


rkuzu_connection *
rkuzu_get_connection( VALUE conn_obj )
{
	return CHECK_CONNECTION( conn_obj );
}


static rkuzu_connection *
rkuzu_connection_alloc( void )
{
	rkuzu_connection *ptr = ALLOC( rkuzu_connection );

	ptr->database = Qnil;
	ptr->queries = rb_ary_new();
	ptr->statements = rb_ary_new();

	return ptr;
}


static void
rkuzu_connection_free( void *ptr )
{
	rkuzu_connection *conn_s = (rkuzu_connection *)ptr;

	if ( ptr ) {
		DEBUG_GC( ">>> freeing connection %p\n", ptr );

		kuzu_connection_destroy( &conn_s->conn );

		xfree( ptr );
		ptr = NULL;
	}
}


static void
rkuzu_connection_mark( void *ptr )
{
	rkuzu_connection *conn_s = (rkuzu_connection *)ptr;

	if ( ptr ) {
		DEBUG_GC( ">>> marking connection %p\n", ptr );
		rb_gc_mark( conn_s->database );
		rb_gc_mark( conn_s->statements );
		rb_gc_mark( conn_s->queries );
	}
}


/*
 * ::allocate function
 */
static VALUE
rkuzu_connection_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_connection_type, NULL );
}



/*
 * call-seq:
 *    new( database )   -> connection
 *
 * Create a Kuzu::Connection to the specified +database+.
 *
 */
static VALUE
rkuzu_connection_initialize( VALUE self, VALUE database )
{
	rkuzu_connection *ptr = CHECK_CONNECTION( self );

	if ( !ptr ) {
		rkuzu_database *dbobject = rkuzu_get_database( database );
		ptr = rkuzu_connection_alloc();

		if ( kuzu_connection_init(&dbobject->db, &ptr->conn) != KuzuSuccess ) {
			xfree( ptr );
			ptr = NULL;
			rb_raise( rkuzu_eConnectionError, "Failed to connect!" );
		}

		DEBUG_GC( ">>> allocated connection %p\n", ptr );
		RTYPEDDATA_DATA( self ) = ptr;

		ptr->database = database;

	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit connection" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
}


struct query_call {
	kuzu_connection *conn;
	const char *query_s;
	kuzu_query_result *result;
};


static void *
rkuzu_connection_do_query_without_gvl( void *ptr )
{
	struct query_call *qcall = (struct query_call *)ptr;
	kuzu_state state;

	state = kuzu_connection_query( qcall->conn, qcall->query_s, qcall->result );

	return (void *)state;
}


static void
rkuzu_connection_cancel_query( void *ptr )
{
	kuzu_connection *conn = (kuzu_connection *)ptr;
	kuzu_connection_interrupt( conn );
}


static kuzu_query_result
rkuzu_connection_do_query( VALUE self, VALUE query )
{
	rkuzu_connection *conn = CHECK_CONNECTION( self );
	const char *query_s = StringValueCStr( query );
	kuzu_query_result result;
	kuzu_state query_state;
	struct query_call qcall;
	void *result_ptr;

	qcall.conn = &conn->conn;
	qcall.query_s = query_s;
	qcall.result = &result;

	result_ptr = rb_thread_call_without_gvl(
		rkuzu_connection_do_query_without_gvl, (void *)&qcall,
		rkuzu_connection_cancel_query, (void *)&conn->conn );

	query_state = (kuzu_state)result_ptr;

	if ( query_state != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not execute query `%s': %s.", query_s, err_detail );

		kuzu_destroy_string( err_detail );
		kuzu_query_result_destroy( &result );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	return *qcall.result;
}



static VALUE
rkuzu_connection__query( VALUE self, VALUE query )
{
	kuzu_query_result result = rkuzu_connection_do_query( self, query );
	return rkuzu_result_from_query( rkuzu_cKuzuResult, self, query, result );
}


/*
 * call-seq:
 *    connection.query!( query_string )
 *
 * Execute the given +query_string+ and return `true` if the query was
 * successful.
 *
 */
static VALUE
rkuzu_connection_query_bang( VALUE self, VALUE query )
{
	kuzu_query_result result = rkuzu_connection_do_query( self, query );
	return kuzu_query_result_is_success( &result ) ? Qtrue : Qfalse;
}


/*
 * call-seq:
 *    connection.max_num_threads_for_exec   -> integer
 *
 * Returns the maximum number of threads of the connection to use for
 * executing queries.
 *
 */
static VALUE
rkuzu_connection_max_num_threads_for_exec( VALUE self )
{
	rkuzu_connection *ptr = CHECK_CONNECTION( self );
	uint64_t count;

	if ( kuzu_connection_get_max_num_thread_for_exec( &ptr->conn, &count ) != KuzuSuccess ) {
		rb_raise( rkuzu_eError, "kuzu_connection_get_max_num_thread_for_exec failed" );
	}

	return ULONG2NUM( count );
}


/*
 * call-seq:
 *    connection.max_num_threads_for_exec = integer
 *
 * Sets the maximum number of threads of the connection to use for
 * executing queries.
 *
 */
static VALUE
rkuzu_connection_max_num_threads_for_exec_eq( VALUE self, VALUE count )
{
	rkuzu_connection *ptr = CHECK_CONNECTION( self );
	uint64_t thread_count = NUM2ULONG( count );

	if ( kuzu_connection_set_max_num_thread_for_exec( &ptr->conn, thread_count ) != KuzuSuccess ) {
		rb_raise( rkuzu_eError, "kuzu_connection_set_max_num_thread_for_exec failed" );
	}

	return Qtrue;
}


/*
 * call-seq:
 *    connection.query_timeout = integer
 *
 * Sets query timeout value in milliseconds for the connection.
 *
 */
static VALUE
rkuzu_connection_query_timeout_eq( VALUE self, VALUE timeout )
{
	rkuzu_connection *ptr = CHECK_CONNECTION( self );
	uint64_t timeout_in_ms = NUM2ULONG( timeout );

	if ( kuzu_connection_set_query_timeout( &ptr->conn, timeout_in_ms ) != KuzuSuccess ) {
		rb_raise( rkuzu_eError, "kuzu_connection_set_query_timeout failed" );
	}

	return Qtrue;
}



/*
 * Document-class: Kuzu::Connection
 */
void
rkuzu_init_connection( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuConnection = rb_define_class_under( rkuzu_mKuzu, "Connection", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuConnection, rkuzu_connection_s_allocate );

	rb_define_protected_method( rkuzu_cKuzuConnection, "initialize", rkuzu_connection_initialize, 1 );

	rb_define_protected_method( rkuzu_cKuzuConnection, "_query", rkuzu_connection__query, 1 );
	rb_define_method( rkuzu_cKuzuConnection, "query!", rkuzu_connection_query_bang, 1 );
	rb_define_alias( rkuzu_cKuzuConnection, "run", "query!" );

	rb_define_method( rkuzu_cKuzuConnection, "max_num_threads_for_exec",
		rkuzu_connection_max_num_threads_for_exec, 0 );
	rb_define_method( rkuzu_cKuzuConnection, "max_num_threads_for_exec=",
		rkuzu_connection_max_num_threads_for_exec_eq, 1 );

	rb_define_method( rkuzu_cKuzuConnection, "query_timeout=", rkuzu_connection_query_timeout_eq, 1 );

	rb_require( "kuzu/connection" );
}
