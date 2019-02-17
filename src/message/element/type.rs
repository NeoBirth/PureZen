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

//! Message elements

/// Message element types
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Type {
    /// Any value (placeholder for uninitialized values)
    ANYTHING,

    /// `bang`: Typically used to trigger an object to perform an action.
    BANG,

    /// `float`: Single-precision floating point value
    FLOAT,

    /// `list`: Lists of other atoms
    LIST,

    /// `symbol`: Symbol messages (i.e. keywords)
    SYMBOL,
}

impl Default for Type {
    fn default() -> Self {
        Type::BANG
    }
}
