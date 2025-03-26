/*
 *  connection.c - Kuzu::Connection class
 *
 */

#include "kuzu_ext.h"


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
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_connection(self) ((rkuzu_connection *)rb_check_typeddata((self), &rkuzu_connection_type))


rkuzu_connection *
rkuzu_get_connection( VALUE conn_obj )
{
	return check_connection( conn_obj );
}


static rkuzu_connection *
rkuzu_connection_alloc( void )
{
	rkuzu_connection *ptr = ALLOC( rkuzu_connection );

	ptr->queries = rb_ary_new();
	ptr->statements = rb_ary_new();
	ptr->destroyed = false;

	return ptr;
}

static void
rkuzu_connection_free( void *ptr )
{
	rkuzu_connection *conn_s = (rkuzu_connection *)ptr;

	if ( ptr && !conn_s->destroyed ) {
		fprintf( stderr, ">>> freeing connection %p\n", ptr );
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
		fprintf( stderr, ">>> marking connection %p\n", ptr );
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
	rkuzu_connection *ptr = check_connection( self );

	if ( !ptr ) {
		rkuzu_database *dbobject = rkuzu_get_database( database );
		ptr = rkuzu_connection_alloc();

		if ( kuzu_connection_init(&dbobject->db, &ptr->conn) != KuzuSuccess ) {
			xfree( ptr );
			ptr = NULL;
			rb_raise( rkuzu_eConnectionError, "Failed to connect!" );
		}

		fprintf( stderr, ">>> allocated connection %p\n", ptr );
		DATA_PTR( self ) = ptr;
		rb_ary_push( dbobject->connections, self );
	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit connection" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
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
	rkuzu_connection *ptr = check_connection( self );
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
	rkuzu_connection *ptr = check_connection( self );
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
	rkuzu_connection *ptr = check_connection( self );
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

	rb_define_method( rkuzu_cKuzuConnection, "max_num_threads_for_exec",
		rkuzu_connection_max_num_threads_for_exec, 0 );
	rb_define_method( rkuzu_cKuzuConnection, "max_num_threads_for_exec=",
		rkuzu_connection_max_num_threads_for_exec_eq, 1 );

	rb_define_method( rkuzu_cKuzuConnection, "query_timeout=", rkuzu_connection_query_timeout_eq, 1 );

}
