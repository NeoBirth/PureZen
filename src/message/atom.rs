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

use super::element;
use core::fmt::{self, Display};

/// Message atoms (a.k.a. elements)
#[derive(Copy, Clone, Debug, PartialEq, PartialOrd)]
pub enum Atom<'pd> {
    /// Any value (placeholder for uninitialized values)
    // TODO: eliminate this and always initialize atom vectors (if possible?)
    Anything,

    /// `bang`: Typically used to trigger an object to perform an action.
    Bang,

    /// `float`: Single-precision floating point value
    Float(f32),

    /// `list`: Lists of other atoms
    // TODO: real lifetimes
    List(&'pd [Atom<'pd>]),

    /// `symbol`: Symbols i.e. Pure Data keywords (e.g. `float`, `list`, `symbol`)
    Symbol(&'pd str),
}

impl<'pd> Atom<'pd> {
    /// Get the element type for this atom
    pub fn get_type(&self) -> element::Type {
        match self {
            Atom::Anything => element::Type::ANYTHING,
            Atom::Bang => element::Type::BANG,
            Atom::Float(_) => element::Type::FLOAT,
            Atom::List(_) => element::Type::LIST,
            Atom::Symbol(_) => element::Type::SYMBOL,
        }
    }

    /// Obtain the numeric constant value for this atom
    pub fn get_float(&self) -> Option<f32> {
        match self {
            Atom::Float(n) => Some(*n),
            _ => None,
        }
    }

    /// Get the slice containing an inner list of atoms
    pub fn get_list(&self) -> Option<&[Atom]> {
        match self {
            Atom::List(l) => Some(l),
            _ => None,
        }
    }

    /// Obtain the symbol value for this atom
    pub fn get_symbol(&self) -> Option<&str> {
        match self {
            Atom::Symbol(s) => Some(s),
            _ => None,
        }
    }

    /// Is this atom an anything placeholder value?
    pub fn is_anything(&self) -> bool {
        self.get_type() == element::Type::ANYTHING
    }

    /// Is this atom a `bang`?
    pub fn is_bang(&self) -> bool {
        self.get_type() == element::Type::BANG
    }

    /// Is this atom a `float`?
    pub fn is_float(&self) -> bool {
        self.get_float().is_some()
    }

    /// Is this atom a `list`?
    pub fn is_list(&self) -> bool {
        self.get_list().is_some()
    }

    /// Is this atom a `symbol`?
    pub fn is_symbol(&self) -> bool {
        self.get_symbol().is_some()
    }

    /// Is this atom a `symbol` which matches the given string?
    pub fn is_symbol_str(&self, test: &str) -> bool {
        self.get_symbol() == Some(test)
    }
}

impl<'pd> Display for Atom<'pd> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Atom::Bang => write!(f, "bang"),
            Atom::Float(value) => write!(f, "{}", value),
            Atom::Symbol(symbol) => write!(f, "{}", symbol),
            _ => Ok(()), // TODO(tarcieri): better string inspection here?
        }
    }
}

impl<'pd> From<f32> for Atom<'pd> {
    fn from(value: f32) -> Atom<'pd> {
        Atom::Float(value)
    }
}

impl<'pd> From<&'pd str> for Atom<'pd> {
    fn from(s: &'pd str) -> Atom<'pd> {
        Atom::Symbol(s)
    }
}
