/*
 *  connection.c - Kuzu::Connection class
 *
 */

#include "kuzu_ext.h"


VALUE rkuzu_cKuzuConnection;


static const rb_data_type_t rkuzu_connection_type = {
	.wrap_struct_name = "Kuzu::Connection",
	.function = {},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};


#define check_connection(self) ((kuzu_connection*)rb_check_typeddata((self), &rkuzu_connection_type))



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
	kuzu_connection *ptr = check_connection( self );

	if ( !ptr ) {
		rkuzu_database *dbobject = rkuzu_check_database( database );
		ptr = ALLOC( kuzu_connection );

		if ( kuzu_connection_init(&dbobject->db, ptr) == KuzuError ) {
			rb_raise( rkuzu_eConnectionError, "Failed to connect!" );
			xfree( ptr );
			ptr = NULL;
		}

		DATA_PTR( self ) = ptr;
		rb_ary_push( dbobject->connections, self );
	} else {
		rb_raise( rb_eRuntimeError, "cannot reinit connection" );
	}

	rb_call_super( 0, 0 );

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

	rb_define_method( rkuzu_cKuzuConnection, "initialize", rkuzu_connection_initialize, 1 );

}

