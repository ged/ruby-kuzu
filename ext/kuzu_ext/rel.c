/*
 *  rel.c - Kuzu::Rel (relationship) class
 *
 */

#include "kuzu_ext.h"

VALUE rkuzu_cKuzuRel;


/*
 * Document-class: Kuzu::Rel
 */
void
rkuzu_init_rel( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuRel = rb_define_class_under( rkuzu_mKuzu, "Rel", rb_cObject );

	rb_require( "kuzu/rel" );
}
