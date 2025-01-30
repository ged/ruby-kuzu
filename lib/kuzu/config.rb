# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


class Kuzu::Config
	extend Loggability


	INSPECT_PARTS = [
		"buffer_pool_size:%d",
		"max_num_threads:%d",
		"enable_compression:%s",
		"read_only:%s",
		"max_db_size:%d",
		"auto_checkpoint:%s",
		"checkpoint_threshold:%d"
	].freeze

	INSPECT_FORMAT = ' ' + INSPECT_PARTS.join( ' ' )


	# Loggability API -- log to the Kuzu logger
	log_to :kuzu


	### Return a default Config object with the specified +options+ overridden.
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


	### Return a human-readable represetnation of the object suitable for debugging.
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
