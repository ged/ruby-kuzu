#!/usr/bin/env rake

#!/usr/bin/env ruby -S rake

require 'rake/deveiate'

Rake::DevEiate.setup( 'ruby-kuzu' ) do |project|
	project.version_from = 'lib/kuzu.rb'
	project.publish_to = 'deveiate:/usr/local/www/public/code'
	project.rdoc_generator = :sixfish
end


task ci: :gemspec

