# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/result'


RSpec.describe( Kuzu::Result ) do

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


	#
	# Specs
	#

	it "can be created via a simple query" do
		result = described_class.from_query( connection, schema.each_line.first )

		expect( result ).to be_a( described_class )
		expect( result ).to be_success
	end


	it "can return a summary of its query's timing" do
		result = described_class.from_query( connection, schema.each_line.first )
		summary = result.query_summary

		expect( summary ).to be_a( Kuzu::QuerySummary )
	end


	it "can return the error message if there was a problem with the query" do
		expect {
			described_class.from_query( connection, "FOOBANGLE NERFRIDER" )
		}.to raise_error( Kuzu::QueryError, /parser exception/i )
	end


	it "knows how many columns are in each tuple" do
		setup_demo_db()

		result = described_class.from_query( connection, <<~END_OF_QUERY )
			MATCH ( a:User )-[ f:Follows ]->( b:User )
		    RETURN a.name, b.name, f.since;
		END_OF_QUERY

		expect( result ).to be_a( described_class )
		expect( result ).to be_success
		expect( result.num_columns ).to eq( 3 )
	end


	

end

