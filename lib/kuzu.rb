# -*- ruby -*-

require 'pathname'
require 'loggability'

require_relative 'kuzu_ext'

#--
# See also: ext/kuzu_ext/kuzu_ext.c
module Kuzu
	extend Loggability


	# Library version
	VERSION = '0.1.0'

	# Name of the file to look for when testing a path to see if it's a Kuzu database.
	KUZU_CATALOG_FILENAME = 'data.kz'


	# Set up a logger for Kuzu classes
	log_as :kuzu


	### Create and return a Kuzu::Database. If +path+ is +nil+, an empty string, or
	### the Symbol :memory, creates an in-memory database. Valid options are:
	###
	### `:buffer_pool_size`
	### :    Max size of the buffer pool in bytes.
	###
	### `:max_num_threads`
	### :    The maximum number of threads to use during query execution.
	###
	### `:enable_compression`
	### :    Whether or not to compress data on-disk for supported types
	###
	### `:read_only`
	### :    If true, open the database in read-only mode. No write transaction is allowed on the
	### Database object. If false, open the database read-write.
	###
	### `:max_db_size`
	###	:    The maximum size of the database in bytes.
	###
	### `:auto_checkpoint`
	### :    If true, the database will automatically checkpoint when the size of
	### the WAL file exceeds the checkpoint threshold.
	###
	### `:checkpoint_threshold`
	### :    The threshold of the WAL file size in bytes. When the size of the
	### WAL file exceeds this threshold, the database will checkpoint if
	### `auto_checkpoint` is true.
	def self::database( path='', **config )
		path = '' if path.nil? || path == :memory
		self.log.info "Opening database %p" % [ path ]
		return Kuzu::Database.new( path.to_s, **config )
	end


	### Returns +true+ if the specified +pathname+ appears to be a valid Kuzu database.
	def self::is_database?( pathname )
		pathname = Pathname( pathname )
		return false unless pathname.directory?

		testfile = pathname / KUZU_CATALOG_FILENAME
		return testfile.exist?
	end
	singleton_class.alias_method( :is_kuzu_database?, :is_database? )


	### Return a Time object from the given +milliseconds+ epoch time.
	def self::timestamp_from_timestamp_ms( milliseconds )
		seconds, subsec = milliseconds.divmod( 1_000 )
		return Time.at( seconds, subsec, :millisecond )
	end


	### Return a Time object from the given +microseconds+ epoch time and
	### optional timezone offset in seconds via the +zone+ argument.
	def self::timestamp_from_timestamp_us( microseconds, zone=nil )
		seconds, subsec = microseconds.divmod( 1_000_000 )
		return Time.at( seconds, subsec, :microsecond, in: zone )
	end


	### Return a Time object from the given +nanoseconds+ epoch time.
	def self::timestamp_from_timestamp_ns( nanoseconds )
		seconds, subsec = nanoseconds.divmod( 1_000_000_000 )
		return Time.at( seconds, subsec, :nanosecond )
	end

end # module Kuzu

