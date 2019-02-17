# Cruft Tracking

Stuff we'd like to refactor, but aren't for now until the whole thing is
translated, compiling, and passing tests

## `message::symbol`

- "Intern" all symbol strings into an internal symbol cache with integer IDs

## `pd::message`

- `is_float`/`is_symbol` etc should be moved to `pd::message::atom::Atom`
