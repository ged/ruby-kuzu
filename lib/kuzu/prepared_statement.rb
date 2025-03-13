# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# A parameterized query which can avoid planning the same query for repeated execution
class Kuzu::PreparedStatement
	extend Loggability

	# Loggability API -- Use Kuzu's logger
	log_to :kuzu


	### Execute the statement against its connection and return a Kuzu::Result.
	def execute( **bound_variables )
		return self.connection.execute( self, **bound_variables )
	end


	### Bind the variables in the specified +variable_map+ to the statement.
	def	bind( **variable_map )
		variable_map.each do |name, value|
			self.bind_variable( name, value )
		end
	end

end # class Kuzu::PreparedStatement
