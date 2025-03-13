/*
 *  prepared_statement.c - Kuzu::PreparedStatement class
 *
 */

#include "kuzu_ext.h"

VALUE rkuzu_cKuzuPreparedStatement;
static ID CONNECTION_IVAR;

static void rkuzu_prepared_statement_free( void * );
static void rkuzu_bind_string( kuzu_prepared_statement *, const char *, VALUE );

static const rb_data_type_t rkuzu_prepared_statement_type = {
	.wrap_struct_name = "Kuzu::PreparedStatement",
	.function = {
		.dfree = rkuzu_prepared_statement_free,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_prepared_statement( self ) \
	((kuzu_prepared_statement *)rb_check_typeddata( (self), &rkuzu_prepared_statement_type) )


/*
 * Fetch function
 */
kuzu_prepared_statement *
rkuzu_get_prepared_statement( VALUE prepared_statement_obj )
{
	return check_prepared_statement( prepared_statement_obj );
}


/*
 * dfree function
 */
static void
rkuzu_prepared_statement_free( void *ptr )
{
	kuzu_prepared_statement *prepared_statement = (kuzu_prepared_statement *)ptr;

	if ( ptr ) {
		kuzu_prepared_statement_destroy( prepared_statement );
		xfree( ptr );
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
	kuzu_prepared_statement *stmt = check_prepared_statement( self );

	if ( !stmt ) {
		kuzu_connection *conn = rkuzu_get_connection( connection );
		const char *query_s = StringValueCStr( query );

		stmt = ALLOC( kuzu_prepared_statement );

		if ( kuzu_connection_prepare(conn, query_s, stmt) != KuzuSuccess ) {
			char *err_detail = kuzu_prepared_statement_get_error_message( stmt );
			char errmsg[ 4096 ] = "\0";

			snprintf( errmsg, 4096, "Could not prepare query `%s': %s.", query_s, err_detail );

			xfree( stmt );
			stmt = NULL;
			kuzu_destroy_string( err_detail );

			rb_raise( rkuzu_eQueryError, "%s", errmsg );
		}

		DATA_PTR( self ) = stmt;
		rb_ivar_set( self, CONNECTION_IVAR, connection );

	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit connection" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
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
	kuzu_prepared_statement *stmt = check_prepared_statement( self );

	if ( kuzu_prepared_statement_is_success(stmt) ) {
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
	kuzu_prepared_statement *stmt = check_prepared_statement( self );
	VALUE name_string = rb_funcall( name, rb_intern("to_s"), 0 );
	const char *name_s = StringValueCStr( name_string );

	switch (TYPE(value)) {
		case T_TRUE:
		case T_FALSE:
			kuzu_prepared_statement_bind_bool( stmt, name_s, RTEST(value) );
			break;

		// fallthrough
		case T_FLOAT:
			kuzu_prepared_statement_bind_float( stmt, name_s, NUM2DBL(value) );
			break;

		case T_BIGNUM:
			kuzu_prepared_statement_bind_int64( stmt, name_s, NUM2LL(value) );
			break;

		case T_FIXNUM:
			kuzu_prepared_statement_bind_int32( stmt, name_s, NUM2INT(value) );
			break;

		case T_SYMBOL:
			rb_notimplement();
			break; // not reached

		case T_NIL:
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
rkuzu_bind_string( kuzu_prepared_statement *stmt, const char *name_s, VALUE value )
{
	const char *value_s = StringValueCStr( value );
	kuzu_prepared_statement_bind_string( stmt, name_s, value_s );
}


/*
 * Document-class: Kuzu::PreparedStatement
 */
void
rkuzu_init_prepared_statement( void )
{
	CONNECTION_IVAR = rb_intern( "@connection" );

#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuPreparedStatement = rb_define_class_under( rkuzu_mKuzu, "PreparedStatement", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuPreparedStatement, rkuzu_prepared_statement_s_allocate );

	rb_define_protected_method( rkuzu_cKuzuPreparedStatement, "initialize", rkuzu_prepared_statement_initialize, 2 );

	rb_define_attr( rkuzu_cKuzuPreparedStatement, "connection", 1, 0 );

	rb_define_method( rkuzu_cKuzuPreparedStatement, "success?", rkuzu_prepared_statement_success_p, 0 );
	rb_define_method( rkuzu_cKuzuPreparedStatement, "bind_variable", rkuzu_prepared_statement_bind_variable, 2 );
}
