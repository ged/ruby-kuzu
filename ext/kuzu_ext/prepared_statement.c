/*
 *  prepared_statement.c - Kuzu::PreparedStatement class
 *
 */

#include "kuzu.h"
#include "kuzu_ext.h"

#define CHECK_PREPARED_STATEMENT( self ) \
	((rkuzu_prepared_statement *)rb_check_typeddata( (self), &rkuzu_prepared_statement_type) )
// #define DEBUG_GC(msg, ptr) fprintf( stderr, msg, ptr )
#define DEBUG_GC(msg, ptr)


VALUE rkuzu_cKuzuPreparedStatement;

static void rkuzu_prepared_statement_free( void * );
static void rkuzu_prepared_statement_mark( void * );
static void rkuzu_bind_string( rkuzu_prepared_statement *, const char *, VALUE );

static const rb_data_type_t rkuzu_prepared_statement_type = {
	.wrap_struct_name = "Kuzu::PreparedStatement",
	.function = {
		.dfree = rkuzu_prepared_statement_free,
		.dmark = rkuzu_prepared_statement_mark,
	},
	.data = NULL,
};



/*
 * Fetch function
 */
rkuzu_prepared_statement *
rkuzu_get_prepared_statement( VALUE prepared_statement_obj )
{
	return CHECK_PREPARED_STATEMENT( prepared_statement_obj );
}


/*
 * Allocation function
 */
static rkuzu_prepared_statement *
rkuzu_prepared_statement_alloc()
{
	rkuzu_prepared_statement *ptr = ALLOC( rkuzu_prepared_statement );

	ptr->connection = Qnil;
	ptr->query = Qnil;

	return ptr;
}


/*
 * dmark function
 */
static void
rkuzu_prepared_statement_mark( void *ptr )
{
	rkuzu_prepared_statement *prepared_statement_s = (rkuzu_prepared_statement *)ptr;

	if ( ptr ) {
		rb_gc_mark( prepared_statement_s->connection );
		rb_gc_mark( prepared_statement_s->query );
	}
}


/*
 * dfree function
 */
static void
rkuzu_prepared_statement_free( void *ptr )
{
	if ( ptr ) {
		DEBUG_GC( ">>> freeing prepared statement %p\n", ptr );
		// Can't kuzu_prepared_statement_destroy here because the database or connection
		// might already have been destroyed.
		xfree( ptr );
		ptr = NULL;
	}
}


/*
 * ::allocate function
 */
static VALUE
rkuzu_prepared_statement_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_prepared_statement_type, NULL );
}


