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

//! Connection lists (i.e. inlets or outlets)

use super::Connection;
use crate::{
    error::{Error, ErrorKind},
    pd,
};
use core::slice;
use heapless::{self, consts::*, ArrayLength};

/// List of connections to other objects for a given index
/// Arbitrarily chosen maximum length of a list, backed by `heapless::Vec`
#[allow(non_camel_case_types)]
type CONNECTION_LIST_SIZE = U64;

/// Lists of connections to other objects for a given inlet/outlet
// TODO(tarcieri): use an actual DLL, and/or consolidate with a generic list type
#[derive(Clone, Debug, Default)]
pub struct List {
    elements: heapless::Vec<Connection, CONNECTION_LIST_SIZE>,
}

impl List {
    /// Create a new `connection::List`
    pub fn new() -> Self {
        List {
            elements: heapless::Vec::new(),
        }
    }

    /// Append an element onto the end of this `connection::List`
    pub fn push_back(&mut self, connection: Connection) -> Result<(), Error> {
        self.elements
            .push(connection)
            .map_err(|_| ErrorKind::BufferOverflow.into())
    }

    /// Remove a connection from this `connection::List`
    ///
    /// Panics if the connection wasn't found
    // TODO(tarcieri): Don't panic?
    pub fn remove(&mut self, connection: Connection) -> Result<(), Error> {
        if let Some(pos) = self.elements.iter().position(|elem| *elem == connection) {
            self.elements.swap_remove(pos);
        } else {
            panic!("can't remove nonexistent connection: {:?}", connection);
        }

        Ok(())
    }

    /// Iterate over this connection list
    pub fn iter(&self) -> Iter {
        Iter(self.elements.iter())
    }

    /// Get the length of this list
    pub fn len(&self) -> usize {
        self.elements.len()
    }

    /// Is this list empty?
    pub fn is_empty(&self) -> bool {
        self.elements.is_empty()
    }
}

/// List iterator
#[derive(Clone, Debug)]
pub struct Iter<'a>(slice::Iter<'a, Connection>);

impl<'a> Iter<'a> {
    /// Sends the given message to all connected objects in this iterator.
    pub fn send_message<'pd, N>(
        &mut self,
        context: &mut pd::Context<'pd>,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        for connection in self {
            connection
                .object_mut(context)
                .receive_message(connection.index(), message);
        }
    }
}

impl<'a> Iterator for Iter<'a> {
    type Item = &'a Connection;

    fn next(&mut self) -> Option<&'a Connection> {
        self.0.next()
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        self.0.size_hint()
    }

    fn count(self) -> usize {
        self.0.count()
    }
}

impl<'a> ExactSizeIterator for Iter<'a> {
    fn len(&self) -> usize {
        self.0.len()
    }
}
