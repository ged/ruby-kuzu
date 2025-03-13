# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu connection class
class Kuzu::Connection
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Executes the given +query_string+ and returns the result.
	def query( query_string )
		return Kuzu::Result.from_query( self, query_string )
	end


	### Executes the given +statement+ (a Kuzu::PreparedStatement) after binding
	### the given +bound_variables+ to it.
	def execute( statement, **bound_variables )
		statement.bind( **bound_variables )
		return Kuzu::Result.from_prepared_statement( statement )
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
