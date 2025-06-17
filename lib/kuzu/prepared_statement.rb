# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# A parameterized query which can avoid planning the same query for repeated execution
class Kuzu::PreparedStatement
	extend Loggability

	# Loggability API -- Use Kuzu's logger
	log_to :kuzu


	### Execute the statement against its connection and return a Kuzu::Result.
	### If a +block+ is supplied, the result will be passed to it instead,
	### then finished automatically, and the return value of the block returned
	### instead.
	def execute( **bound_variables, &block )
		self.log.debug "Executing statement:\n%s\nwith variables:\n%p" %
			[ self.query, bound_variables ]
		self.bind( **bound_variables )
		result = self._execute
		return Kuzu::Result.wrap_block_result( result, &block )
	end


	### Execute the statement against its connection and return `true` if it
	### succeeded.
	def execute!( **bound_variables )
		self.bind( **bound_variables )
		return self._execute!
	end


	### Bind the variables in the specified +variable_map+ to the statement.
	def	bind( **variable_map )
		variable_map.each do |name, value|
			self.bind_variable( name, value )
		end
	end

end # class Kuzu::PreparedStatement
