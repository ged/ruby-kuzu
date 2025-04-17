# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu'


RSpec.describe( "data types" ) do

	let( :db ) { Kuzu.database }
	let( :connection ) { db.connect }


	#
	# Specs
	#

	it "converts TIMESTAMP values to Time objects" do
		result = connection.query( %{RETURN timestamp("1970-01-01 00:00:00.004666-10") as x;} )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success

		value = result.first
		expect( value ).to include( 'x' )

		x = value['x']
		expect( x ).to be_a( Time )
		expect( x.year ).to eq( 1970 )
		expect( x.month ).to eq( 1 )
		expect( x.day ).to eq( 1 )
	end


	it "converts STRUCT values to OpenStructs" do
		result = connection.query( "RETURN {first: 'Adam', last: 'Smith'} AS record;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		Kuzu.logger.debug "Result is: %p" % [ result ]

		value = result.first
		expect( value ).to include( 'record' )

		record = value['record']
		expect( record ).to be_a( OpenStruct )
		expect( record.first ).to eq( "Adam" )
		expect( record.first.encoding ).to eq( Encoding::UTF_8 )
		expect( record.last ).to eq( "Smith" )
		expect( record.last.encoding ).to eq( Encoding::UTF_8 )

		result.finish
	end


	it "converts MAP values to Hashes" do
		result = connection.query( "RETURN map([1, 2], ['a', 'b']) AS m;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		Kuzu.logger.debug "Result is: %p" % [ result ]

		value = result.first
		expect( value ).to include( 'm' )

		record = value['m']
		expect( record ).to be_a( Hash )
		expect( record ).to eq({ 1 => 'a', 2 => 'b' })

		result.finish
	end


	it "converts LIST values to Arrays" do
		result = connection.query( 'RETURN ["Alice", "Bob"] AS l;' )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		Kuzu.logger.debug "Result is: %p" % [ result ]

		value = result.first
		expect( value ).to include( 'l' )

		record = value['l']
		expect( record ).to be_an( Array )
		expect( record ).to eq( ['Alice', 'Bob'] )

		result.finish
	end


	it "converts ARRAY values to Arrays" do
		result = connection.query( "RETURN CAST([3,4,12,11], 'INT64[4]') as a;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		Kuzu.logger.debug "Result is: %p" % [ result ]

		value = result.first
		expect( value ).to include( 'a' )

		record = value['a']
		expect( record ).to be_an( Array )
		expect( record ).to eq( [3, 4, 12, 11] )

		result.finish
	end


	it "converts ARRAYs of LIST values correctly" do
		result = connection.query( "RETURN CAST([[5,2,1],[2,3],[15,64,74]], 'INT64[][3]') as a;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		Kuzu.logger.debug "Result is: %p" % [ result ]

		value = result.first
		expect( value ).to include( 'a' )

		record = value['a']
		expect( record ).to be_an( Array )
		expect( record ).to eq( [ [5, 2, 1], [2, 3], [15, 64, 74] ] )

		result.finish
	end


	it "converts NODE values to Kuzu::Node objects" do
		connection.run( <<~END_OF_SCHEMA )
			CREATE NODE TABLE Person(id INT64, name STRING, age INT64, PRIMARY KEY(id));
			COPY Person FROM 'spec/data/test/Person.csv';
		END_OF_SCHEMA
		result = connection.query( "MATCH (a:Person) RETURN a;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		expect( result.num_tuples ).to eq( 120 )

		result.each do |value|
			expect( value ).to include( 'a' )

			node = value['a']
			expect( node ).to be_a( Kuzu::Node )

			expect( node.properties.keys ).to contain_exactly( :id, :name, :age )
		end

		result.finish
	end


	it "converts REL values to Kuzu::Rel objects" do
		connection.run( <<~END_OF_SCHEMA )
			CREATE NODE TABLE Person(id INT64, name STRING, age INT64, PRIMARY KEY(id));
			CREATE REL TABLE Follows (from Person to Person, since INT64);
			COPY Person FROM 'spec/data/test/Person.csv';
			COPY Follows FROM 'spec/data/test/Follows.csv';
		END_OF_SCHEMA
		result = connection.query( "MATCH (a:Person)-[r:Follows]->(b:Person) RETURN r;" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success
		expect( result.num_tuples ).to eq( 5 )

		result.each do |value|
			expect( value ).to include( 'r' )

			rel = value['r']
			expect( rel ).to be_a( Kuzu::Rel )

			expect( rel.src_id ).to_not be_nil
			expect( rel.dst_id ).to_not be_nil

			expect( rel.properties.keys ).to contain_exactly( :since )
		end

		result.finish
	end


	it "converts RECURSIVE_REL values to Kuzu::RecursiveRel objects" do
		connection.run( <<~END_OF_SCHEMA )
			CREATE NODE TABLE Person(id INT64, name STRING, age INT64, PRIMARY KEY(id));
			CREATE REL TABLE Follows (from Person to Person, since INT64);
			COPY Person FROM 'spec/data/test/Person.csv';
			COPY Follows FROM 'spec/data/test/Follows.csv';
		END_OF_SCHEMA
		result = connection.query( <<~END_OF_QUERY )
			MATCH p = (a:Person)-[:Follows]->(b:Person)
			WHERE a.name = 'Jake Kling' AND b.name = 'Joaquin Schamberger'
			RETURN p;
		END_OF_QUERY

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success

		result.each do |value|
			expect( value ).to include( 'p' )

			rel = value['p']
			expect( rel ).to be_a( Kuzu::RecursiveRel )

			expect( rel.nodes ).to be_an( Array ).and have_attributes( length: 2 )
			expect( rel.nodes[0] ).to be_a( Kuzu::Node )
			expect( rel.nodes[0][:name] ).to eq( 'Jake Kling' )
			expect( rel.nodes[1] ).to be_a( Kuzu::Node )
			expect( rel.nodes[1][:name] ).to eq( 'Joaquin Schamberger' )

			expect( rel.rels ).to be_an( Array ).and have_attributes( length: 1 )
			expect( rel.rels[0][:since] ).to eq( 2012 )
		end

		result.finish
	end


	it "converts between nil and NULL" do
		stmt = connection.prepare( "RETURN $the_value AS value;" )
		result = stmt.execute( the_value: nil )

		expect( result.first ).to eq( {'value' => nil} )
	end

end
