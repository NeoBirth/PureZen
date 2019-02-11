//
// Copyright © 2009-2019 NeoBirth Developers, Reality Jockey, Ltd.
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

//! Legacy utility functions
//!
//! These should eventually be replaced with Rust `core` and `std` functionality

#[cfg(feature = "alloc")]
use crate::prelude::*;
use core::f32::consts::PI;

/// Legacy string copying function
// TODO: replace all usages with `to_owned()` once all C++ is translated
#[cfg(feature = "alloc")]
pub fn copy_string(s: &str) -> String {
    s.to_owned()
}

/// Legacy check for if value is numeric
// TODO: replace all usages with `parse::<f32>` once all C++ is translated
pub fn is_numeric(s: &str) -> bool {
    s.parse::<f32>().is_ok()
}

/// Legacy string concatenation function
// TODO: replace all usages with `+` once all C++ is translated
#[cfg(feature = "alloc")]
pub fn concat_strings(s0: &str, s1: &str) -> String {
    s0.to_owned() + s1
}

/// Legacy since approximation function
// TODO: replace all usages with `f32::sin`?
pub fn sine_approx(x: f32) -> f32 {
    let a = 4.0f32 / PI; // 1.273239544735163
    let b = 4.0f32 / (PI * PI); // 0.405284734569351
    (a * x) - (b * x) * x.abs()
}

/// Legacy string tokenization function
// TODO: replace all usages with `core::str::Split`
#[cfg(feature = "alloc")]
pub fn tokenize_string<'a>(input: &'a str, delim: &str) -> Vec<&'a str> {
    input.split(delim).collect()
}

/// Legacy file exists function
// TODO: replace all usages with `Path::exists`
#[cfg(feature = "std")]
pub fn file_exists(path: &str) -> bool {
    std::path::Path::new(path).exists()
}
