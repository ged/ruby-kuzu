#!/usr/bin/env ruby

require 'rbconfig'
require 'mkmf'

dir_config( 'libkuzu' )

have_library( 'kuzu' ) or
	abort "No kuzu library!"

have_header( 'kuzu.h' ) or
	abort "No kuzu.h header!"
have_header( 'ruby/thread.h' ) or
	abort "Your Ruby is too old!"

have_func( 'kuzu_database_init', 'kuzu.h' )

create_header()
create_makefile( 'kuzu_ext' )

