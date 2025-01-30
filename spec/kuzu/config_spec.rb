# -*- ruby -*-

require_relative '../spec_helper'

require 'kuzu/config'


RSpec.describe( Kuzu::Config ) do

	it "can be created with defaults" do
		result = described_class.new

		expect( result ).to be_a( described_class )

		expect( result.buffer_pool_size ).to be_a( Integer )
		expect( result.max_num_threads ).to be_a( Integer )
		expect( result.enable_compression ).to eq( true )
		expect( result.read_only ).to eq( false )
		expect( result.max_db_size ).to be_a( Integer )
		expect( result.auto_checkpoint ).to eq( true )
		expect( result.checkpoint_threshold ).to be_a( Integer )
	end


	it "can set its buffer_pool_size" do
		instance = described_class.new

		expect {
			instance.buffer_pool_size = 2 ** 11
		}.to change { instance.buffer_pool_size }.to( 2 ** 11 )
	end


	it "can set its max_num_threads" do
		instance = described_class.new

		expect {
			instance.max_num_threads = 4
		}.to change { instance.max_num_threads }.to( 4 )
	end


	it "can disable compression" do
		instance = described_class.new

		expect {
			instance.enable_compression = false
		}.to change { instance.enable_compression }.to( false )
	end


	it "can set read-only mode" do
		instance = described_class.new

		expect {
			instance.read_only = true
		}.to change { instance.read_only }.to( true )
	end


	it "can set read-only mode using a truthy value" do
		instance = described_class.new

		expect {
			instance.read_only = :yep
		}.to change { instance.read_only }.to( true )
	end


	it "can set its max_db_size" do
		instance = described_class.new

		expect {
			instance.max_db_size = 2 ** 21
		}.to change { instance.max_db_size }.to( 2 ** 21 )
	end


	it "can disable auto-checkpointing" do
		instance = described_class.new

		expect {
			instance.auto_checkpoint = false
		}.to change { instance.auto_checkpoint }.to( false )
	end


	it "can set its checkpoint threshold" do
		instance = described_class.new

		expect {
			instance.checkpoint_threshold = 2 ** 22
		}.to change { instance.checkpoint_threshold }.to( 2 ** 22 )
	end


end

