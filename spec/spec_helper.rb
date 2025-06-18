# -*- ruby -*-

require 'pathname'
require 'simplecov' if ENV['COVERAGE'] || ENV['CI']

require 'tmpdir'
require 'rspec'
require 'loggability/spechelpers'

require 'kuzu'

begin
	require 'observability'
	$have_observability = true

	Observability::Sender.configure( type: :testing )
rescue LoadError => err
	$have_observability = false
end


module Kuzu::SpecHelpers

	# The directory to look in for fixture data
	TEST_DATA_DIR = Pathname( 'spec/data' ).expand_path



	### Inclusion callback -- install some hooks
	def self::included( context )

		context.after( :all ) do
			Kuzu::SpecHelpers.cleanup_tmpfiles
		end

		context.before( :each ) do
			GC.start
		end

	end


	### Clean any files generated during a spec run.
	def self::cleanup_tmpfiles
		Pathname( Dir::Tmpname.tmpdir ).children.
			select{|f| f.basename.to_s.start_with?(/kuzu.*\-test\-/) }.
			each( &:rmtree )
	end


	###############
	module_function
	###############

	### Return a Pathname pointing to a temporary file.
	def tmpfile_pathname( filetype='spec' )
		return Pathname( Dir::Tmpname.create(['kuzu-', '-test-' + filetype]) {} )
	end


	### Return a Pathname containing the path to the test data with the given +name+.
	def test_data_pathname( name )
		return TEST_DATA_DIR / name
	end


	### Load and return the contents of the test data with the given +name+.
	def load_test_data( name )
		file = test_data_pathname( name )
		return file.read
	end

end # module Kuzu::SpecHelpers



RSpec.configure do |config|
	config.expect_with :rspec do |expectations|
		expectations.include_chain_clauses_in_custom_matcher_descriptions = true
		expectations.syntax = :expect
	end

	config.mock_with( :rspec ) do |mock|
		mock.syntax = :expect
		mock.verify_partial_doubles = true
	end

	config.disable_monkey_patching!
	config.example_status_persistence_file_path = "spec/.status"
	config.filter_run :focus
	config.filter_run_excluding :observability unless $have_observability
	config.filter_run_when_matching :focus
	config.order = :random
	config.profile_examples = 5
	config.run_all_when_everything_filtered = true
	config.shared_context_metadata_behavior = :apply_to_host_groups
	config.warnings = true

	config.include( Kuzu::SpecHelpers )
	config.include( Loggability::SpecHelpers )
end
