# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu system config options container class.
class Kuzu::Config
	extend Loggability


	# The detail fragments that make up the #inspect output, in the order
	# they should appear.
	INSPECT_PARTS = [
		"buffer_pool_size:%d",
		"max_num_threads:%d",
		"enable_compression:%s",
		"read_only:%s",
		"max_db_size:%d",
		"auto_checkpoint:%s",
		"checkpoint_threshold:%d"
	].freeze

	# The printf pattern used for #inspect output
	INSPECT_FORMAT = ' ' + INSPECT_PARTS.join( ' ' )


	# Loggability API -- log to the Kuzu logger
	log_to :kuzu


	### Return a default Config object with the specified +options+ overridden. If
	### +source_options+ is a Kuzu::Config, the returned object will be a clone of
	### it with the +options+ applied.
	def self::from_options( source_options=nil, **options )
		config = source_options.dup || new()

		config.set( **options )

		return config
	end


	### Set one or more +options+.
	def set( **options )
		options.each do |opt, val|
			self.public_send( "#{opt}=", val )
		end
	end


	### Return a human-readable representation of the object suitable for debugging.
	def inspect
		details = INSPECT_FORMAT % [
			self.buffer_pool_size,
			self.max_num_threads,
			self.enable_compression,
			self.read_only,
			self.max_db_size,
			self.auto_checkpoint,
			self.checkpoint_threshold,
		]

		default = super
		return default.sub( />/, details + '>' )
	end


end # class Kuzu::Config
