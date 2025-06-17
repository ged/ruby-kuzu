# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/connection'


RSpec.describe( Kuzu::Connection ) do

	let( :db ) { Kuzu.database }


	it "can set the maximum number of threads for execution" do
		connection = db.connect

		expect {
			connection.max_num_threads_for_exec += 1
		}.to change { connection.max_num_threads_for_exec }.by( 1 )
	end


	it "shows the number of threads used for execution when inspected" do
		connection = db.connect

		expect( connection.inspect ).to match( /threads:\d+/i )
	end


	it "knows what database it's a connection for" do
		connection = db.connect

		expect( connection.database ).to eq( db )
	end

end

