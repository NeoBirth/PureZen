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

//! Connection tables (i.e. tables of connection inlets or outlets)

use super::list::List;
use crate::pd;
use core::{ops, slice};
use heapless::{self, consts::*, ArrayLength};
use typenum::marker_traits::Unsigned;

/// Size of the connection table
// TODO(tarcieri): this is pulled out of thin air. Tweak it
#[allow(non_camel_case_types)]
type CONNECTION_TABLE_SIZE = U8;

/// Indexes into a connection table
#[derive(Copy, Clone, Debug, Eq, PartialEq, PartialOrd, Ord)]
pub struct Index(pub(crate) usize);

impl From<usize> for Index {
    fn from(n: usize) -> Index {
        debug_assert!(
            n < CONNECTION_TABLE_SIZE::to_usize(),
            "index exceeds CONNECTION_TABLE_SIZE"
        );
        Index(n)
    }
}

/// Connection tables (i.e. table of connection inlets or outlets)
#[derive(Debug, Default)]
pub struct Table {
    /// Connection entries in the connection table
    entries: heapless::Vec<List, CONNECTION_TABLE_SIZE>,
}

impl Table {
    /// Create a new connection table
    pub fn new() -> Self {
        Self {
            entries: heapless::Vec::new(),
        }
    }

    /// Get the number of inlets/outlets in the table
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    /// Is the table empty?
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    /// Iterate over this connection table
    pub fn iter(&self) -> Iter {
        Iter(self.entries.iter())
    }

    /// Sends the given message to all connected objects at the given outlet index.
    pub fn send_message<'pd, N>(
        &self,
        context: &mut pd::Context<'pd>,
        outlet_index: Index,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        self[outlet_index].iter().send_message(context, message)
    }
}

impl ops::Index<Index> for Table {
    type Output = List;

    fn index(&self, index: Index) -> &List {
        self.entries.index(index.0)
    }
}

impl ops::IndexMut<Index> for Table {
    fn index_mut(&mut self, index: Index) -> &mut List {
        self.entries.index_mut(index.0)
    }
}

/// `connection::Table` iterator
#[derive(Clone, Debug)]
pub struct Iter<'a>(slice::Iter<'a, List>);

impl<'a> Iterator for Iter<'a> {
    type Item = &'a List;

    fn next(&mut self) -> Option<&'a List> {
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
