<img alt="PureZen" src="https://raw.githubusercontent.com/NeoBirth/PureZen/master/purezen.png" width="50%">

[![Crate][crate-image]][crate-link]
[![Docs][docs-image]][docs-link]
[![Build Status][build-image]][build-link]
[![LGPL 3.0 licensed][license-image]][license-link]
[![Gitter Chat][gitter-image]][gitter-link]

Runtime for the [Pure Data] (Pd) audio programming language, implemented as an
extensible audio library allowing full control over signal processing, message
passing, and graph manipulation.

Written in Rust with intent to target embedded (i.e. `no_std`) platforms.
Adapted from [ZenGarden], which was written in C++.

[Documentation][docs-link]

## Status

Work is underway to translate ZenGarden from C++, and the basic structure of
the Rust crate is in place. However, important core domain objects have not
yet been translated (including anything DSP-related), so it is not ready
to use.

## Concepts

The following four types provide the main interface to **PureZen**:

- `pd::Context` (WIP): core type which owns all PureZen engine memory
- `pd::Graph` (WIP): `message::Object` graphs (and sub-graphs)
- [pd::Message]: control messages for sending commands and triggering sounds
- [message::Object]: objects in a `pd::Graph` which send/receive messages

The first three have to do with how the signal graph is organized.
The latter represents discrete messages which are sent into, processed by,
and out of the graph.

A `pd::Context` represents a unique and independent instance of Pure Data.
Think of it as Pure Data's console window. A context is defined by its
block size, sample rate, and the number of input and output channels.
Contexts are entirely independent and messages and objects cannot be exchanged
between them.

A graph is a collection of objects and the connections between them.
A `pd::Graph` is a `message::Object`, and thus a `pd::Graph` can contain other
`pd::Graph` objects (i.e. subgraphs).

`pd::Message` represents any Pd message, be it a bang or a list of assorted
float, symbols, or bangs. Each `pd::Message` is timestamped and contains at
least one element, and may otherwise contain any number and any combination of
primitives.

**PureZen** messages are always lists of typed elementary types.

### Graph Attachment

*NOTE: This feature is unlikely to be supported soon, but we do want to keep it!*

Whenever any change in the signal graph takes place in Pd, the audio thread
must wait until the reconfiguration is finished. For minor changes such as
removing a connection this can be very fast and avoid underruning the audio
buffer.

For larger changes, such as adding an object requiring significant
initialization, or many changes at once, such as adding a complex abstraction,
audio underruns are almost guaranteed.

**PureZen** solves this problem by allowing an new object or graph to be
created on another thread, and then attached to a context at a convenient
time. As the graph has already been instantiated, the attachement process is a
relatively quick one and can thus be accomplished without causing any audio
dropouts.

Graph attachement generally involves registering global senders and receivers
and ensuring that existing objects are aware of the new ones. Similarly, a
graph can be unattached from a context, leaving it in memory yet inert.

## Code of Conduct

We abide by the [Contributor Covenant][cc] and ask that you do as well.

For more information, please see [CODE_OF_CONDUCT.md].

## License

Copyright © 2009-2019 NeoBirth Developers, Reality Jockey, Ltd.

**PureZen** is licensed under the [LGPL 3.0], which means:

- You must indicate that you are using the **PureZen** library (a fork of [ZenGarden]).
- If you extend the library (not including externals), you must make that code public.
- You may use this library for any application, including commerical ones.

[crate-image]: https://img.shields.io/crates/v/purezen.svg
[crate-link]: https://crates.io/crates/purezen
[docs-image]: https://docs.rs/purezen/badge.svg
[docs-link]: https://docs.rs/purezen/
[build-image]: https://secure.travis-ci.org/NeoBirth/PureZen.svg?branch=master
[build-link]: https://travis-ci.org/NeoBirth/PureZen
[license-image]: https://img.shields.io/badge/license-LGPL%203.0-blue.svg
[license-link]: https://github.com/NeoBirth/PureZen/blob/master/COPYING.LESSER
[gitter-image]: https://badges.gitter.im/NeoBirth/PureZen.svg
[gitter-link]: https://gitter.im/NeoBirth/PureZen
[Pure Data]: https://puredata.info/
[ZenGarden]: https://github.com/mhroth/ZenGarden
[pd::Message]: https://docs.rs/purezen/latest/purezen/pd/struct.Message.html
[message::Object]: https://docs.rs/purezen/latest/purezen/message/object/struct.Object.html
[cc]: https://contributor-covenant.org
[CODE_OF_CONDUCT.md]: https://github.com/NeoBirth/PureZen/blob/master/CODE_OF_CONDUCT.md
[LGPL 3.0]: https://www.gnu.org/licenses/lgpl-3.0.en.html
