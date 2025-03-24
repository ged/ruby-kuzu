# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/prepared_statement'


RSpec.describe( Kuzu::PreparedStatement ) do

	let( :db ) { Kuzu.database }
	let( :connection ) { db.connect }

	let( :schema ) { load_test_data( 'demo-db/schema.cypher' ) }
	let( :copy_statements ) { load_test_data( 'demo-db/copy.cypher' ) }


	def setup_demo_db
		result = connection.query( schema )
		raise "query error while loading schema: %s" % [ result.error_message ] unless
			result.success?

		result = connection.query( copy_statements )
		raise "query error while loading data: %s" % [ result.error_message ] unless
			result.success?
	end


	before( :each ) do
		setup_demo_db()
	end


	#
	# Specs
	#

	it "can be created via a connection and a query string" do
		statement = described_class.new( connection, <<~END_OF_QUERY )
			MATCH (u:User)
		    WHERE u.age > $min_age and u.age < $max_age
		    RETURN u.name
		END_OF_QUERY

		expect( statement ).to be_a( described_class )
		expect( statement ).to be_success
		expect( statement.connection ).to be( connection )
	end


	it "doesn't error if executed before its variables are bound" do
		statement = described_class.new( connection, <<~END_OF_QUERY )
			MATCH (u:User)
		    WHERE u.age > $min_age and u.age < $max_age
		    RETURN u.name
		END_OF_QUERY

		result = statement.execute

		expect( result.num_tuples ).to eq( 0 )
	end


	it "can be executed if all of its variables are bound" do
		statement = described_class.new( connection, <<~END_OF_QUERY )
		MATCH (u:User)
		WHERE u.age >= $min_age and u.age <= $max_age
		RETURN u.name
		END_OF_QUERY

		result = statement.execute( min_age: 40, max_age: 50 )

		expect( result.num_tuples ).to eq( 2 )
	end


	it "can be reused" do
		statement = described_class.new( connection, <<~END_OF_QUERY )
		MATCH (u:User)
		WHERE u.age >= $min_age and u.age <= $max_age
		RETURN u.name
		END_OF_QUERY

		result = statement.execute( min_age: 40, max_age: 50 )
		expect( result ).to be_success
		expect( result.num_tuples ).to eq( 2 )

		result = statement.execute( min_age: 5, max_age: 90 )
		expect( result ).to be_success
		expect( result.num_tuples ).to eq( 4 )
	end

end
