/*
 *  recursive_rel.c - Kuzu::RecursiveRel class
 *
 */

#include "kuzu_ext.h"

VALUE rkuzu_cKuzuRecursiveRel;


/*
 * Document-class: Kuzu::RecursiveRel
 */
void
rkuzu_init_recursive_rel( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuRecursiveRel = rb_define_class_under( rkuzu_mKuzu, "RecursiveRel", rb_cObject );

	rb_require( "kuzu/recursive_rel" );
}
