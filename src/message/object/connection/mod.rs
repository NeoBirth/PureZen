//
// Copyright © 2009-2019 NeoBirth Developers
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

//! Management of connections between `message::Object` types

pub mod list;
pub mod table;

pub use self::{
    list::List,
    table::{Index, Table},
};

use crate::{
    allocator::Allocator,
    message::{object, Object},
};

/// Types of connections between objects
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Type {
    /// Describes an audio connection
    DSP,

    /// Describes a message connection
    MESSAGE,
}

/// Entries in our connections table. Named `Connection` to match the Zg name
/// until the translation is finished.
// TODO(tarcieri): refactor this into a proper type for storing connections
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub struct Connection {
    /// Identifier for the target object.
    ///
    /// Named `first` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `first` and rename this field to `object_id`
    pub first: object::Id,

    /// Index into the connection array.
    ///
    /// Named `second` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `first` and rename this field to `index`
    pub second: Index,
}

impl Connection {
    /// Create a new `Connection`
    pub fn new(object_id: object::Id, index: Index) -> Self {
        Connection {
            first: object_id,
            second: index,
        }
    }

    /// Get the object's ID
    pub fn object_id(&self) -> object::Id {
        self.first
    }

    /// Get a reference to the connected object from the given allocator
    pub fn object<'a, A>(&self, allocator: &'a A) -> &'a Object
    where
        A: Allocator<Object>,
    {
        self.object_id().get(allocator)
    }

    /// Get a mutable reference to the connected object from the given allocator
    pub fn object_mut<'a, A>(&self, allocator: &'a mut A) -> &'a mut Object
    where
        A: Allocator<Object>,
    {
        self.object_id().get_mut(allocator)
    }

    /// Get the connection index
    pub fn index(&self) -> Index {
        self.second
    }
}
