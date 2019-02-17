# PureZen Translation Notes

PureZen is a work-in-progress Rust translation of [ZenGarden], which was
originally written in C++.

This file contains notes on how individual files were translated, and
discrepancies between the translated version and the original C++ code (and,
where applicable, the original Pure Data interpreter).

The translation process is manual, as in addition to doing an outright
translation, it is also simultaneously being adapted to be `no_std` friendly.
This includes gating anything which presently requires the heap to be gated
under the `"alloc"` cargo feature.

The original library ignores several errors (and worse, contains numerous memory
safety errors). PureZen instead is starting with a stricter treatment of input,
and panics on errors rather than ignore them (ideally these should be changed
into `Result` types).

## General Notes

- Redundant prefixes (e.g. `Message*`) are mapped onto Rust modules
- Function names are eagerly translated to `snake_case`

### `MessageElement.cpp` + `MessageElementType.cpp` -> `message/element.rs`

- `MessageElement` renamed to `message::Element`. Seems to overlap greatly with
  `message::Atom` and the two types should probably be consolidated.
- `MessageElementType` renamed to `message::element::Type`

### `MessageObject.cpp` -> `message/object.rs`

- `MessageObject` renamed to `message::Object`

### `PdContext.cpp` -> `pd/context.rs`

- `PdContext` renamed to `pd::Context`

### `PdMessage.cpp` -> `pd/message/mod.rs`

- `MessageAtom` (from `PdMessage.h`) renamed to `message::Atom`
- `PdMessage` renamed to `pd::Message`
- All `PdMessage::initWith*` initializers renamed to `Message::from_*`
- Several `PdMessage` parse errors changed to `Result` instead of silently
  ignoring them. But should we ignore them to match Pd / Zg behavior?

[ZenGarden]: https://github.com/mhroth/ZenGarden
