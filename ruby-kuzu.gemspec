# -*- encoding: utf-8 -*-
# stub: ruby-kuzu 0.1.0.pre.20250617120046 ruby lib
# stub: ext/kuzu_ext/extconf.rb

Gem::Specification.new do |s|
  s.name = "ruby-kuzu".freeze
  s.version = "0.1.0.pre.20250617120046".freeze

  s.required_rubygems_version = Gem::Requirement.new(">= 0".freeze) if s.respond_to? :required_rubygems_version=
  s.metadata = { "bug_tracker_uri" => "https://todo.sr.ht/~ged/Ruby-Kuzu", "changelog_uri" => "https://deveiate.org/code/ruby-kuzu/History_md.html", "documentation_uri" => "https://deveiate.org/code/ruby-kuzu", "homepage_uri" => "https://sr.ht/~ged/Ruby-Kuzu", "source_uri" => "https://hg.sr.ht/~ged/Ruby-Kuzu" } if s.respond_to? :metadata=
  s.require_paths = ["lib".freeze]
  s.authors = ["Michael Granger".freeze]
  s.date = "2025-06-17"
  s.description = "A Ruby binding for the K\u00F9zu embedded graph database.".freeze
  s.email = ["ged@FaerieMUD.org".freeze]
  s.extensions = ["ext/kuzu_ext/extconf.rb".freeze]
  s.files = ["History.md".freeze, "LICENSE.txt".freeze, "README.md".freeze, "ext/kuzu_ext/config.c".freeze, "ext/kuzu_ext/connection.c".freeze, "ext/kuzu_ext/database.c".freeze, "ext/kuzu_ext/extconf.rb".freeze, "ext/kuzu_ext/kuzu_ext.c".freeze, "ext/kuzu_ext/kuzu_ext.h".freeze, "ext/kuzu_ext/node.c".freeze, "ext/kuzu_ext/prepared_statement.c".freeze, "ext/kuzu_ext/query_summary.c".freeze, "ext/kuzu_ext/recursive_rel.c".freeze, "ext/kuzu_ext/rel.c".freeze, "ext/kuzu_ext/result.c".freeze, "ext/kuzu_ext/types.c".freeze, "lib/kuzu.rb".freeze, "lib/kuzu/config.rb".freeze, "lib/kuzu/connection.rb".freeze, "lib/kuzu/database.rb".freeze, "lib/kuzu/node.rb".freeze, "lib/kuzu/prepared_statement.rb".freeze, "lib/kuzu/query_summary.rb".freeze, "lib/kuzu/recursive_rel.rb".freeze, "lib/kuzu/rel.rb".freeze, "lib/kuzu/result.rb".freeze, "spec/kuzu/config_spec.rb".freeze, "spec/kuzu/database_spec.rb".freeze, "spec/kuzu/prepared_statement_spec.rb".freeze, "spec/kuzu/query_summary_spec.rb".freeze, "spec/kuzu/result_spec.rb".freeze, "spec/kuzu/types_spec.rb".freeze, "spec/kuzu_spec.rb".freeze, "spec/spec_helper.rb".freeze]
  s.homepage = "https://sr.ht/~ged/Ruby-Kuzu".freeze
  s.licenses = ["BSD-3-Clause".freeze]
  s.rubygems_version = "3.6.7".freeze
  s.summary = "A Ruby binding for the K\u00F9zu embedded graph database.".freeze

  s.specification_version = 4

  s.add_runtime_dependency(%q<rake-compiler>.freeze, ["~> 1.2".freeze])
  s.add_development_dependency(%q<rake-deveiate>.freeze, ["~> 0.25".freeze])
  s.add_development_dependency(%q<rdoc-generator-sixfish>.freeze, ["~> 0.3".freeze])
  s.add_development_dependency(%q<ruby-lsp>.freeze, ["~> 0.23".freeze])
end
