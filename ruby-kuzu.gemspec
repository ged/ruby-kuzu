# -*- encoding: utf-8 -*-
# stub: ruby-kuzu 0.1.0.pre.20250126204603 ruby lib

Gem::Specification.new do |s|
  s.name = "ruby-kuzu".freeze
  s.version = "0.1.0.pre.20250126204603".freeze

  s.required_rubygems_version = Gem::Requirement.new(">= 0".freeze) if s.respond_to? :required_rubygems_version=
  s.metadata = { "bug_tracker_uri" => "https://todo.sr.ht/~ged/ruby-kuzu", "changelog_uri" => "https://deveiate.org/code/ruby-kuzu/History_md.html", "documentation_uri" => "https://deveiate.org/code/ruby-kuzu", "homepage_uri" => "https://hg.sr.ht/~ged/ruby-kuzu", "source_uri" => "https://hg.sr.ht/~ged/ruby-kuzu" } if s.respond_to? :metadata=
  s.require_paths = ["lib".freeze]
  s.authors = ["Michael Granger".freeze]
  s.date = "2025-01-26"
  s.description = "A Ruby binding for the K\u00F9zu embedded graph database.".freeze
  s.email = ["ged@FaerieMUD.org".freeze]
  s.files = ["History.md".freeze, "LICENSE.txt".freeze, "README.md".freeze, "lib/kuzu.rb".freeze, "spec/kuzu_spec.rb".freeze, "spec/spec_helper.rb".freeze]
  s.homepage = "https://hg.sr.ht/~ged/ruby-kuzu".freeze
  s.licenses = ["BSD-3-Clause".freeze]
  s.rubygems_version = "3.6.2".freeze
  s.summary = "A Ruby binding for the K\u00F9zu embedded graph database.".freeze

  s.specification_version = 4

  s.add_runtime_dependency(%q<rake-compiler>.freeze, ["~> 1.2".freeze])
  s.add_development_dependency(%q<rake-deveiate>.freeze, ["~> 0.25".freeze])
  s.add_development_dependency(%q<rdoc-generator-sixfish>.freeze, ["~> 0.3".freeze])
end
