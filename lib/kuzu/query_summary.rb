# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu query summary class
class Kuzu::QuerySummary
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Return a string representation of the receiver suitable for debugging.
	def inspect
		details = " compiling: %0.3fs  execution: %0.3fs" % [
			self.compiling_time,
			self.execution_time,
		]

		default = super
		return default.sub( />/, details + '>' )
	end

end # class Kuzu::QuerySummary
