/*
 *  config.c - Kuzu::Config class
 *
 */

#include "kuzu_ext.h"

#define CHECK_CONFIG(self) ((kuzu_system_config*)rb_check_typeddata((self), &rkuzu_config_type))


VALUE rkuzu_cKuzuConfig;

static const rb_data_type_t rkuzu_config_type = {
	.wrap_struct_name = "Kuzu::Config",
	.function = {},
	.data = NULL,
};



/*
 * ::allocate function
 */
static VALUE
rkuzu_config_s_allocate( VALUE klass )
{
	return TypedData_Wrap_Struct( klass, &rkuzu_config_type, NULL );
}


kuzu_system_config *
rkuzu_get_config( VALUE obj )
{
	return CHECK_CONFIG( obj );
}



/*
 * call-seq:
 *    new()   -> config
 *
 * Create a Kuzu::Config with default values.
 *
 */
static VALUE
rkuzu_config_initialize( VALUE self )
{
	kuzu_system_config *ptr = CHECK_CONFIG( self );

	if ( !ptr ) {
		ptr = ALLOC( kuzu_system_config );
		kuzu_system_config defaults = kuzu_default_system_config();

		ptr->buffer_pool_size     = defaults.buffer_pool_size;
		ptr->max_num_threads      = defaults.max_num_threads;
		ptr->enable_compression   = defaults.enable_compression;
		ptr->read_only            = defaults.read_only;
		ptr->max_db_size          = defaults.max_db_size;
		ptr->auto_checkpoint      = defaults.auto_checkpoint;
		ptr->checkpoint_threshold = defaults.checkpoint_threshold;

		RTYPEDDATA_DATA( self ) = ptr;
	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit config" );
	}

	rb_call_super( 0, 0 );

	return Qtrue;
}


/*
 * call-seq:
 *   config.buffer_pool_size()  -> integer
 *
 * Return the buffer_pool_size config value.
 */
static VALUE
rkuzu_config_buffer_pool_size( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return ULONG2NUM( config->buffer_pool_size );
}


/*
 * call-seq:
 *   config.max_num_threads()  -> integer
 *
 * Return the max_num_threads config value.
 */
static VALUE
rkuzu_config_max_num_threads( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return ULONG2NUM( config->max_num_threads );
}


/*
 * call-seq:
 *   config.enable_compression()  -> true or false
 *
 * Return the enable_compression config value.
 */
static VALUE
rkuzu_config_enable_compression( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return config->enable_compression ? Qtrue : Qfalse;
}


/*
 * call-seq:
 *   config.read_only()  -> true or false
 *
 * Return the read_only config value.
 */
static VALUE
rkuzu_config_read_only( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return config->read_only ? Qtrue : Qfalse;
}


/*
 * call-seq:
 *   config.max_db_size()  -> integer
 *
 * Return the max_db_size config value.
 */
static VALUE
rkuzu_config_max_db_size( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return ULONG2NUM( config->max_db_size );
}


/*
 * call-seq:
 *   config.auto_checkpoint()  -> true or false
 *
 * Return the auto_checkpoint config value.
 */
static VALUE
rkuzu_config_auto_checkpoint( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return config->auto_checkpoint ? Qtrue : Qfalse;
}


/*
 * call-seq:
 *   config.checkpoint_threshold()  -> integer
 *
 * Return the checkpoint_threshold config value.
 */
static VALUE
rkuzu_config_checkpoint_threshold( VALUE self )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	return ULONG2NUM( config->checkpoint_threshold );
}


/*
 * call-seq:
 *   config.buffer_pool_size = integer
 *
 * Set the buffer_pool_size config value.
 */
static VALUE
rkuzu_config_buffer_pool_size_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->buffer_pool_size = NUM2ULONG( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.max_num_threads = integer
 *
 * Set the max_num_threads config value.
 */
static VALUE
rkuzu_config_max_num_threads_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->max_num_threads = NUM2ULONG( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.enable_compression = true or false
 *
 * Set the enable_compression config value.
 */
static VALUE
rkuzu_config_enable_compression_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->enable_compression = RTEST( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.read_only = true or false
 *
 * Set the read_only config value.
 */
static VALUE
rkuzu_config_read_only_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->read_only = RTEST( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.max_db_size = integer
 *
 * Set the max_db_size config value.
 */
static VALUE
rkuzu_config_max_db_size_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->max_db_size = NUM2ULONG( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.auto_checkpoint = true or false
 *
 * Set the auto_checkpoint config value.
 */
static VALUE
rkuzu_config_auto_checkpoint_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->auto_checkpoint = RTEST( value );

	return Qtrue;
}


/*
 * call-seq:
 *   config.checkpoint_threshold = integer
 *
 * Set the checkpoint_threshold config value.
 */
static VALUE
rkuzu_config_checkpoint_threshold_eq( VALUE self, VALUE value )
{
	kuzu_system_config *config = CHECK_CONFIG( self );
	config->checkpoint_threshold = NUM2ULONG( value );

	return Qtrue;
}



/*
 * Document-class: Kuzu::Config
 */
void
rkuzu_init_config( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuConfig = rb_define_class_under( rkuzu_mKuzu, "Config", rb_cObject );

	rb_define_alloc_func( rkuzu_cKuzuConfig, rkuzu_config_s_allocate );

	rb_define_method( rkuzu_cKuzuConfig, "initialize", rkuzu_config_initialize, 0 );

	rb_define_method( rkuzu_cKuzuConfig, "buffer_pool_size", rkuzu_config_buffer_pool_size, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "max_num_threads", rkuzu_config_max_num_threads, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "enable_compression", rkuzu_config_enable_compression, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "read_only", rkuzu_config_read_only, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "max_db_size", rkuzu_config_max_db_size, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "auto_checkpoint", rkuzu_config_auto_checkpoint, 0 );
	rb_define_method( rkuzu_cKuzuConfig, "checkpoint_threshold", rkuzu_config_checkpoint_threshold, 0 );

	rb_define_method( rkuzu_cKuzuConfig, "buffer_pool_size=", rkuzu_config_buffer_pool_size_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "max_num_threads=", rkuzu_config_max_num_threads_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "enable_compression=", rkuzu_config_enable_compression_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "read_only=", rkuzu_config_read_only_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "max_db_size=", rkuzu_config_max_db_size_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "auto_checkpoint=", rkuzu_config_auto_checkpoint_eq, 1 );
	rb_define_method( rkuzu_cKuzuConfig, "checkpoint_threshold=", rkuzu_config_checkpoint_threshold_eq, 1 );

	rb_require( "kuzu/config" );
}
