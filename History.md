# Release History for ruby-kuzu

---

## v0.2.0 [2025-07-16] Michael Granger <ged@FaerieMUD.org>

Enhancements:

- Add support for the SERIAL type.
- Add Kuzu.is_database? method
- Flesh out documentation

Bugfixes:

- Fixups for Linux and Kuzu 0.11
- Fix storage version check (0.11+)
- Fix a bug in Result#next
- Fix Date type conversion


## v0.1.0 [2025-06-17] Michael Granger <ged@FaerieMUD.org>

Enhancements:

- Add Connection#database
- Squelch some warnings
- Add Result#tuples and Result#[].


## v0.0.3 [2025-05-22] Michael Granger <ged@FaerieMUD.org>

Fixes:

- Prevent implicit conversion of nil to Hash in Ruby < 3.4.


## v0.0.2 [2025-05-15] Michael Granger <ged@FaerieMUD.org>

Fixes:

- Fix an RDoc failure
- Add a bit more documentation


## v0.0.1 [2025-05-01] Michael Granger <ged@FaerieMUD.org>

Initial release
