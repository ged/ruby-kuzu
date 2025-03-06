# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu query result class
class Kuzu::Result
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Return a Kuzu::QuerySummary for the query that generated the Result.
	def query_summary
		return Kuzu::QuerySummary.from_result( self )
	end


	

	### Return a string representation of the receiver suitable for debugging.
	def inspect
		details = " success: %p" % [
			self.success?,
		]

		default = super
		return default.sub( />/, details + '>' )
	end


	#########
	protected
	#########
	
	### Return an Enumerator that yields result tuples as Hashes. 
	def each_enum
		
	end

end # class Kuzu::Result
