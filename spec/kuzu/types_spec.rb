# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu'


RSpec.describe( "data types" ) do

	let( :db ) { Kuzu.database }
	let( :connection ) { db.connect }


	#
	# Specs
	#

	it "converts STRUCT values to OpenStructs" do
		result = connection.query( "RETURN {first: 'Adam', last: 'Smith'};" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success

		value = result.first
		expect( value ).to be_a( OpenStruct )
		expect( value.first ).to eq( "Adam" )
		expect( value.first.encoding ).to eq( Encoding::UTF_8 )
		expect( value.last ).to eq( "Smith" )
		expect( value.last.encoding ).to eq( Encoding::UTF_8 )

	end

end
