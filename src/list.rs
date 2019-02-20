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

//! List type. Used in the places ZenGarden was previously using the
//! [C++ std::list] type. Should probably be re-evaluated / re-implemented /
//! replaced.
//!
//! [C++ std::list]: http://www.cplusplus.com/reference/list/list/

// TODO(tarcieri): implement this better or replace it with something else

use crate::{Error, ErrorKind};
use core::slice;
use heapless;
use typenum::U64;

/// Arbitrarily chosen maximum length of a list, backed by `heapless::Vec`
#[allow(non_camel_case_types)]
type MAX_LIST_SIZE = U64;

/// `List` type, used as a temporary replacement for C++ `std::list` in order
/// to make progress with the translation. Potentially needs to be replaced
/// with an actual doubly-linked list type which allocates from a heap/arena.
// TODO: replace this with a real linked list-like data structure
#[derive(Clone, Debug, Default)]
pub struct List<T>(heapless::Vec<T, MAX_LIST_SIZE>);

impl<T> List<T> {
    /// Create a new `List`
    pub fn new() -> Self {
        List(heapless::Vec::new())
    }

    /// Append an element onto the end of `List`
    pub fn append(&mut self, elem: T) -> Result<(), Error> {
        self.0
            .push(elem)
            .map_err(|_| ErrorKind::BufferOverflow.into())
    }

    /// Alias for append
    // TODO(tarcieri): have everything use `append` instead of `push_back`
    pub fn push_back(&mut self, elem: T) -> Result<(), Error> {
        self.append(elem)
    }

    /// Iterate over this list
    pub fn iter(&self) -> Iter<T> {
        Iter(self.0.iter())
    }

    /// Is this list empty?
    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
    }

    /// Insert an element at the given position in the list
    // TODO(tarcieri): probably not the best way to actually implement this
    pub fn insert_at(&mut self, index: usize, elem: T) -> Result<(), Error> {
        if index > self.0.len() {
            Err(ErrorKind::IndexOutOfBounds)?;
        }

        // NOTE(tarcieri): this is a weird way to do this, but leverages `swap_remove`
        // to individually replace the elements
        if self.0.push(elem).is_err() {
            // TODO(tarcieri) is this really the only error case?
            Err(ErrorKind::BufferOverflow)?;
        }

        // TODO(tarcieri): does this even work correctly? Write some tests
        for i in index..self.0.len() {
            let value = self.0.swap_remove(i);
            if self.0.push(value).is_err() {
                // TODO(tarcieri) is this really the only error case?
                Err(ErrorKind::BufferOverflow)?;
            }
        }

        Ok(())
    }

    /// Remove an element from the given position in the list
    // TODO(tarcieri): probably not the best way to actually implement this
    pub fn remove_at(&mut self, index: usize) -> Result<T, Error> {
        if index > self.0.len() {
            Err(ErrorKind::IndexOutOfBounds)?;
        }

        let mut result = None;
        let mut tmp = heapless::Vec::new();

        for i in (self.0.len()..0).rev() {
            if i == index {
                result = Some(self.0.pop().unwrap());
            } else if tmp.push(self.0.pop().unwrap()).is_err() {
                // TODO(tarcieri) is this really the only error case?
                Err(ErrorKind::BufferOverflow)?;
            }
        }

        self.0 = tmp;
        Ok(result.unwrap())
    }

    /// Peek at the value at the head of the list
    pub fn peek(&self) -> Option<&T> {
        self.0.get(0)
    }

    /// Pop the first message off of the front of the list
    // TODO(tarcieri): this is a bad name. We should use `Take` or something.
    pub fn pop(&mut self) -> Option<T> {
        self.remove_at(0).ok()
    }
}

/// `List` iterator
#[derive(Clone, Debug)]
pub struct Iter<'a, T>(slice::Iter<'a, T>);

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<&'a T> {
        self.0.next()
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        self.0.size_hint()
    }

    fn count(self) -> usize {
        self.0.count()
    }
}

impl<'a, T> ExactSizeIterator for Iter<'a, T> {
    fn len(&self) -> usize {
        self.0.len()
    }
}
