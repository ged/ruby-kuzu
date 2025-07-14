#!/usr/bin/env ruby -S rspec

require_relative 'spec_helper'
require 'kuzu'


RSpec.describe( Kuzu ) do

	let( :spec_tmpdir ) do
		path = tmpfile_pathname()
		path.mkpath
		return path
	end


	it "should have a VERSION constant" do
		expect( subject.const_get('VERSION') ).to_not be_empty
	end


	it "knows what version of Kuzu it's linked with" do
		expect( described_class.kuzu_version ).to match( /\A\d+\.\d+\.\d+/ )
	end


	it "knows what version of the storage scheme it's using" do
		result = described_class.storage_version

		expect( result ).to be_an( Integer )
		expect( result ).to be > 0
	end


	it "can construct an in-memory database with reasonable defaults" do
		expect( described_class.database ).to be_a( Kuzu::Database )
	end


	it "can construct an in-memory database with a nil path" do
		expect( described_class.database(nil) ).to be_a( Kuzu::Database )
	end


	it "can construct an in-memory database explicitly" do
		expect( described_class.database(:memory) ).to be_a( Kuzu::Database )
	end


	it "can construct a on-disk database with reasonable defaults" do
		filename = spec_tmpdir + 'spec_db'
		result = described_class.database( filename )

		expect( result ).to be_a( Kuzu::Database )
		if Kuzu.storage_version <= 38
			expect( filename ).to be_a_directory
		else
			expect( filename ).to be_a_file
		end
	end


	it "can tell whether a string looks like a path to a Kuzu database" do
		path = spec_tmpdir + 'spec_db'

		expect {
			described_class.database( path )
		}.to change {
			described_class.is_database?( path.to_s )
		}.from( false ).to( true )
	end


	it "can tell whether a Pathname looks like a path to a Kuzu database" do
		path = spec_tmpdir + 'spec_db'

		expect {
			described_class.database( path )
		}.to change {
			described_class.is_database?( path )
		}.from( false ).to( true )
	end

end
