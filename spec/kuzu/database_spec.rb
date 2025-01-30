# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/database'


RSpec.describe( Kuzu::Database ) do

	let( :spec_tmpdir ) do
		tmpfile_pathname()
	end


	it "can be created in-memory" do
		instance = described_class.new( '' )
		expect( instance ).to be_a( described_class )
	end


	it "can be created read-only from an existing on-disk database" do
		db_path = spec_tmpdir + 'spec_db'
		original = described_class.new( db_path.to_s )

		ro = described_class.new( db_path.to_s, read_only: true )
		expect( ro ).to be_read_only
	end

end

