/*
 *  database.c - Kuzu::Database class
 */

#include "kuzu_ext.h"

#define CHECK_DATABASE(self) ((rkuzu_database*)rb_check_typeddata((self), &rkuzu_database_type))
// #define DEBUG_GC(msg, ptr) fprintf( stderr, msg, ptr )
#define DEBUG_GC(msg, ptr)


VALUE rkuzu_cKuzuDatabase;

static void rkuzu_database_free( void * );
static void rkuzu_database_mark( void * );

static const rb_data_type_t rkuzu_database_type = {
	.wrap_struct_name = "Kuzu::Database",
	.function = {
		.dmark = rkuzu_database_mark,
		.dfree = rkuzu_database_free,
	},
	.data = NULL,
};


rkuzu_database *
rkuzu_get_database( VALUE obj )
{
	return CHECK_DATABASE( obj );
}


/*
 * Struct allocation/init function.
 */
static rkuzu_database *
rkuzu_database_alloc( void )
{
	rkuzu_database *ptr = ALLOC( rkuzu_database );

	ptr->path = Qnil;
	ptr->config = Qnil;

	return ptr;
}


/*
 * Free function
 */
static void
rkuzu_database_free( void *ptr )
{
	if ( ptr ) {
		DEBUG_GC( ">>> freeing database %p\n", ptr );
		rkuzu_database *database_s = (rkuzu_database *)ptr;

		kuzu_database_destroy( &database_s->db );

		database_s->path = Qnil;
		database_s->config = Qnil;

		xfree( ptr );
		ptr = NULL;
	}
}


/*
 * Mark function
 */
static void
rkuzu_database_mark( void *ptr )
{
	rkuzu_database *database_s = (rkuzu_database *)ptr;

	DEBUG_GC( ">>> marking database %p\n", ptr );

	rb_gc_mark( database_s->path );
	rb_gc_mark( database_s->config );
}



/*
 * ::allocate function
 */
static VALUE
rkuzu_database_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_database_type, NULL );
}


/*
 * call-seq:
 *    database.new( path, **options )   -> database
 *
 * Create a new Datbase using the given +path+ and +options+.
 *
 */
static VALUE
rkuzu_database_initialize( int argc, VALUE *argv, VALUE self )
{
	rkuzu_database *ptr = CHECK_DATABASE( self );

	if ( !ptr ) {
		VALUE path, options, config;
		VALUE config_argv[1];
		kuzu_system_config *sysconfig;
		char *database_path;

		rb_scan_args( argc, argv, "1:", &path, &options );
		config_argv[0] = options;
		config = rb_funcallv_public_kw( rkuzu_cKuzuConfig, rb_intern("from_options"), 1,
			config_argv, RB_PASS_KEYWORDS );

		sysconfig = rkuzu_get_config( config );
		database_path = StringValueCStr( path );

		ptr = rkuzu_database_alloc();
		if ( kuzu_database_init(database_path, *sysconfig, &ptr->db) != KuzuSuccess ) {
			xfree( ptr );
			ptr = NULL;

			rb_raise( rkuzu_eDatabaseError, "Couldn't create database!" );
		}

		DEBUG_GC( ">>> allocated database %p\n", ptr );
		RTYPEDDATA_DATA( self ) = ptr;

		ptr->path = rb_obj_freeze( rb_obj_dup(path) );
		ptr->config = rb_obj_freeze( config );
	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit database" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
}


/*
 * call-seq:
 *    database.config()   -> config
 *
 * Return the Kuzu::Config that reflects the config options the database
 * was created with.
 *
 */
static VALUE
rkuzu_database_config( VALUE self )
{
	rkuzu_database *ptr = CHECK_DATABASE( self );
	return ptr->config;
}



/*
 * call-seq:
 *    database.path()   -> string or nil
 *
 * Return the path the database was created with, or +nil+ if it was
 * created in memory.
 *
 */
static VALUE
rkuzu_database_path( VALUE self )
{
	rkuzu_database *ptr = CHECK_DATABASE( self );

	if ( RSTRING_LEN(ptr->path) == 0 ) {
		return Qnil;
	} else {
		return ptr->path;
	}
}


void
rkuzu_init_database( void )
{
	rkuzu_cKuzuDatabase = rb_define_class_under( rkuzu_mKuzu, "Database", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuDatabase, rkuzu_database_s_allocate );

	rb_define_method( rkuzu_cKuzuDatabase, "initialize", rkuzu_database_initialize, -1 );

	rb_define_method( rkuzu_cKuzuDatabase, "config", rkuzu_database_config, 0 );
	rb_define_method( rkuzu_cKuzuDatabase, "path", rkuzu_database_path, 0 );

	rb_require( "kuzu/database" );
}
