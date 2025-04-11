# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu connection class
class Kuzu::Connection
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Execute the given +query_string+ via the connection and return the
	### Kuzu::Result. If a block is given, the result will instead be yielded to it,
	### finished when it returns, and the return value of the block will be returned
	### instead.
	def query( query_string, &block )
		result = self._query( query_string )
		return Kuzu::Result.wrap_block_result( result, &block )
	end


	### Create a new Kuzu::PreparedStatement for the specified +query_string+.
	def prepare( query_string )
		return Kuzu::PreparedStatement.new( self, query_string )
	end


	### Executes the given +statement+ (a Kuzu::PreparedStatement) after binding
	### the given +bound_variables+ to it.
	def execute( statement, **bound_variables, &block )
		statement.bind( **bound_variables )
		return statement.execute
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
