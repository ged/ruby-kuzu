/*
 *  query_summary.c - Kuzu::QuerySummary class
 *
 */

#include "kuzu_ext.h"


VALUE rkuzu_cKuzuQuerySummary;


static void rkuzu_query_summary_free( void * );


static const rb_data_type_t rkuzu_query_summary_type = {
	.wrap_struct_name = "Kuzu::QuerySummary",
	.function = {
		.dfree = rkuzu_query_summary_free,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_query_summary(self) \
	((kuzu_query_summary*)rb_check_typeddata((self), &rkuzu_query_summary_type))


static void
rkuzu_query_summary_free( void *ptr )
{
	kuzu_query_summary *query_summary = (kuzu_query_summary *)ptr;

	if ( ptr ) {
		fprintf( stderr, ">>> freeing query summary %p\n", ptr );
		kuzu_query_summary_destroy( query_summary );
		xfree( ptr );
	}
}



/*
 * ::allocate function
 */
static VALUE
rkuzu_query_summary_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_query_summary_type, NULL );
}


/*
 * call-seq:
 *    Kuzu::QuerySummary.from_result( result )   -> query_summary
 *
 * Return a Kuzu::QuerySummary from a Kuzu::QueryResult.
 *
 */
static VALUE
rkuzu_query_summary_s_from_result( VALUE klass, VALUE query_result )
{
	kuzu_query_result *result = rkuzu_get_result( query_result );

	kuzu_query_summary *query_summary = ALLOC( kuzu_query_summary );
	VALUE query_summary_obj = rb_class_new_instance( 0, 0, klass );

	/*
		TODO Release the GIL
	*/
	if ( kuzu_query_result_get_query_summary(result, query_summary) != KuzuSuccess ) {
		xfree( query_summary );
		query_summary = NULL;
		rb_raise( rkuzu_eQueryError, "Could not fetch the query summary." );
	}

	fprintf( stderr, ">>> allocated query summary %p\n", query_summary );
	DATA_PTR( query_summary_obj ) = query_summary;

	return query_summary_obj;
}


/**
 * call-seq:
 *    summary.compiling_time             -> float
 *
 * Returns the compilation time of the given query summary in milliseconds.
 *
 */
static VALUE
rkuzu_query_summary_compiling_time( VALUE self )
{
	kuzu_query_summary *summary = check_query_summary( self );
	double result = kuzu_query_summary_get_compiling_time( summary );

	return rb_float_new( result );
}


/**
 * call-seq:
 *    summary.execution_time             -> float
 *
 * Returns the execution time of the given query summary in milliseconds.
 *
 */
static VALUE
rkuzu_query_summary_execution_time( VALUE self )
{
	kuzu_query_summary *summary = check_query_summary( self );
	double result = kuzu_query_summary_get_execution_time( summary );

	return rb_float_new( result );
}


/*
 * Document-class: Kuzu::QuerySummary
 */
void
rkuzu_init_query_summary( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuQuerySummary = rb_define_class_under( rkuzu_mKuzu, "QuerySummary", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuQuerySummary, rkuzu_query_summary_s_allocate );
	rb_undef_method( CLASS_OF(rkuzu_cKuzuQuerySummary), "new" );

	rb_define_singleton_method( rkuzu_cKuzuQuerySummary, "from_result",
		rkuzu_query_summary_s_from_result, 1 );
	rb_define_alias( CLASS_OF(rkuzu_cKuzuQuerySummary), "from_query_result", "from_result" );

	rb_define_method( rkuzu_cKuzuQuerySummary, "compiling_time",
		rkuzu_query_summary_compiling_time, 0 );
	rb_define_method( rkuzu_cKuzuQuerySummary, "execution_time",
		rkuzu_query_summary_execution_time, 0 );

}
