#
# Pry initialization script. Load it like:
#
# $ pry experiments/load-test-db.rb
#

require 'kuzu'

db = Kuzu.database
c = db.connect
c.run( File.read('spec/data/demo-db/schema.cypher') )
c.run( File.read('spec/data/demo-db/copy.cypher') )

binding.pry

