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

[![builds.sr.ht status](https://builds.sr.ht/~ged/Ruby-Kuzu.svg)](https://builds.sr.ht/~ged/Ruby-Kuzu?)


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

There are two ways of running queries, immediate execution and via a prepared
statement. Either method returns one or more results as Kuzu::Result objects.


### Results

The each result of a query in a query string is returned as a Kuzu::Result. If
there are more than one queries in the query string, the Results will be
chained together so they can be iterated over.

Each tuple in a Result can be fetched using Kuzu::Result#next, which returns a
Hash of the variables in the `RETURN` clause, keyed by the column name as a
`String`:

    res = conn.query( 'MATCH (a:User)-[f:Follows]->(b:User) RETURN a.name, b.name, f.since;' )
    # => #<Kuzu::Result:0x00000001268449d8 success: true (4 tuples of 3 columns)>
    res.next
    # => {"a.name" => "Adam", "b.name" => "Karissa", "f.since" => 2020}
    res.next
    # => {"a.name" => "Adam", "b.name" => "Zhang", "f.since" => 2020}

The value's type will be determined by the Kuzu datatype of that column of the
result. See Kuzu::Result for a mapping of the types.

If there are more tuples to fetch, Kuzu::Result#has_next? will return `true`:

    res.has_next?
    # => true
    res.next
    # => {"a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021}
    res.next
    # => {"a.name" => "Zhang", "b.name" => "Noura", "f.since" => 2022}
    res.has_next?
    # => false
    res.next
    # => nil

Kuzu::Result is also Enumerable, so you can `#each` over its tuples:

    res.each
    # => #<Enumerator: ...>
    res.map do |tuple|
        "%s has followed %s since %s" % tuple.values_at('a.name', 'b.name', 'f.since')
    end
    # => ["Adam has followed Karissa since 2020",
    #     "Adam has followed Zhang since 2020",
    #     "Karissa has followed Zhang since 2021",
    #     "Zhang has followed Noura since 2022"]

If the query string has more than one query in it, a separate Kuzu::Result is
linked to the previous one, and can be fetched using Kuzu::Result#next_set.

For example:

    res = conn.query( <<~END_OF_QUERY )
        MATCH (a:User)-[f:Follows]->(b:User)
        RETURN a.name, b.name, f.since;
        MATCH (u:User) RETURN *
    END_OF_QUERY
    # => #<Kuzu::Result:0x000000011e6faaf8 success: true (4 tuples of 3 columns)>
    res.to_a
    # => [{"a.name" => "Adam", "b.name" => "Karissa", "f.since" => 2020},
    #     {"a.name" => "Adam", "b.name" => "Zhang", "f.since" => 2020},
    #     {"a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021},
    #     {"a.name" => "Zhang", "b.name" => "Noura", "f.since" => 2022}]
    res.has_next_set?
    # => true
    res2 = res.next_set
    # => #<Kuzu::Result:0x000000011e338fa0 success: true (4 tuples of 1 columns)>
    res2.to_a
    # => [{"u" => #<Kuzu::Node:0x000000011e7553b8 @id=[0, 0], @label="User",
    #       @properties={name: "Adam", age: 30}>},
    #     {"u" => #<Kuzu::Node:0x000000011e7551b0 @id=[0, 1], @label="User",
    #       @properties={name: "Karissa", age: 40}>},
    #     {"u" => #<Kuzu::Node:0x000000011e755048 @id=[0, 2], @label="User",
    #       @properties={name: "Zhang", age: 50}>},
    #     {"u" => #<Kuzu::Node:0x000000011e754ee0 @id=[0, 3], @label="User",
    #       @properties={name: "Noura", age: 25}>}]
    res2.has_next_set?
    # => false
    res2.next_set
    # => nil


### Prepared Statements

An alternative to using strings to query the database is to used *prepared statements*. These have a number of advantages, such as reusability and using parameters for queries instead of string interpolation.

You can create a Kuzu::PreparedStatement by calling Kuzu::Connection#prepare, then execute it one or more times with parameters using Kuzu::PreparedStatement#execute:

    stmt = conn.prepare( <<~END_OF_QUERY )
      MATCH (a:User)-[f:Follows]->(b:User)
      WHERE a.name = $name
      RETURN a.name, b.name, f.since'
    END_OF_QUERY
    # => #<Kuzu::PreparedStatement:0x000000011e919b68>
    res = stmt.execute( name: "Karissa" )
    # => #<Kuzu::Result:0x000000011ee8df90 success: true (1 tuples of 3 columns)>
    res.to_a
    # => [{"a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021}]


### Result Memory Management

Because of the way Ruby frees memory when it's shutting down (i.e., the order is indeterminate), Kuzu::Result objects may not be freed immediately when they go out of scope. To provide some way to manage this, the Kuzu::Result#finish call is provided as a way to explicitly destroy the underlying Kuzu data structure so the Result can be freed. Since this is somewhat inconvenient to manage, there are two forms of Kuzu::Connection#query and Kuzu::PreparedStatement#execute, one which returns a Result and a "bang" equivalent one which just returns success or failure. Additionally, passing a block to either method will yield the Result to the block and then immediately `finish` the Result for you and return the block's value.

    query_string = 'MATCH (a:User)-[f:Follows]->(b:User) RETURN a.name, b.name, f.since'
    conn.query!( query_string )
    # => true
    conn.query( query_string ) {|res| res.tuples }
    # => [{"a.name" => "Adam", "b.name" => "Karissa", "f.since" => 2020},
    #     {"a.name" => "Adam", "b.name" => "Zhang", "f.since" => 2020},
    #     {"a.name" => "Karissa", "b.name" => "Zhang", "f.since" => 2021},
    #     {"a.name" => "Zhang", "b.name" => "Noura", "f.since" => 2022}]

    stmt = conn.prepare( "CREATE (:User {name: $name, age: $age})" )
    # => #<Kuzu::PreparedStatement:0x000000010bc7cfe8>
    stmt.execute!( name: 'David', age: 19 )
    # => true
    stmt.execute!( name: 'Agnes', age: 28 )
    # => true


## Examples

    require 'kuzu'

    db = Kuzu.database
    conn = db.connect
    conn.run("CREATE NODE TABLE User(name STRING, age INT64, PRIMARY KEY (name))")
    conn.run("CREATE NODE TABLE City(name STRING, population INT64, PRIMARY KEY (name))")
    conn.run("CREATE REL TABLE Follows(FROM User TO User, since INT64)")
    conn.run("CREATE REL TABLE LivesIn(FROM User TO City)")

    # Load data.
    conn.run("COPY User FROM \"user.csv\"")
    conn.run("COPY City FROM \"city.csv\"")
    conn.run("COPY Follows FROM \"follows.csv\"")
    conn.run("COPY LivesIn FROM \"lives-in.csv\"")

    # Execute a simple query.
    result = conn.query("MATCH (a:User)-[f:Follows]->(b:User) RETURN a.name, f.since, b.name;")

    # Output query result.
    result.each do |tuple|
        name, since, name2 = tuple.values_at( 'a.name', 'f.since', 'b.name' )
        puts "%s follows %s since %lld", [ name, name2, since ]
    end

    result.finish


## To-Do List

- `UNION` result type.
- `JSON` result type from the JSON extension
- Better memory management for Kuzu::Results


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
