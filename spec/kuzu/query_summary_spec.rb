# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/query_summary'


RSpec.describe( Kuzu::QuerySummary ) do

	CREATE_TABLE_STATEMENTS = [
		'CREATE NODE TABLE User(name STRING, age INT64, PRIMARY KEY (name))',
		'CREATE NODE TABLE City(name STRING, population INT64, PRIMARY KEY (name))',
	]


	let( :db ) { Kuzu.database }
	let( :connection ) { db.connect }


	it "can return a summary of its query's timing" do
		result = connection.query( CREATE_TABLE_STATEMENTS.first ).query_summary

		expect( result ).to be_a( described_class )
		expect( result.compiling_time ).to be_a( Float ).and( be > 0.0 )
		expect( result.execution_time ).to be_a( Float ).and( be > 0.0 )
	end

end

