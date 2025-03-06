/*
 *  types.c - Data type conversion functions
 *
 */

#include "kuzu_ext.h"


#define CONVERT_CHECK( TYPE, CONVERSION ) \
	do { if ( (CONVERSION) != KuzuSuccess ) {\
		rb_raise( rb_eTypeError, "couldn't convert " #TYPE ); }\
	} while (0)


static VALUE
rkuzu_convert_bool( kuzu_value *value )
{
	bool typed_value;

	CONVERT_CHECK( KUZU_BOOL, kuzu_value_get_bool(value, &typed_value) );

	return typed_value ? Qtrue : Qfalse;
}


static VALUE
rkuzu_convert_int64( kuzu_value *value )
{
	int64_t typed_value;

	CONVERT_CHECK( KUZU_INT64, kuzu_value_get_int64(value, &typed_value) );

	return LL2NUM( typed_value );
}


static VALUE
rkuzu_convert_int32( kuzu_value *value )
{
	int32_t typed_value;

	CONVERT_CHECK( KUZU_INT32, kuzu_value_get_int32(value, &typed_value) );

	return INT2FIX( typed_value );
}


static VALUE
rkuzu_convert_int16( kuzu_value *value )
{
	int16_t typed_value;

	CONVERT_CHECK( KUZU_INT16, kuzu_value_get_int16(value, &typed_value) );

	return INT2FIX( typed_value );
}


static VALUE
rkuzu_convert_int8( kuzu_value *value )
{
	int8_t typed_value;

	CONVERT_CHECK( KUZU_INT8, kuzu_value_get_int8(value, &typed_value) );

	return INT2FIX( typed_value );
}


static VALUE
rkuzu_convert_uint64( kuzu_value *value )
{
	uint64_t typed_value;

	CONVERT_CHECK( KUZU_UINT64, kuzu_value_get_uint64(value, &typed_value) );

	return ULL2NUM( typed_value );
}


static VALUE
rkuzu_convert_uint32( kuzu_value *value )
{
	uint32_t typed_value;

	CONVERT_CHECK( KUZU_UINT32, kuzu_value_get_uint32(value, &typed_value) );

	return UINT2NUM( typed_value );
}


static VALUE
rkuzu_convert_uint16( kuzu_value *value )
{
	uint16_t typed_value;

	CONVERT_CHECK( KUZU_UINT16, kuzu_value_get_uint16(value, &typed_value) );

	return UINT2NUM( typed_value );
}


static VALUE
rkuzu_convert_uint8( kuzu_value *value )
{
	uint8_t typed_value;

	CONVERT_CHECK( KUZU_UINT8, kuzu_value_get_uint8(value, &typed_value) );

	return UINT2NUM( typed_value );
}


static VALUE
rkuzu_convert_int128( kuzu_value *value )
{
	kuzu_int128_t typed_value;
	char *int_string;
	VALUE int_rb_string;

	CONVERT_CHECK( KUZU_INT128, kuzu_value_get_int128(value, &typed_value) );

	kuzu_int128_t_to_string( typed_value, &int_string );
	int_rb_string = rb_str_new2( int_string );
	kuzu_destroy_string( int_string );

	return rb_funcall( rb_mKernel, rb_intern("Integer"), 1, int_rb_string );
}


static VALUE
rkuzu_convert_double( kuzu_value *value )
{
	double typed_value;

	CONVERT_CHECK( KUZU_DOUBLE, kuzu_value_get_double(value, &typed_value) );

	return rb_float_new( typed_value );
}


static VALUE
rkuzu_convert_float( kuzu_value *value )
{
	float typed_value;

	CONVERT_CHECK( KUZU_FLOAT, kuzu_value_get_float(value, &typed_value) );

	return rb_float_new( typed_value );
}


static VALUE
rkuzu_convert_date( kuzu_value *value )
{
	kuzu_date_t typed_value;
	struct tm time;
	VALUE argv[3];

	CONVERT_CHECK( KUZU_DATE, kuzu_value_get_date(value, &typed_value) );

	kuzu_date_to_tm( typed_value, &time );

	argv[0] = INT2FIX( time.tm_year );
	argv[1] = INT2FIX( time.tm_mon );
	argv[2] = INT2FIX( time.tm_mday );

	return rb_class_new_instance( 3, argv, rkuzu_rb_cDate );
}


static VALUE
rkuzu_convert_timestamp( kuzu_value *value )
{
	kuzu_timestamp_t typed_value;

	CONVERT_CHECK( KUZU_TIMESTAMP, kuzu_value_get_timestamp(value, &typed_value) );

	return rb_funcall( rkuzu_mKuzu, rb_intern("timestamp_from_timestamp_us"),
		1, LL2NUM(typed_value.value) );
}


static VALUE
rkuzu_convert_timestamp_sec( kuzu_value *value )
{
	kuzu_timestamp_sec_t typed_value;

	CONVERT_CHECK( KUZU_TIMESTAMP_SEC, kuzu_value_get_timestamp_sec(value, &typed_value) );

	return rb_funcall( rb_cTime, rb_intern("at"), 1, INT2FIX(typed_value.value) );
}


static VALUE
rkuzu_convert_timestamp_ms( kuzu_value *value )
{
	kuzu_timestamp_ms_t typed_value;

	CONVERT_CHECK( KUZU_TIMESTAMP_MS, kuzu_value_get_timestamp_ms(value, &typed_value) );
	return rb_funcall( rkuzu_mKuzu, rb_intern("timestamp_from_timestamp_ms"),
		1, LL2NUM(typed_value.value) );
}


static VALUE
rkuzu_convert_timestamp_ns( kuzu_value *value )
{
	kuzu_timestamp_ns_t typed_value;

	CONVERT_CHECK( KUZU_TIMESTAMP_NS, kuzu_value_get_timestamp_ns(value, &typed_value) );

	return rb_funcall( rkuzu_mKuzu, rb_intern("timestamp_from_timestamp_ns"),
		1, LL2NUM(typed_value.value) );
}


static VALUE
rkuzu_convert_timestamp_tz( kuzu_value *value )
{
	kuzu_timestamp_tz_t typed_value;

	CONVERT_CHECK( KUZU_TIMESTAMP_TZ, kuzu_value_get_timestamp_tz(value, &typed_value) );

	return rb_funcall( rkuzu_mKuzu, rb_intern("timestamp_from_timestamp_ms"),
		2, LL2NUM(typed_value.value), INT2FIX(0) );
}


static VALUE
rkuzu_convert_decimal( kuzu_value *value )
{
	char *decimal_as_string;
	VALUE decimal_ruby_string;

	CONVERT_CHECK( KUZU_DECIMAL, kuzu_value_get_decimal_as_string(value, &decimal_as_string) );

	decimal_ruby_string = rb_str_new2( decimal_as_string );
	kuzu_destroy_string( decimal_as_string );

	return rb_funcall( rb_mKernel, rb_intern("Float"), 1, decimal_ruby_string );
}


static VALUE
rkuzu_convert_string( kuzu_value *value )
{
	char *result_string;
	VALUE rval;

	CONVERT_CHECK( KUZU_STRING, kuzu_value_get_string(value, &result_string) );

	rval = rb_str_new2( result_string );
	kuzu_destroy_string( result_string );

	return rval;
}


VALUE
rkuzu_convert_type( kuzu_data_type_id type, kuzu_value *value )
{
	switch( type ) {
		case KUZU_BOOL: return rkuzu_convert_bool( value );
		case KUZU_INT64: return rkuzu_convert_int64( value );
		case KUZU_INT32: return rkuzu_convert_int32( value );
		case KUZU_INT16: return rkuzu_convert_int16( value );
		case KUZU_INT8: return rkuzu_convert_int8( value );
		case KUZU_UINT64: return rkuzu_convert_uint64( value );
		case KUZU_UINT32: return rkuzu_convert_uint32( value );
		case KUZU_UINT16: return rkuzu_convert_uint16( value );
		case KUZU_UINT8: return rkuzu_convert_uint8( value );
		case KUZU_INT128: return rkuzu_convert_int128( value );
		case KUZU_DOUBLE: return rkuzu_convert_double( value );
		case KUZU_FLOAT: return rkuzu_convert_float( value );
		case KUZU_DATE: return rkuzu_convert_date( value );
		case KUZU_TIMESTAMP: return rkuzu_convert_timestamp( value );
		case KUZU_TIMESTAMP_SEC: return rkuzu_convert_timestamp_sec( value );
		case KUZU_TIMESTAMP_MS: return rkuzu_convert_timestamp_ms( value );
		case KUZU_TIMESTAMP_NS: return rkuzu_convert_timestamp_ns( value );
		case KUZU_TIMESTAMP_TZ: return rkuzu_convert_timestamp_tz( value );
		case KUZU_DECIMAL: return rkuzu_convert_decimal( value );

		case KUZU_STRING: return rkuzu_convert_string( value );

		case KUZU_INTERVAL:
		case KUZU_BLOB:
		case KUZU_LIST:
		case KUZU_ARRAY:
		case KUZU_STRUCT:
		case KUZU_MAP:
		case KUZU_UNION:
		case KUZU_POINTER:
		case KUZU_UUID:

		case KUZU_INTERNAL_ID:

		// Fallthrough
		default:
			rb_raise( rb_eTypeError, "Unhandled Kuzu data type: %d", type );
	}
}
