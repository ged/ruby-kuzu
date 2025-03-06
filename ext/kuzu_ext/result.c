/*
 *  result.c - Kuzu::Result class
 *
 */

#include "kuzu_ext.h"


VALUE rkuzu_cKuzuResult;


static void rkuzu_result_free( void * );


static const rb_data_type_t rkuzu_result_type = {
	.wrap_struct_name = "Kuzu::Result",
	.function = {
		.dfree = rkuzu_result_free,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_result(self) ((kuzu_query_result*)rb_check_typeddata((self), &rkuzu_result_type))


/*
 * Fetch function
 */
kuzu_query_result *
rkuzu_get_result( VALUE result_obj )
{
	return check_result( result_obj );
}


/*
 * dfree function
 */
static void
rkuzu_result_free( void *ptr )
{
	kuzu_query_result *result = (kuzu_query_result *)ptr;

	if ( ptr ) {
		kuzu_query_result_destroy( result );
		xfree( ptr );
	}
}



/*
 * ::allocate function
 */
static VALUE
rkuzu_result_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_result_type, NULL );
}


/*
 * call-seq:
 *    Kuzu::Result.from_query( connection, query )   -> result
 *
 * Return a Kuzu::Result from executing the given +query+ on the specified
 * +connection+.
 *
 */
static VALUE
rkuzu_result_s_from_query( VALUE klass, VALUE connection, VALUE query )
{
	kuzu_connection *conn = rkuzu_get_connection( connection );
	const char *query_s = StringValueCStr( query );

	kuzu_query_result *result = ALLOC( kuzu_query_result );
	VALUE result_obj = rb_class_new_instance( 0, 0, klass );

	/*
		TODO Release the GIL
	*/
	if ( kuzu_connection_query(conn, query_s, result) != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not execute query `%s': %s.", query_s, err_detail );

		xfree( result );
		result = NULL;
		kuzu_destroy_string( err_detail );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	DATA_PTR( result_obj ) = result;

	return result_obj;
}


/*
 * call-seq:
 *    Kuzu::Result.from_prepared_statement( conn, statement )   -> result
 *
 * Return a Kuzu::Result from executing the given +statement+ on the specified
 * +conn+.
 *
 */
static VALUE
rkuzu_result_s_from_prepared_statement( VALUE klass, VALUE conn, VALUE statement )
{
	rb_notimplement();
}


/*
 * call-seq:
 *    result.success?   -> true or false
 *
 * Returns true if the query is executed successful, false otherwise.
 *
 */
static VALUE
rkuzu_result_success_p( VALUE self )
{
	kuzu_query_result *result = check_result( self );

	if ( kuzu_query_result_is_success(result) ) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}


/*
 * call-seq:
 *    result.num_columns   -> integer
 *
 * Returns the number of columns in the query result.
 *
 */
static VALUE
rkuzu_result_num_columns( VALUE self )
{
	kuzu_query_result *result = check_result( self );
	const uint64_t count = kuzu_query_result_get_num_columns( result );

	return ULONG2NUM( count );
}


/*
 * Document-class: Kuzu::Result
 */
void
rkuzu_init_result( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuResult = rb_define_class_under( rkuzu_mKuzu, "Result", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuResult, rkuzu_result_s_allocate );
	rb_undef_method( CLASS_OF(rkuzu_cKuzuResult), "new" );

	rb_define_singleton_method( rkuzu_cKuzuResult, "from_query", rkuzu_result_s_from_query, 2 );
	rb_define_singleton_method( rkuzu_cKuzuResult, "from_prepared_statement",
		rkuzu_result_s_from_prepared_statement, 2 );

	rb_define_method( rkuzu_cKuzuResult, "success?", rkuzu_result_success_p, 0 );

	rb_define_method( rkuzu_cKuzuResult, "num_columns", rkuzu_result_num_columns, 0 );
}
