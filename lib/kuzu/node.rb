# -*- ruby -*-

require 'forwardable'
require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kuzu node class
class Kuzu::Node
	extend Loggability,
		Forwardable

	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Create a new Node with the given +id+, +label+, and +properties+.
	def initialize( id, label, **properties )
		@id = id
		@label = label
		@properties = properties
	end


	######
	public
	######

	##
	# The internal id value of the given node
	attr_reader :id

	##
	# The label value of the given node
	attr_reader :label

	##
	# The Hash of the Node's properties, keyed by name as a Symbol
	attr_reader :properties


	# Allow direct access to properties
	def_delegators :@properties, :[], :[]=, :dig

end # class Kuzu::Node
