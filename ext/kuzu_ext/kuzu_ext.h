/*
 *  kuzu_ext.h - Ruby binding for Kùzu
 *  $Id$
 *
 *  Authors:
 *    * Michael Granger <ged@FaerieMUD.org>
 *
 */

#ifndef KUZU_EXT_H_AA9CC4A5
#define KUZU_EXT_H_AA9CC4A5

#include "extconf.h"

#include <ruby.h>
#include <ruby/intern.h>
#include <ruby/thread.h>
#include <ruby/encoding.h>
#include <ruby/version.h>

#include "kuzu.h"


/* --------------------------------------------------------------
 * Declarations
 * -------------------------------------------------------------- */

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
void rkuzu_log_obj( VALUE, const char *, const char *, ... );
void rkuzu_log( const char *, const char *, ... );
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
void rkuzu_log_obj( VALUE, const char *, const char *, va_dcl );
void rkuzu_log( const char *, const char *, va_dcl );
#endif


/* --------------------------------------------------------------
 * Structs
 * -------------------------------------------------------------- */



/* -------------------------------------------------------
 * Globals
 * ------------------------------------------------------- */

/*
 * Modules
 */
extern VALUE rkuzu_mKuzu;
extern VALUE rkuzu_cKuzuDatabase;
extern VALUE rkuzu_cKuzuConfig;
extern VALUE rkuzu_cKuzuConnection;
extern VALUE rkuzu_cKuzuStatement;
extern VALUE rkuzu_cKuzuResult;

extern VALUE rkuzu_eError;
extern VALUE rkuzu_eConnectionError;


/* -------------------------------------------------------
 * Initializer functions
 * ------------------------------------------------------- */
extern void Init_kuzu_ext _(( void ));

extern void rkuzu_init_database _(( void ));
extern void rkuzu_init_config _(( void ));
extern void rkuzu_init_connection _(( void ));
extern void rkuzu_init_statement _(( void ));
extern void rkuzu_init_result _(( void ));

extern kuzu_system_config *rkuzu_check_config _(( VALUE ));
extern kuzu_database *rkuzu_check_database _(( VALUE ));

#endif /* end of include guard: KUZU_EXT_H_AA9CC4A5 */


