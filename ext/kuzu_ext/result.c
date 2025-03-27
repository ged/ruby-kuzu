/*
 *  result.c - Kuzu::Result class
 *
 */

#include "kuzu.h"
#include "kuzu_ext.h"
#include <stdio.h>

VALUE rkuzu_cKuzuResult;


static void rkuzu_result_free( void * );
static void rkuzu_result_mark( void * );


static const rb_data_type_t rkuzu_result_type = {
	.wrap_struct_name = "Kuzu::Result",
	.function = {
		.dfree = rkuzu_result_free,
		.dmark = rkuzu_result_mark,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_result(self) ((rkuzu_query_result *)rb_check_typeddata((self), &rkuzu_result_type))


/*
 * Fetch function
 */
rkuzu_query_result *
rkuzu_get_result( VALUE result_obj )
{
	return check_result( result_obj );
}


/*
 * Allocation function
 */
static rkuzu_query_result *
rkuzu_result_alloc( void )
{
	rkuzu_query_result *ptr = ALLOC( rkuzu_query_result );

	ptr->connection = Qnil;
	ptr->query = Qnil;
	ptr->statement = Qnil;
	ptr->previous_result = Qnil;
	ptr->destroyed = false;

	return ptr;
}


/*
 * dmark function
 */
static void
rkuzu_result_mark( void *ptr )
{
	rkuzu_query_result *result = (rkuzu_query_result *)ptr;

	if ( ptr ) {
		rb_gc_mark( result->connection );
		rb_gc_mark( result->query );
		rb_gc_mark( result->statement );
		rb_gc_mark( result->previous_result );
	}
}


/*
 * dfree function
 */
static void
rkuzu_result_free( void *ptr )
{
	rkuzu_query_result *result = (rkuzu_query_result *)ptr;

	if ( ptr ) {
		fprintf( stderr, ">>> freeing result %p\n", ptr );
		if ( !result->destroyed ) {
			kuzu_query_result_destroy( &result->result );
		}

		xfree( ptr );
		ptr = NULL;
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
	rkuzu_connection *conn = rkuzu_get_connection( connection );
	const char *query_s = StringValueCStr( query );
	rkuzu_query_result *result = rkuzu_result_alloc();

	/*
		TODO Release the GIL
	*/
	if ( kuzu_connection_query(&conn->conn, query_s, &result->result) != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &result->result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not execute query `%s': %s.", query_s, err_detail );

		xfree( result );
		result = NULL;
		kuzu_destroy_string( err_detail );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	fprintf( stderr, ">>> allocated result %p\n", result );

	VALUE result_obj = rb_class_new_instance( 0, 0, klass );
	DATA_PTR( result_obj ) = result;

	result->connection = connection;
	result->query = query;

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
rkuzu_result_s_from_prepared_statement( VALUE klass, VALUE statement )
{
	VALUE connection = rb_funcall( statement, rb_intern("connection"), 0 );
	rkuzu_connection *conn = rkuzu_get_connection( connection );
	kuzu_prepared_statement *stmt = rkuzu_get_prepared_statement( statement );
	rkuzu_query_result *result = rkuzu_result_alloc();

	/*
		TODO Release the GIL
	*/
	if ( kuzu_connection_execute(&conn->conn, stmt, &result->result) != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &result->result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not execute prepared statement: %s.", err_detail );

		xfree( result );
		result = NULL;
		kuzu_destroy_string( err_detail );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	VALUE result_obj = rb_class_new_instance( 0, 0, klass );
	DATA_PTR( result_obj ) = result;

	result->connection = connection;
	result->statement = statement;

	return result_obj;
}



/*
 * call-seq:
 *    Kuzu::Result.from_next_set( result )   -> result2
 *
 * Return a Kuzu::Result for the next result set after the specified +result+.
 * Returns `nil` if there is no next result set..
 *
 */
static VALUE
rkuzu_result_s_from_next_set( VALUE klass, VALUE result )
{
	rkuzu_query_result *start_result = rkuzu_get_result( result );
	rkuzu_query_result *next_result;
	VALUE result_obj;

	if ( !kuzu_query_result_has_next_query_result(&start_result->result) ) {
		return Qnil;
	}

	next_result = rkuzu_result_alloc();

	if ( kuzu_query_result_get_next_query_result(&start_result->result, &next_result->result) != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &next_result->result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not fetch next query result set: %s.", err_detail );

		xfree( next_result );
		next_result = NULL;
		kuzu_destroy_string( err_detail );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	result_obj = rb_class_new_instance( 0, 0, klass );
	DATA_PTR( result_obj ) = next_result;

	next_result->connection = start_result->connection;
	next_result->previous_result = result;

	return result_obj;
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
	rkuzu_query_result *result = check_result( self );

	if ( kuzu_query_result_is_success(&result->result) ) {
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
	rkuzu_query_result *result = check_result( self );
	const uint64_t count = kuzu_query_result_get_num_columns( &result->result );

	return ULONG2NUM( count );
}


/*
 * call-seq:
 *    result.num_tuples   -> integer
 *
 * Returns the number of tuples in the query result.
 *
 */
static VALUE
rkuzu_result_num_tuples( VALUE self )
{
	rkuzu_query_result *result = check_result( self );
	const uint64_t count = kuzu_query_result_get_num_tuples( &result->result );

	return ULONG2NUM( count );
}


/*
 * call-seq:
 *    result.to_s   -> string
 *
 * Returns the result as a String.
 *
 */
static VALUE
rkuzu_result_to_s( VALUE self )
{
	rkuzu_query_result *result = check_result( self );
	char *string = kuzu_query_result_to_string( &result->result );

	VALUE rval = rb_str_new2( string );
	kuzu_destroy_string( string );

	return rval;
}


/*
 * call-seq:
 *    result.has_next?   -> true or false
 *
 * Returns true if we have not consumed all tuples in the query result, false
 * otherwise.
 *
 */
static VALUE
rkuzu_result_has_next_p( VALUE self )
{
	rkuzu_query_result *result = check_result( self );

	if ( kuzu_query_result_has_next(&result->result) ) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}


/*
 * call-seq:
 *    result.has_next_set?   -> true or false
 *
 * Returns true if there was more than one result set in the results, and
 * the current set is not the last one. You can call #next_set to move on to
 * the next result set.
 *
 */
static VALUE
rkuzu_result_has_next_set_p( VALUE self )
{
	rkuzu_query_result *result = check_result( self );

	if ( kuzu_query_result_has_next_query_result(&result->result) ) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}


/*
 * call-seq:
 *    result.reset_iterator
 *
 * Resets the iterator of the query result to the beginning.
 *
 */
static VALUE
rkuzu_result_reset_iterator( VALUE self )
{
	rkuzu_query_result *result = check_result( self );
	kuzu_query_result_reset_iterator( &result->result );
	return Qtrue;
}


/*
 * call-seq:
 *    result.get_next_values
 *
 * Returns the next tuple of the query result values if there is one, otherwise
 * returns `nil`.
 *
 */
static VALUE
rkuzu_result_get_next_values( VALUE self )
{
	rkuzu_query_result *result = check_result( self );
	kuzu_flat_tuple tuple;
	kuzu_logical_type column_type;
	kuzu_value column_value;
	uint64_t column_count = kuzu_query_result_get_num_columns( &result->result );
	VALUE current_value = Qnil,
	      rval = rb_ary_new();

	if ( !kuzu_query_result_has_next(&result->result) ) {
		return Qnil;
	}

	if ( kuzu_query_result_get_next(&result->result, &tuple) != KuzuSuccess ) {
		char *err_detail = kuzu_query_result_get_error_message( &result->result );
		char errmsg[ 4096 ] = "\0";

		snprintf( errmsg, 4096, "Could not fetch next tuple: %s.", err_detail );

		kuzu_destroy_string( err_detail );

		rb_raise( rkuzu_eQueryError, "%s", errmsg );
	}

	for ( uint64_t i = 0 ; i < column_count ; i++ ) {
		kuzu_query_result_get_column_data_type( &result->result, i, &column_type );
		kuzu_flat_tuple_get_value( &tuple, i, &column_value );

		current_value = rkuzu_convert_logical_kuzu_value_to_ruby( &column_type, &column_value );
		rb_ary_push( rval, current_value );
	}

	kuzu_flat_tuple_destroy( &tuple );

	return rval;
}


/*
 * call-seq:
 *    result.get_column_names
 *
 * Returns the names of the columns of the results as an Array of Strings.
 *
 */
static VALUE
rkuzu_result_get_column_names( VALUE self )
{
	rkuzu_query_result *result = check_result( self );
	uint64_t col_count = kuzu_query_result_get_num_columns( &result->result );
	char *name;
	VALUE rval = rb_ary_new();

	for ( uint64_t i = 0 ; i < col_count ; i++ ) {
		if ( kuzu_query_result_get_column_name(&result->result, i, &name) != KuzuSuccess ) {
			rb_raise( rkuzu_eError, "couldn't fetch name of column %llu", i );
		}
		rb_ary_push( rval, rb_str_new2(name) );
	}

	return rval;
}


/*
 * Document-class: Kuzu::Rhodesult
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
		rkuzu_result_s_from_prepared_statement, 1 );
	rb_define_singleton_method( rkuzu_cKuzuResult, "from_next_set", rkuzu_result_s_from_next_set, 1 );

	rb_define_method( rkuzu_cKuzuResult, "success?", rkuzu_result_success_p, 0 );

	rb_define_method( rkuzu_cKuzuResult, "num_columns", rkuzu_result_num_columns, 0 );
	rb_define_method( rkuzu_cKuzuResult, "num_tuples", rkuzu_result_num_tuples, 0 );
	rb_define_method( rkuzu_cKuzuResult, "to_s", rkuzu_result_to_s, 0 );

	rb_define_method( rkuzu_cKuzuResult, "has_next?", rkuzu_result_has_next_p, 0 );
	rb_define_method( rkuzu_cKuzuResult, "has_next_set?", rkuzu_result_has_next_set_p, 0 );

	rb_define_method( rkuzu_cKuzuResult, "reset_iterator", rkuzu_result_reset_iterator, 0 );
	rb_define_method( rkuzu_cKuzuResult, "get_next_values", rkuzu_result_get_next_values, 0 );
	rb_define_method( rkuzu_cKuzuResult, "get_column_names", rkuzu_result_get_column_names, 0 );
}
