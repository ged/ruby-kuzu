# -*- ruby -*-

require 'loggability'

require 'kuzu' unless defined?( Kuzu )


# Kùzu query result class
#
# These objects contain one result set from either a Kuzu::Connection#query call
# or Kuzu::PreparedStatement#execute. If there are multiple result sets, you can
# fetch the next one by calling Kuzu::Result#next_set. You can use #has_next_set?
# to test for a following set.
#
# Tuple values are converted to corresponding Ruby objects:
#
# | Kuzu Type       | Ruby Type                                       |
# | --------------- | ----------------------------------------------- |
# | +INT8+          | +Integer+                                       |
# | +INT16+         | +Integer+                                       |
# | +INT32+         | +Integer+                                       |
# | +INT64+         | +Integer+                                       |
# | +INT128+        | +Integer+                                       |
# | +UINT8+         | +Integer+                                       |
# | +UINT16+        | +Integer+                                       |
# | +UINT32+        | +Integer+                                       |
# | +UINT64+        | +Integer+                                       |
# | +FLOAT+         | +Float+                                         |
# | +DOUBLE+        | +Float+                                         |
# | +DECIMAL+       | +Float+                                         |
# | +BOOLEAN+       | +TrueClass+ or +FalseClass+                     |
# | +UUID+          | +String+ (UTF-8 encoding)                       |
# | +STRING+        | +String+ (UTF-8 encoding)                       |
# | +NULL+          | +NilClass+                                      |
# | +DATE+          | +Date+                                          |
# | +TIMESTAMP+     | +Time+                                          |
# | +INTERVAL+      | +Float+ (interval in seconds)                   |
# | +STRUCT+        | +OpenStruct+ via the +ostruct+ standard library |
# | +MAP+           | +Hash+                                          |
# | +UNION+         | (not yet handled)                               |
# | +BLOB+          | +String+ (+ASCII_8BIT+ encoding)                |
# | +SERIAL+        | +Integer+                                       |
# | +NODE+          | Kuzu::Node                                      |
# | +REL+           | Kuzu::Rel                                       |
# | +RECURSIVE_REL+ | Kuzu::RecursiveRel                              |
# | +LIST+          | +Array+                                         |
# | +ARRAY+         | +Array+                                         |
#
#
class Kuzu::Result
	extend Loggability


	# Loggability API -- log to Kuzu's logger
	log_to :kuzu


	### Execute the given +query+ via the specified +connection+ and return the
	### Kuzu::Result. If a block is given, the result will instead be yielded to it,
	### finished when it returns, and the return value of the block will be returned
	### instead.
	def self::from_query( connection, query, &block )
		return connection.query( query, &block )
	end


	### Execute the given +statement+ and return the Kuzu::Result. If a block is given,
	### the result will instead be yielded to it, finished when it returns, and the
	### return value of the block will be returned instead.
	def self::from_prepared_statement( statement, &block )
		return statement.execute( &block )
	end


	### If the +block+ is provided, yield +result+ to it and then call #finish on it,
	### returning the +block+ result. If +block+ is not given, just return +result+.
	def self::wrap_block_result( result, &block )
		return result unless block

		begin
			rval = block.call( result )
		ensure
			result.finish
		end

		return rval
	end


	### Fetch the names of the columns in the result as an Array of Strings.
	def	column_names
		return @column_names ||= self.get_column_names
	end


	### Return a Kuzu::QuerySummary for the query that generated the Result.
	def query_summary
		return Kuzu::QuerySummary.from_result( self )
	end


	### Get the next tuple of the result as a Hash.
	def next
		values = self.get_next_values or return nil
		pairs = self.column_names.zip( values )
		return Hash[ pairs ]
	end


	### Iterate over each tuple of the result, yielding it to the +block+. If no
	### +block+ is given, return an Enumerator that will yield them instead.
	def	each( &block )
		enum = self.tuple_enum
		return enum.each( &block ) if block
		return enum
	end


	### Return the tuples from the current result set. This method is memoized
	### for efficiency.
	def tuples
		return @_tuples ||= self.to_a
	end


	### Index operator: fetch the tuple at +index+ of the current result set.
	def []( index )
		return self.tuples[ index ]
	end


	### Return the next result set after this one as a Kuzu::Result, or `nil`if
	### there is no next set.
	def next_set
		return nil unless self.has_next_set?
		return self.class.from_next_set( self )
	end


	### Iterate over each result set in the results, yielding it to the block. If
	### no +block+ is given, return an Enumerator tht will yield each set as its own
	### Result.
	def each_set( &block )
		enum = self.next_set_enum
		return enum.each( &block ) if block
		return enum
	end


	### Return a string representation of the receiver suitable for debugging.
	def inspect
		if self.finished?
			details = " (finished)"
		else
			details = " success: %p (%d tuples of %d columns)" % [
				self.success?,
				self.num_tuples,
				self.num_columns,
			]
		end

		default = super
		return default.sub( />/, details + '>' )
	end


	#########
	protected
	#########

	### Return an Enumerator that yields result tuples as Hashes.
	def tuple_enum
		self.log.debug "Fetching a tuple Enumerator"
		return Enumerator.new do |yielder|
			self.reset_iterator
			while self.has_next?
				tuple = self.next
				yielder.yield( tuple )
			end
		end
	end


	### Return an Enumerator that yields a Result for each set.
	def next_set_enum
		self.log.debug "Fetching a result set Enumerator"
		result = self
		return Enumerator.new do |yielder|
			while result
				yielder.yield( result )
				result = result.next_set
			end
		end
	end

end # class Kuzu::Result
