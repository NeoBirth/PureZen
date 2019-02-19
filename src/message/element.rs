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

//! Message Elements: Components of a Pure Data message
//!
//! ## Translation Notes
//!
//! This file was translated from the following ZenGarden C++ sources:
//!
//! - `MessageElement.cpp`
//! - `MessageElementType.cpp`
//!
//! `MessageElement` renamed to `message::Element`. It seems extremely similar
//! to `message::Atom` (i.e. the two types should probably be consolidated).
//! For now `message::Atom` borrows from the Pure Data sources it's parsed
//! from, whereas `message::Element` is an owned type.
//!
//! `MessageElementType` was renamed to `message::element::Type`

use super::Symbol;
use crate::pd;

/// Types of message elements
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Type {
    /// Any value (placeholder for uninitialized values)
    // TODO: eliminate this state by always initializing all message elements
    ANYTHING,

    /// `bang`: Typically used to trigger an object to perform an action.
    BANG,

    /// `float`: Single-precision floating point value
    FLOAT,

    /// `list`: Lists of other elements
    LIST,

    /// `symbol`: Symbol messages (i.e. keywords)
    SYMBOL,
}

/// Message Elements: Components of a Pure Data message
#[derive(Clone, Debug, PartialEq)]
pub enum Element {
    /// Any value (placeholder for uninitialized values)
    // TODO: eliminate this and always initialize element vectors (if possible?)
    Anything,

    /// `bang`: Typically used to trigger an object to perform an action.
    Bang,

    /// `float`: Single-precision floating point value
    Float(f32),

    /// `list`: Lists of other elements
    // TODO: list support
    List(),

    /// `symbol`: Symbols i.e. Pure Data keywords (e.g. `float`, `list`, `symbol`)
    Symbol(Symbol),
}

impl Element {
    /// Get the element type for this element
    pub fn get_type(&self) -> Type {
        match self {
            Element::Anything => Type::ANYTHING,
            Element::Bang => Type::BANG,
            Element::Float(_) => Type::FLOAT,
            Element::List() => Type::LIST,
            Element::Symbol(_) => Type::SYMBOL,
        }
    }

    /// Obtain the numeric constant value for this element
    pub fn get_float(&self) -> Option<f32> {
        match self {
            Element::Float(n) => Some(*n),
            _ => None,
        }
    }

    /// Get the slice containing an inner list of elements
    pub fn get_list(&self) -> Option<&[Element]> {
        panic!("unimplemented!")
    }

    /// Obtain the symbol value for this element
    pub fn get_symbol(&self) -> Option<&str> {
        match self {
            Element::Symbol(s) => Some(s),
            _ => None,
        }
    }

    /// Is this element an anything placeholder value?
    pub fn is_anything(&self) -> bool {
        self.get_type() == Type::ANYTHING
    }

    /// Is this element a `bang`?
    pub fn is_bang(&self) -> bool {
        self.get_type() == Type::BANG
    }

    /// Is this element a `float`?
    pub fn is_float(&self) -> bool {
        self.get_float().is_some()
    }

    /// Is this element a `list`?
    pub fn is_list(&self) -> bool {
        self.get_list().is_some()
    }

    /// Is this element a `symbol`?
    pub fn is_symbol(&self) -> bool {
        self.get_symbol().is_some()
    }

    /// Is this element a `symbol` which matches the given string?
    pub fn is_symbol_str(&self, test: &str) -> bool {
        self.get_symbol() == Some(test)
    }

    /// Set the anything value on this element
    // TODO: eliminate these methods by setting values directly
    pub fn set_anything(&mut self) {
        panic!("unimplemented");
    }

    /// Set the bang value on this element
    // TODO: eliminate these methods by setting values directly
    pub fn set_bang(&mut self) {
        panic!("unimplemented");
    }

    /// Set the floating point value of this element
    // TODO: eliminate these methods by setting values directly
    pub fn set_float(&mut self, new_value: f32) {
        match self {
            Element::Float(ref mut value) => *value = new_value,
            _ => panic!("can't set_float on {:?}!", self),
        }
    }

    /// Set this value to a list
    // TODO: eliminate these methods by setting values directly
    pub fn set_list(&mut self) {
        panic!("unimplemented");
    }

    /// Set the floating point value of this element
    // TODO: eliminate these methods by setting values directly
    pub fn set_symbol(&mut self, new_symbol: &str) {
        match self {
            Element::Symbol(ref mut symbol) => *symbol = Symbol::new(new_symbol),
            _ => panic!("cann't set_symbol on {:?}!", self),
        }
    }
}

impl From<f32> for Element {
    fn from(value: f32) -> Element {
        Element::Float(value)
    }
}

impl<'a> From<&'a str> for Element {
    fn from(s: &str) -> Element {
        Element::Symbol(Symbol::new(s))
    }
}

impl<'a> From<pd::message::Atom<'a>> for Element {
    fn from(atom: pd::message::Atom) -> Element {
        match atom {
            pd::message::Atom::Anything => Element::Anything,
            pd::message::Atom::Bang => Element::Bang,
            pd::message::Atom::Float(f) => Element::Float(f),
            pd::message::Atom::List(_) => Element::List(),
            pd::message::Atom::Symbol(s) => Element::Symbol(Symbol::from(s)),
        }
    }
}
