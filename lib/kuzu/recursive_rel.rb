# -*- ruby -*-

require 'forwardable'
require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kuzu recursive relationship class
class Kuzu::RecursiveRel
	extend Loggability,
		Forwardable

	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Create a new RecursiveRel with the given +nodes+ and +rels+.
	def initialize( nodes, rels )
		@nodes = nodes
		@rels = rels
	end


	######
	public
	######

	##
	# The Array of Kuzu::Nodes in the chain
	attr_reader :nodes

	##
	# The Array of Kuzu::Rels connecting the Nodes in the chain
	attr_reader :rels

end # class Kuzu::RecursiveRel