static VALUE
rkuzu_prepared_statement_initialize( VALUE self, VALUE connection, VALUE query )
{
	rkuzu_prepared_statement *stmt = CHECK_PREPARED_STATEMENT( self );

	if ( !stmt ) {
		rkuzu_connection *conn = rkuzu_get_connection( connection );
		const char *query_s = StringValueCStr( query );

		stmt = rkuzu_prepared_statement_alloc();

		if ( kuzu_connection_prepare(&conn->conn, query_s, &stmt->statement) != KuzuSuccess ) {
			char *err_detail = kuzu_prepared_statement_get_error_message( &stmt->statement );
			char errmsg[ 4096 ] = "\0";

			snprintf( errmsg, 4096, "Could not prepare query `%s': %s.", query_s, err_detail );

			xfree( stmt );
			stmt = NULL;
			kuzu_destroy_string( err_detail );

			rb_raise( rkuzu_eQueryError, "%s", errmsg );
		}

		DEBUG_GC( ">>> allocated prepared statement %p\n", stmt );
		RTYPEDDATA_DATA( self ) = stmt;

		stmt->connection = connection;
		stmt->query = query;

	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit prepared statement" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
}


struct execute_call {
	kuzu_connection *conn;
	kuzu_prepared_statement *stmt;
	kuzu_query_result *result;
};


static void *
rkuzu_connection_do_execute_without_gvl( void *ptr )
{
	struct execute_call *qcall = (struct execute_call *)ptr;
	kuzu_state state;

	state = kuzu_connection_execute( qcall->conn, qcall->stmt, qcall->result );

	return (void *)state;
}


static void
rkuzu_connection_cancel_execute( void *ptr )
{
	kuzu_connection *conn = (kuzu_connection *)ptr;
	kuzu_connection_interrupt( conn );
}


// Inner prepared statement constructor
static kuzu_query_result
rkuzu_prepared_statement_do_execute( VALUE self )
{
	rkuzu_prepared_statement *stmt = CHECK_PREPARED_STATEMENT( self );
	VALUE connection = stmt->connection;
	rkuzu_connection *conn = rkuzu_get_connection( connection );
	kuzu_query_result result;
	struct execute_call qcall;
	kuzu_state execute_state;
	void *result_ptr;

	qcall.conn = &conn->conn;
	qcall.stmt = &stmt->statement;
	qcall.result = &result;

	result_ptr = rb_thread_call_without_gvl(
		rkuzu_connection_do_execute_without_gvl, (void *)&qcall,
		rkuzu_connection_cancel_execute, (void *)&conn->conn );

	_Pragma("GCC diagnostic push")
	_Pragma("GCC diagnostic ignored \"-Wvoid-pointer-to-enum-cast\"")
	execute_state = (kuzu_state)result_ptr;
	_Pragma("GCC diagnostic pop")

	if ( execute_state != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not execute prepared statement: %s.", err_detail );

		kuzu_destroy_string( err_detail );
		kuzu_query_result_destroy( &result );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	return *qcall.result;
}


static VALUE
rkuzu_prepared_statement__execute( VALUE self )
{
	rkuzu_prepared_statement *stmt = CHECK_PREPARED_STATEMENT( self );
	kuzu_query_result result = rkuzu_prepared_statement_do_execute( self );

	return rkuzu_result_from_prepared_statement( rkuzu_cKuzuResult, stmt->connection, self, result );
}


static VALUE
rkuzu_prepared_statement__execute_bang( VALUE self )
{
	kuzu_query_result result = rkuzu_prepared_statement_do_execute( self );
	return kuzu_query_result_is_success( &result ) ? Qtrue : Qfalse;
}


/*
 * call-seq:
 *    statement.success?   -> true or false
 *
 * Returns +true+ if the query was prepared successfully.
 *
 */
static VALUE
rkuzu_prepared_statement_success_p( VALUE self )
{
	rkuzu_prepared_statement *stmt = CHECK_PREPARED_STATEMENT( self );

	if ( kuzu_prepared_statement_is_success(&stmt->statement) ) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}


/*
 * call-seq:
 *    statement.bind_variable( name, value )
 *
 * Binds the given +value+ to the given parameter +name+ in the prepared statement
 *
 */
static VALUE
rkuzu_prepared_statement_bind_variable( VALUE self, VALUE name, VALUE value )
{
	rkuzu_prepared_statement *stmt = CHECK_PREPARED_STATEMENT( self );
	VALUE name_string = rb_funcall( name, rb_intern("to_s"), 0 );
	const char *name_s = StringValueCStr( name_string );
	kuzu_value *null_value;

	switch (TYPE(value)) {
		case T_TRUE:
		case T_FALSE:
			kuzu_prepared_statement_bind_bool( &stmt->statement, name_s, RTEST(value) );
			break;

		// fallthrough
		case T_FLOAT:
			kuzu_prepared_statement_bind_float( &stmt->statement, name_s, NUM2DBL(value) );
			break;

		case T_BIGNUM:
			kuzu_prepared_statement_bind_int64( &stmt->statement, name_s, NUM2LL(value) );
			break;

		case T_FIXNUM:
			kuzu_prepared_statement_bind_int32( &stmt->statement, name_s, NUM2INT(value) );
			break;

		case T_SYMBOL:
			rb_notimplement();
			break; // not reached

		case T_NIL:
			null_value = kuzu_value_create_null();
			kuzu_prepared_statement_bind_value( &stmt->statement, name_s, null_value );
			kuzu_value_destroy( null_value );
			break;

		case T_OBJECT:
		case T_CLASS:
		case T_MODULE:
		case T_REGEXP:
		case T_ARRAY:
		case T_HASH:
		case T_STRUCT:
		case T_COMPLEX:
		case T_RATIONAL:
		case T_FILE:
		case T_DATA:
		case T_STRING:
		default:
			rkuzu_bind_string( stmt, name_s, value );
			break;

			// kuzu_prepared_statement_bind_int8
			// kuzu_prepared_statement_bind_int16
			// kuzu_prepared_statement_bind_uint64
			// kuzu_prepared_statement_bind_uint32
			// kuzu_prepared_statement_bind_uint16
			// kuzu_prepared_statement_bind_uint8

			// kuzu_prepared_statement_bind_double
			// kuzu_prepared_statement_bind_date
			// kuzu_prepared_statement_bind_timestamp_ns
			// kuzu_prepared_statement_bind_timestamp_sec
			// kuzu_prepared_statement_bind_timestamp_tz
			// kuzu_prepared_statement_bind_timestamp_ms
			// kuzu_prepared_statement_bind_timestamp
			// kuzu_prepared_statement_bind_interval
			// kuzu_prepared_statement_bind_value
	}

	return Qtrue;
}


static void
rkuzu_bind_string( rkuzu_prepared_statement *stmt, const char *name_s, VALUE value )
{
	const char *value_s = StringValueCStr( value );
	kuzu_prepared_statement_bind_string( &stmt->statement, name_s, value_s );
}


/*
 * call-seq:
 *    statement.connection   -> conn
 *
 * Return the Kuzu::Connection used to run this statement.
 *
 */
static VALUE
rkuzu_prepared_statement_connection( VALUE self )
{
	rkuzu_prepared_statement *statement_s = rkuzu_get_prepared_statement( self );
	return statement_s->connection;
}


/*
 * call-seq:
 *    statement.query   -> string
 *
 * Return the query string used to build this statement.
 *
 */
static VALUE
rkuzu_prepared_statement_query( VALUE self )
{
	rkuzu_prepared_statement *statement_s = rkuzu_get_prepared_statement( self );
	return statement_s->query;
}



/*
 * Document-class: Kuzu::PreparedStatement
 */
void
rkuzu_init_prepared_statement( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuPreparedStatement = rb_define_class_under( rkuzu_mKuzu, "PreparedStatement", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuPreparedStatement, rkuzu_prepared_statement_s_allocate );

	rb_define_protected_method( rkuzu_cKuzuPreparedStatement, "initialize",
		rkuzu_prepared_statement_initialize, 2 );
	rb_define_protected_method( rkuzu_cKuzuPreparedStatement, "_execute",
		rkuzu_prepared_statement__execute, 0 );
	rb_define_protected_method( rkuzu_cKuzuPreparedStatement, "_execute!",
		rkuzu_prepared_statement__execute_bang, 0 );

	rb_define_method( rkuzu_cKuzuPreparedStatement, "connection",
		rkuzu_prepared_statement_connection, 0 );
	rb_define_method( rkuzu_cKuzuPreparedStatement, "query",
		rkuzu_prepared_statement_query, 0 );

	rb_define_method( rkuzu_cKuzuPreparedStatement, "success?", rkuzu_prepared_statement_success_p, 0 );
	rb_define_method( rkuzu_cKuzuPreparedStatement, "bind_variable",
		rkuzu_prepared_statement_bind_variable, 2 );

	rb_require( "kuzu/prepared_statement" );
}
