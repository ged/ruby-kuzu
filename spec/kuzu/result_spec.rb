# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/result'


RSpec.describe( Kuzu::Result ) do

	let( :db ) { Kuzu.database }
	let( :connection ) { db.connect }

	let( :schema ) { load_test_data( 'demo-db/schema.cypher' ) }
	let( :copy_statements ) { load_test_data( 'demo-db/copy.cypher' ) }


	def setup_demo_db
		connection.run( schema )
		connection.run( copy_statements )
	end


	#
	# Specs
	#

	describe "query constructor" do

		it "can be created via a simple query" do
			result = described_class.from_query( connection, schema.each_line.first )

			expect( result ).to be_a( described_class )
			expect( result ).to be_success

			result.finish
		end


		it "automatically finishes the result when run with a block" do
			block_result = nil

			described_class.from_query( connection, schema.each_line.first ) do |result|
				block_result = result
				expect( result ).to be_a( described_class )
				expect( result ).to be_success
			end

			expect( block_result ).to be_finished
		end


		it "can return a summary of its query's timing" do
			result = described_class.from_query( connection, schema.each_line.first )
			summary = result.query_summary

			expect( summary ).to be_a( Kuzu::QuerySummary )

			result.finish
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
			expect( result.to_a ).to eq([
				{ "a.name" => "Adam", "b.name" => "Karissa", "f.since" => 2020 },
				{ "a.name" => "Adam", "b.name" => "Zhang", "f.since" => 2020 },
				{ "a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021 },
				{ "a.name" => "Zhang", "b.name" => "Noura", "f.since" => 2022 }
			])

			result.finish
		end


		it "handles a #next after it finishes iteration over the current set" do
			setup_demo_db()

			result = described_class.from_query( connection, <<~END_OF_QUERY )
				MATCH ( a:User )-[ f:Follows ]->( b:User )
			    RETURN a.name, b.name, f.since;
			END_OF_QUERY

			result.tuples # Iterate over all tuples

			expect( result.next ).to be_nil

			result.finish
		end


		it "can fetch individual result tuples via the index operator" do
			setup_demo_db()

			result = described_class.from_query( connection, <<~END_OF_QUERY )
				MATCH ( a:User )-[ f:Follows ]->( b:User )
			    RETURN a.name, b.name, f.since;
			END_OF_QUERY

			tuples = result.to_a

			expect( result[0] ).to eq( tuples[0] )
			expect( result[1] ).to eq( tuples[1] )
			expect( result[2] ).to eq( tuples[2] )
			expect( result[3] ).to eq( tuples[3] )

			result.finish
		end


		it "can iterate over result sets" do
			result = described_class.from_query( connection, <<~END_QUERY )
				return 1;
				return 2;
				return 3;
			END_QUERY

			rval = result.each_set.flat_map do |subset|
				subset.to_a
			end

			expect( rval ).to eq([
				{ "1" => 1 },
				{ "2" => 2 },
				{ "3" => 3 },
			])

			result.finish
		end


		it "also finishes successive results if they've been fetched when the first one is finished" do
			result = described_class.from_query( connection, <<~END_QUERY )
				return 1;
				return 2;
				return 3;
			END_QUERY

			second_result = result.next_set

			result.finish

			expect( second_result ).to be_finished
		end


		it "errors if it's used after being finished" do
			result = described_class.from_query( connection, "return 1;" )
			result.finish

			expect( result ).to be_finished

			expect {
				result.get_next_values
			}.to raise_error( Kuzu::FinishedError )
		end

	end


	describe "prepared statement constructor" do

		before( :each ) do
			setup_demo_db()
		end


		it "can be created via a prepared statement" do
			statement = Kuzu::PreparedStatement.new( connection, <<~END_OF_QUERY )
				MATCH (u:User)
			    WHERE u.age > $min_age and u.age < $max_age
			    RETURN u.name
			END_OF_QUERY
			statement.bind( min_age: 32, max_age: 46 )

			result = described_class.from_prepared_statement( statement )

			expect( result ).to be_a( described_class )
			expect( result ).to be_success

			result.finish
		end


		it "is automatically finished if constructed with a block" do
			statement = Kuzu::PreparedStatement.new( connection, <<~END_OF_QUERY )
				MATCH (u:User)
			    WHERE u.age > $min_age and u.age < $max_age
			    RETURN u.name
			END_OF_QUERY
			statement.bind( min_age: 32, max_age: 46 )

			block_result = nil
			rval = described_class.from_prepared_statement( statement ) do |result|
				block_result = result
				expect( result ).to be_a( described_class )
				expect( result ).to be_success

				result.first['u.name']
			end

			expect( block_result ).to be_finished
			expect( rval ).to eq( "Karissa" )
		end

	end


end
