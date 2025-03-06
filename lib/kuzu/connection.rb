# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu connection class
class Kuzu::Connection
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Executes the given +statement+ and returns the result. 
	def query( statement )
		return Kuzu::Result.from_query( self, statement )
	end


	### Return a string representation of the receiver suitable for debugging.
	def inspect
		details = " threads:%d" % [
			self.max_num_threads_for_exec,
		]

		default = super
		return default.sub( />/, details + '>' )
	end

end # class Kuzu::Connection
