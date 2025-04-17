/*
 *  kuzu_ext.c - Ruby binding for Kùzu
 *
 *  Authors:
 *    * Michael Granger <ged@FaerieMUD.org>
 *
 *  Refs:
 *  - https://docs.kuzudb.com/
 *
 */

#include "kuzu_ext.h"

VALUE rkuzu_mKuzu;

VALUE rkuzu_eError;
VALUE rkuzu_eDatabaseError;
VALUE rkuzu_eConnectionError;
VALUE rkuzu_eQueryError;
VALUE rkuzu_eFinishedError;

VALUE rkuzu_rb_cDate;
VALUE rkuzu_rb_cOstruct;


/* --------------------------------------------------------------
 * Logging Functions
 * -------------------------------------------------------------- */

/*
 * Log a message to the given +context+ object's logger.
 */
void
#if HAVE_STDARG_PROTOTYPES
rkuzu_log_obj( VALUE context, const char *level, const char *fmt, ... )
#else
rkuzu_log_obj( VALUE context, const char *level, const char *fmt, va_dcl )
#endif
{
	char buf[BUFSIZ];
	va_list	args;
	VALUE logger = Qnil;
	VALUE message = Qnil;

	va_init_list( args, fmt );
	vsnprintf( buf, BUFSIZ, fmt, args );
	message = rb_str_new2( buf );

	logger = rb_funcall( context, rb_intern("log"), 0 );
	rb_funcall( logger, rb_intern(level), 1, message );

	va_end( args );
}


/*
 * Log a message to the global logger.
 */
void
#if HAVE_STDARG_PROTOTYPES
rkuzu_log( const char *level, const char *fmt, ... )
#else
rkuzu_log( const char *level, const char *fmt, va_dcl )
#endif
{
	char buf[BUFSIZ];
	va_list	args;
	VALUE logger = Qnil;
	VALUE message = Qnil;

	va_init_list( args, fmt );
	vsnprintf( buf, BUFSIZ, fmt, args );
	message = rb_str_new2( buf );

	logger = rb_funcall( rkuzu_mKuzu, rb_intern("logger"), 0 );
	rb_funcall( logger, rb_intern(level), 1, message );

	va_end( args );
}



/* --------------------------------------------------------------
 * Module methods
 * -------------------------------------------------------------- */

/*
 * call-seq:
 *    Kuzu.kuzu_version   -> string
 *
 * Return the version of the underlying Kuzu library.
 *
 */
static VALUE
rkuzu_s_kuzu_version( VALUE _ )
{
	const char *version = kuzu_get_version();

	return rb_str_new2( version );
}


/*
 * call-seq:
 *    Kuzu.storage_version   -> integer
 *
 * Return the storage version used by the underlying library.
 *
 */
static VALUE
rkuzu_s_storage_version( VALUE _ )
{
	const unsigned long long version = kuzu_get_storage_version();

	return ULONG2NUM( version );
}


/*
 * Kuzu extension init function
 */
void
Init_kuzu_ext( void )
{
	rb_require( "date" );
	rkuzu_rb_cDate = rb_const_get( rb_cObject, rb_intern("Date") );

	rb_require( "ostruct" );
	rkuzu_rb_cOstruct = rb_const_get( rb_cObject, rb_intern("OpenStruct") );

	/*
	 * Document-module: Kuzu
	 *
	 * The top level namespace for Kuzu classes.
	 */
	rkuzu_mKuzu = rb_define_module( "Kuzu" );

	rb_define_singleton_method( rkuzu_mKuzu, "kuzu_version", rkuzu_s_kuzu_version, 0 );
	rb_define_singleton_method( rkuzu_mKuzu, "storage_version", rkuzu_s_storage_version, 0 );

	rkuzu_eError = rb_define_class_under( rkuzu_mKuzu, "Error", rb_eRuntimeError );
	rkuzu_eDatabaseError = rb_define_class_under( rkuzu_mKuzu, "DatabaseError", rkuzu_eError );
	rkuzu_eConnectionError = rb_define_class_under( rkuzu_mKuzu, "ConnectionError", rkuzu_eError );
	rkuzu_eQueryError = rb_define_class_under( rkuzu_mKuzu, "QueryError", rkuzu_eError );
	rkuzu_eFinishedError = rb_define_class_under( rkuzu_mKuzu, "FinishedError", rkuzu_eError );

	rb_require( "kuzu" );

	rkuzu_init_database();
	rkuzu_init_config();
	rkuzu_init_connection();
	rkuzu_init_prepared_statement();
	rkuzu_init_result();
	rkuzu_init_query_summary();
	rkuzu_init_node();
	rkuzu_init_rel();
	rkuzu_init_recursive_rel();
}
