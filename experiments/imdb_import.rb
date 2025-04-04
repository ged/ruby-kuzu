#!/usr/bin/env ruby

# Ruby port of the original importer for Nim by Mahlon E. Smith.
#
# Copyright (c) 2025 Mahlon E. Smith
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the author/s, nor the names of the project's
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

require 'pathname'
require 'uri'
require 'open-uri'
require 'zlib'
require 'csv'
require 'tty-progressbar'

require 'kuzu'

DB     = "imdb"
SOURCE = "https://datasets.imdbws.com"
FILES  = [ "name.basics", "title.basics", "title.principals" ]

SCHEMA = <<END_OF_SCHEMA
CREATE NODE TABLE Actor (actorId INT64, name STRING, birthYear UINT16,
	deathYear UINT16, PRIMARY KEY (actorId));
CREATE NODE TABLE Movie (movieId INT64, title STRING, year UINT16,
	durationMins INT, PRIMARY KEY (movieId));
CREATE REL TABLE ActedIn (FROM Actor TO Movie);
END_OF_SCHEMA

PROGRESS_BAR = '[:bar] :current/:total :percent ETA::eta :rate/s'


CSV::Converters[:nulls] = ->( field ) do
	field == '\\N' ? nil : field
end


def main( *argv )
	db = create_database()

	FILES.each do |file|
		tsv_file = Pathname( "#{file}.tsv" )

		download_tsv( tsv_file )
		import_data( db, tsv_file )
	end

end


def create_database
	if File.directory?( DB )
		$stderr.puts "Database already exists. Aborting."
		exit( 1 )
	end

	$stderr.print "Creating IMDB database..."
	db = Kuzu.database( DB )
	conn = db.connect
	conn.run( SCHEMA )
	$stderr.puts "done."

	return db
end


def download_tsv( filepath )
	if filepath.exist?
		$stderr.puts "%s already exists; skipping." % [ filepath ]
		return
	end

	$stderr.print "Downloading %s..." % [ filepath ]

	uri = URI( "#{SOURCE}/#{filepath}.gz" )
	bytes = 0
	filepath.open( File::CREAT|File::WRONLY|File::EXCL, 0644, encoding: 'utf-8' ) do |fh|
		uri.open do |tempfile|
			reader = Zlib::GzipReader.new( tempfile )
			bytes = IO.copy_stream( reader, fh )
			reader.close
		end
	end

	$stderr.puts "done: %0.2K" % [ bytes/1024 ]
rescue => err
	$stderr.puts "%p while downloading %s: %s" % [ err.class, filepath, err.message ]
	filepath.unlink if filepath.exist?
end


def import_data( db, tsv_file )
	file = tsv_file.basename( '.tsv' )
	csv = CSV.open( tsv_file, headers: true, col_sep: "\t",
		converters: %i[integer date nulls] )
	conn = db.connect

	case file.to_s
	when 'name.basics' then import_actors( conn, csv )
	when 'title.basics' then import_titles( conn, csv )
	when 'title.principals' then import_roles( conn, csv )
	else
		raise "Don't know how to import the %s file" % [ file ]
	end
end


def import_actors( conn, csv )
	total_lines = File.open( csv.path ).each_line.count
	progress = TTY::ProgressBar.new( "Actors: #{PROGRESS_BAR}",
		hide_cursor: true, total: total_lines, bar_format: :block )

	insert = conn.prepare( <<~CREATE_ACTOR )
		CREATE (actor:Actor {
			actorId: $actor_id,
			name: $name,
			birthYear: $birth_year,
			deathYear: $death_year
		});
	CREATE_ACTOR

	data = {} # Reuse the same hash
	csv.each_with_index do |row, i|
		data[ :actor_id ] = row['nconst'].slice( 2..-1 ).to_i
		data[ :name ] = row['primaryName']
		data[ :birth_year ] = row['birthYear']
		data[ :death_year ] = row['deathYear']

		# $stderr.puts( data )
		insert.execute( **data ) do |res|
			raise "CREATE failed!" unless res.success?
			# GC.start if (i % 1000).zero?
		end

		progress.advance
	end
rescue Interrupt, StandardError => err
	progress&.stop
	raise( err )
end


def import_titles( conn, csv )
	$stderr.puts "Importing titles..."
end


def import_roles( conn, csv )
	$stderr.puts "Importing actors..."
end


if __FILE__ == $0
	$stderr.sync = true
	main()
end
