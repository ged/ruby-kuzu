/*
 *  node.c - Kuzu::Node class
 *
 */

#include "kuzu_ext.h"

VALUE rkuzu_cKuzuNode;


/*
 * Document-class: Kuzu::Node
 */
void
rkuzu_init_node( void )
{
#ifdef FOR_RDOC
	rkuzu_mKuzu = rb_define_module( "Kuzu" );
#endif

	rkuzu_cKuzuNode = rb_define_class_under( rkuzu_mKuzu, "Node", rb_cObject );

	rb_require( "kuzu/node" );
}
