# -*- ruby -*-

require 'forwardable'
require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kuzu rel (relationship) class
class Kuzu::Rel
	extend Loggability,
		Forwardable

	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Create a new Rel with the given +id+, +label+, and +properties+.
	def initialize( src_id, dst_id, label, **properties )
		@src_id = src_id
		@dst_id = dst_id
		@label = label
		@properties = properties
	end


	######
	public
	######

	##
	# The internal id value of the source node
	attr_reader :src_id

	##
	# The internal id value of the destination node
	attr_reader :dst_id

	##
	# The label value of the given node
	attr_reader :label

	##
	# The Hash of the Rel's properties, keyed by name as a Symbol
	attr_reader :properties


	# Allow direct access to properties
	def_delegators :@properties, :[], :[]=, :dig

end # class Kuzu::Rel
