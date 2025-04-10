/*
 *  types.c - Data type conversion functions
 *
 */

#include "kuzu_ext.h"
#include "ruby/internal/intern/hash.h"


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


static VALUE
rkuzu_convert_interval( kuzu_value *value )
{
	kuzu_interval_t interval;
	double interval_seconds = 0.0;
	VALUE rval;

	CONVERT_CHECK( KUZU_INTERVAL, kuzu_value_get_interval(value, &interval) );
	assert( kuzu_interval_to_difftime(interval, &interval_seconds) == KuzuSuccess );

	rval = rb_float_new( interval_seconds );

	return rval;
}


static VALUE
rkuzu_convert_blob( kuzu_value *value )
{
	uint8_t *raw_blob;
	VALUE rval;

	CONVERT_CHECK( KUZU_BLOB, kuzu_value_get_blob(value, &raw_blob) );

	rval = rb_enc_str_new( (char *)raw_blob, strnlen((char *)raw_blob, CHAR_MAX), rb_ascii8bit_encoding() );

	return rval;
}


static VALUE
rkuzu_convert_list( kuzu_value *value )
{
	uint64_t count = 0;
	kuzu_value item_value;
	kuzu_logical_type item_type;
	VALUE item;
	VALUE rval = rb_ary_new();

	assert( kuzu_value_get_list_size(value, &count) == KuzuSuccess );

	for( uint64_t i =  0 ; i < count ; i++ ) {
		kuzu_value_get_list_element( value, i, &item_value );
		kuzu_value_get_data_type( &item_value, &item_type );
		item = rkuzu_convert_logical_kuzu_value_to_ruby( &item_type, &item_value );

		rb_ary_push( rval, item );
	}

	return rval;
}


static VALUE
rkuzu_convert_struct( kuzu_value *value )
{
	uint64_t count = 0;
	char *item_name;
	kuzu_value item_value;
	kuzu_logical_type item_type;
	VALUE item, item_sym;
	VALUE rval = rb_class_new_instance( 0, 0, rkuzu_rb_cOstruct );

	assert( kuzu_value_get_struct_num_fields(value, &count) == KuzuSuccess );

	for( uint64_t i =  0 ; i < count ; i++ ) {
		kuzu_value_get_struct_field_name( value, i, &item_name );
		kuzu_value_get_struct_field_value( value, i, &item_value );
		kuzu_value_get_data_type( &item_value, &item_type );
		item = rkuzu_convert_logical_kuzu_value_to_ruby( &item_type, &item_value );
		item_sym = rb_check_symbol_cstr( item_name, strnlen(item_name, CHAR_MAX),
			rb_usascii_encoding() );

		rb_funcall( rval, rb_intern("[]="), 2, item_sym, item );
	}

	return rval;
}


static VALUE
rkuzu_convert_map( kuzu_value *map_value )
{
	uint64_t count = 0;
	kuzu_value key, value;
	kuzu_logical_type key_type, value_type;
	VALUE key_obj, value_obj;
	VALUE rval = rb_hash_new();

	assert( kuzu_value_get_map_num_fields(value, &count) == KuzuSuccess );

	for( uint64_t i =  0 ; i < count ; i++ ) {
		kuzu_value_get_map_key( map_value, i, &key );
		kuzu_value_get_data_type( &key, &key_type );

		key_obj = rkuzu_convert_logical_kuzu_value_to_ruby( &key_type, &key );

		kuzu_value_get_map_value( map_value, i, &value );
		kuzu_value_get_data_type( &value, &value_type );

		value_obj = rkuzu_convert_logical_kuzu_value_to_ruby( &value_type, &value );

		rb_hash_aset( rval, key_obj, value_obj );
	}

	return rval;
}


VALUE
rkuzu_convert_kuzu_value_to_ruby( kuzu_data_type_id type_id, kuzu_value *value )
{
	if ( kuzu_value_is_null(value) ) {
		return Qnil;
	}

	switch( type_id ) {
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

		case KUZU_INTERVAL: return rkuzu_convert_interval( value );
		case KUZU_BLOB: return rkuzu_convert_blob( value );
		case KUZU_LIST: return rkuzu_convert_list( value );
		case KUZU_STRUCT: return rkuzu_convert_struct( value );

		case KUZU_MAP: return rkuzu_convert_map( value );
		case KUZU_UNION:
		case KUZU_POINTER:
		case KUZU_UUID:

		case KUZU_ARRAY:
		case KUZU_INTERNAL_ID:

		// Fallthrough
		default:
			rb_raise( rb_eTypeError, "Unhandled Kuzu data type: %d", type_id );
	}
}


VALUE
rkuzu_convert_logical_kuzu_value_to_ruby( kuzu_logical_type *data_type, kuzu_value *value )
{
	kuzu_data_type_id type_id = kuzu_data_type_get_id( data_type );
	return rkuzu_convert_kuzu_value_to_ruby( type_id, value );
}
