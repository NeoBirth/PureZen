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

use core::ops::Deref;
use heapless;
use typenum::U56;

/// Maximum length of a `Symbol` in bytes.
//
// Value originally selected by ZenGarden to ensure that its `MessageElement`
// type had a size of 64-bytes. See:
//
// <https://github.com/mhroth/ZenGarden/commit/66e787d944c789d6d5ef77eb0e2871c6c51af9ca>
//
// TODO(tarcieri): find a better solution for these than fixed-sized buffers
//
// Some options:
// - enum
// - borrow symbols from the original program
// - arena allocator
// - "intern" symbols as integers
#[allow(non_camel_case_types)]
type SYMBOL_BUFFER_LENGTH = U56;

/// Symbols are Pure Data keywords or command names
#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Symbol(heapless::String<SYMBOL_BUFFER_LENGTH>);

impl Symbol {
    /// Create a new symbol from the given string
    pub fn new<T>(string: T) -> Self
    where
        T: AsRef<str>,
    {
        Symbol(heapless::String::from(string.as_ref()))
    }

    /// Borrow the inner string contents of this `Symbol`
    pub fn as_str(&self) -> &str {
        self.0.as_str()
    }
}

impl AsRef<str> for Symbol {
    fn as_ref(&self) -> &str {
        self.as_str()
    }
}

impl Deref for Symbol {
    type Target = str;

    fn deref(&self) -> &str {
        self.as_str()
    }
}

impl<'a> From<&'a str> for Symbol {
    fn from(s: &str) -> Symbol {
        Symbol::new(s)
    }
}
