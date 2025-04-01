/*
 *  kuzu_ext.h - Ruby binding for Kùzu
 *
 *  Authors:
 *    * Michael Granger <ged@FaerieMUD.org>
 *
 */

#ifndef KUZU_EXT_H_AA9CC4A5
#define KUZU_EXT_H_AA9CC4A5

#include "extconf.h"

#include <ruby.h>
#include <ruby/encoding.h>
#include <ruby/intern.h>
#include <ruby/thread.h>
#include <ruby/version.h>

#include <stdbool.h>

#include "kuzu.h"

/* --------------------------------------------------------------
 * Declarations
 * -------------------------------------------------------------- */

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a, b) va_start (a, b)
void rkuzu_log_obj (VALUE, const char *, const char *, ...);
void rkuzu_log (const char *, const char *, ...);
#else
#include <varargs.h>
#define va_init_list(a, b) va_start (a)
void rkuzu_log_obj (VALUE, const char *, const char *, va_dcl);
void rkuzu_log (const char *, const char *, va_dcl);
#endif

/* --------------------------------------------------------------
 * Structs
 * -------------------------------------------------------------- */

typedef struct {
    kuzu_database db;
    VALUE path;
    VALUE config;
} rkuzu_database;

typedef struct {
    kuzu_connection conn;
    VALUE database;
    VALUE queries;
    VALUE statements;
} rkuzu_connection;

typedef struct {
    kuzu_query_result result;
    VALUE connection;
    VALUE query;
    VALUE statement;
    VALUE previous_result;
    VALUE next_result;
    bool finished;
} rkuzu_query_result;

typedef struct {
    kuzu_prepared_statement statement;
    VALUE connection;
    VALUE query;
    bool finished;
} rkuzu_prepared_statement;


/* -------------------------------------------------------
 * Globals
 * ------------------------------------------------------- */

// Modules and classes
extern VALUE rkuzu_mKuzu;
extern VALUE rkuzu_cKuzuDatabase;
extern VALUE rkuzu_cKuzuConfig;
extern VALUE rkuzu_cKuzuConnection;
extern VALUE rkuzu_cKuzuPreparedStatement;
extern VALUE rkuzu_cKuzuResult;
extern VALUE rkuzu_cKuzuQuerySummary;

// Exception types
extern VALUE rkuzu_eError;
extern VALUE rkuzu_eDatabaseError;
extern VALUE rkuzu_eConnectionError;
extern VALUE rkuzu_eQueryError;
extern VALUE rkuzu_eFinishedError;

// Internal refs to external classes
extern VALUE rkuzu_rb_cDate;
extern VALUE rkuzu_rb_cOstruct;


/* -------------------------------------------------------
 * Initializer functions
 * ------------------------------------------------------- */
extern void Init_kuzu_ext _ ((void));

extern void rkuzu_init_database _ ((void));
extern void rkuzu_init_config _ ((void));
extern void rkuzu_init_connection _ ((void));
extern void rkuzu_init_prepared_statement _ ((void));
extern void rkuzu_init_result _ ((void));
extern void rkuzu_init_query_summary _ ((void));

extern rkuzu_database *rkuzu_get_database _ ((VALUE));
extern kuzu_system_config *rkuzu_get_config _ ((VALUE));
extern rkuzu_connection *rkuzu_get_connection _ ((VALUE));
extern rkuzu_prepared_statement *rkuzu_get_prepared_statement _ ((VALUE));
extern rkuzu_query_result *rkuzu_get_result _ ((VALUE));

extern VALUE rkuzu_convert_kuzu_value_to_ruby _ ((kuzu_data_type_id, kuzu_value *));
extern VALUE rkuzu_convert_logical_kuzu_value_to_ruby _ ((kuzu_logical_type *, kuzu_value *));

#endif /* end of include guard: KUZU_EXT_H_AA9CC4A5 */
