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


	it "can iterate over result tuples" do
		setup_demo_db()

		result = described_class.from_query( connection, <<~END_OF_QUERY )
			MATCH ( a:User )-[ f:Follows ]->( b:User )
		    RETURN a.name, b.name, f.since;
		END_OF_QUERY

		expect( result ).to be_a( described_class )
		expect( result ).to be_success
		expect( result.num_columns ).to eq( 3 )
		expect( result.column_names ).to eq([ "a.name", "b.name", "f.since" ])
		expect( result.each.to_a ).to eq([
			{ "a.name" => "Adam", "b.name" => "Karissa", "f.since" => 2020 },
			{ "a.name" => "Adam", "b.name" => "Zhang", "f.since" => 2020 },
			{ "a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021 },
			{ "a.name" => "Zhang", "b.name" => "Noura", "f.since" => 2022 }
		])
	end


	it "can iterate over result sets" do
		result = described_class.from_query( connection, <<~END_QUERY )
			return 1;
			return 2;
			return 3;
		END_QUERY

		rval = result.each_set.flat_map do |subset|
			subset.each.to_a
		end

		expect( rval ).to eq([
			{ "1" => 1 },
			{ "2" => 2 },
			{ "3" => 3 },
		])
	end

end
