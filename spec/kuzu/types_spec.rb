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
		pending "need to fix the type conversion"

		result = connection.query( "RETURN {first: 'Adam', last: 'Smith'};" )

		expect( result ).to be_a( Kuzu::Result )
		expect( result ).to be_success

		expect( result ).to respond_to( :each )
		expect( result ).to respond_to( :first )

		value = result.first
		expect( value ).to be_a( OpenStruct )
		expect( value.first ).to eq( "Adam" )
		expect( value.first.encoding ).to eq( Encoding::UTF_8 )
		expect( value.last ).to eq( "Smith" )
		expect( value.last.encoding ).to eq( Encoding::UTF_8 )

		result.finish
	end

end
