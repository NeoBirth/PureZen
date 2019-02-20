//
// Copyright © 2019 NeoBirth Developers
//
// This file is part of PureZen (a fork of ZenGarden)
//
// PureZen is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PureZen is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with PureZen.  If not, see <http://www.gnu.org/licenses/>.
//

//! **PureZen**: runtime for the [Pure Data] (Pd) audio programming language,
//! implemented as an extensible audio library allowing full control over
//! signal processing, message passing, and graph manipulation.
//!
//! Adapted from [ZenGarden] (originally written in C++) with intent to target
//! embedded (i.e. `no_std`) platforms.
//!
//! [Pure Data]: https://puredata.info/
//! [ZenGarden]: https://github.com/mhroth/ZenGarden

#![no_std]
#![cfg_attr(all(feature = "nightly", not(feature = "std")), feature(alloc))]
#![deny(
    warnings,
    missing_docs,
    trivial_casts,
    trivial_numeric_casts,
    unsafe_code,
    unused_import_braces,
    unused_qualifications
)]
#![forbid(unsafe_code)]
#![doc(
    html_logo_url = "https://raw.githubusercontent.com/NeoBirth/PureZen/master/purezen.png",
    html_root_url = "https://docs.rs/purezen/0.0.1"
)]

#[cfg(any(feature = "std", test))]
#[macro_use]
extern crate std;

#[macro_use]
extern crate failure_derive;

// TODO: remove `pub` on modules which are not part of the public API
pub mod allocator;
mod error;
mod list;
pub mod message;
pub mod pd;
#[cfg(feature = "alloc")]
mod prelude;
pub mod utils;

pub use error::*;
