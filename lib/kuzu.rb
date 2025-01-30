# -*- ruby -*-

require 'loggability'

require_relative 'kuzu_ext'

#--
# See also: ext/kuzu_ext/kuzu_ext.c
module Kuzu
	extend Loggability


	# Library version
	VERSION = '0.0.1'


	# Set up a logger for Kuzu classes
	log_as :kuzu


	require 'kuzu/database'
	require 'kuzu/config'


	### Create and return a Kuzu::Database. If +path+ is +nil+, an empty string, or
	### the Symbol :memory, creates an in-memory database. Valid options are:
	###
	### [:buffer_pool_size]
	###   Max size of the buffer pool in bytes.
    ### [:max_num_threads]
    ###   The maximum number of threads to use during query execution.
    ### [:enable_compression]
    ###   Whether or not to compress data on-disk for supported types
    ### [:read_only]
    ###   If true, open the database in read-only mode. No write transaction is allowed on the 
	###   Database object. If false, open the database read-write.
    ### [:max_db_size]
	###		The maximum size of the database in bytes.
    ### [:auto_checkpoint]
    ###   If true, the database will automatically checkpoint when the size of
    ###   the WAL file exceeds the checkpoint threshold.
    ### [:checkpoint_threshold]
    ###   The threshold of the WAL file size in bytes. When the size of the
    ###   WAL file exceeds this threshold, the database will checkpoint if 
	###   `auto_checkpoint` is true.
	def self::database( path='', **config )
		path = '' if path.nil? || path == :memory
		return Kuzu::Database.new( path.to_s, **config )
	end

end # module Kuzu

RKuzu = Kuzu # Convenience alias

