# ruby-kuzu

home
: https://sr.ht/~ged/Ruby-Kuzu

code
: https://hg.sr.ht/~ged/Ruby-Kuzu

github
: https://github.com/ged/ruby-kuzu

docs
: https://deveiate.org/code/ruby-kuzu


## Description

A Ruby binding for the Kùzu embedded graph database.


### Creating A Database and Connecting To It

To create an in-memory database:

    database = Kuzu.database
    # => #<Kuzu::Database:0x000000012917ec68 path:nil read-only:false>

Or explicitly via an empty string:

    database = Kuzu.database( '' )
    # => #<Kuzu::Database:0x000000012144edb0 path:nil read-only:false>

If you pass a non-empty string, it is assumed to be the path to a database:

    database = Kuzu.database( 'path/to/mydb' )
    # => #<Kuzu::Database:0x0000000121624d10 path:"path/to/mydb" read-only:false>

There's also support for passing configuration options to the database
handle as keyword arguments to the `.database` method:

    database = Kuzu.database( 'mydb', read_only: true )
    # => #<Kuzu::Database:0x00000001227aa5a8 path:"mydb" read-only:true>

Once you have a Kuzu::Database object, you need a connection to actually use it:

    conn = db.connect
    # => #<Kuzu::Connection:0x0000000122a41f28 threads:16>



### Querying

### Results

### Prepared Statements

### Datatypes



## Examples

    require 'kuzu'

    db = Kuzu.database
    conn = db.connect
    conn.query("CREATE NODE TABLE User(name STRING, age INT64, PRIMARY KEY (name))")
    conn.query("CREATE NODE TABLE City(name STRING, population INT64, PRIMARY KEY (name))")
    conn.query("CREATE REL TABLE Follows(FROM User TO User, since INT64)")
    conn.query("CREATE REL TABLE LivesIn(FROM User TO City)")

    # Load data.
    conn.query("COPY User FROM \"user.csv\"")
    conn.query("COPY City FROM \"city.csv\"")
    conn.query("COPY Follows FROM \"follows.csv\"")
    conn.query("COPY LivesIn FROM \"lives-in.csv\"")

    # Execute a simple query.
    result = conn.query("MATCH (a:User)-[f:Follows]->(b:User) RETURN a.name, f.since, b.name;")

    # Output query result.
    result.each do |tuple|
        name, since, name2 = tuple.values_at( :a_name, :f_since, :b_name )
        puts "%s follows %s since %lld", [ name, name2, since ]
    end

## Requirements

- Ruby >= 3
- Kuzu >= 0.9


## Install

    $ gem install kuzu


## Authors

- Michael Granger <ged@FaerieMUD.org>


## Copyright

Copyright (c) 2025 Michael Granger

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
