#!/usr/bin/env ruby -S rspec

require_relative 'spec_helper'
require 'kuzu'


describe Kuzu do
  it "should have a VERSION constant" do
    expect(subject.const_get('VERSION')).to_not be_empty
  end
end
