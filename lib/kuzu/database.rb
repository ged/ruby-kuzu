# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Main Kùzu database class
class Kuzu::Database
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Return a connection to this database.
	def connect
		return Kuzu::Connection.new( self )
	end


	### Return +true+ if this database was created in read-only mode.
	def read_only?
		return self.config.read_only
	end


	### Returns +true+ if this database will automatically checkpoint when the size of
	### the WAL file exceeds the `checkpoint_threshold`.
	def auto_checkpointing?
		return self.config.auto_checkpoint
	end


	### Returns +true+ if this database uses compression for data on disk.
	def compression_enabled?
		return self.config.enable_compression
	end


	### Return a string representation of the receiver suitable for debugging.
	def inspect
		details = " path:%p read-only:%p" % [
			self.path,
			self.read_only?,
		]

		default = super
		return default.sub( />/, details + '>' )
	end

end # class Kuzu::Database
