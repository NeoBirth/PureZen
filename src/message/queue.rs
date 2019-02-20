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

//! Ordered message queue
//!
//! ## Translation Notes
//!
//! //! This file was translated from the following ZenGarden C++ sources:
//!
//! - `message::OrderedQueue.cpp`
//! - `message::OrderedQueue.h`

use super::object::{self, connection};
use crate::{list::List, pd, Error};
use heapless::ArrayLength;

/// Message entries in the queue.
// TODO(tarcieri): refactor this when the overall translation is done
#[derive(Clone, Debug, PartialEq)]
pub struct MessageEntry<'pd, N: ArrayLength<pd::message::Atom<'pd>>> {
    /// Message to be delivered to the given connection index.
    ///
    /// Named `first` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `first` and rename this field to `object_id`
    pub first: pd::Message<'pd, N>,

    /// Index into the connection array.
    ///
    /// Named `second` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `second`
    pub second: connection::Index,
}

/// Object and `MessageEntry` in the message queue.
// TODO(tarcieri): refactor this when the overall translation is done
#[derive(Clone, Debug, PartialEq)]
pub struct ObjectMessageEntry<'pd, N: ArrayLength<pd::message::Atom<'pd>>> {
    /// Identifier for the target object.
    ///
    /// Named `first` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `first` and rename this field to `object_id`
    pub first: object::Id,

    /// `MessageEntry` to be delivered to this object.
    ///
    /// Named `second` for historical reasons: in Zg this was modeled as a
    /// C++ `std::pair`
    // TODO(tarcieri): eliminate all usages of `second`
    pub second: MessageEntry<'pd, N>,
}

impl<'pd, N> ObjectMessageEntry<'pd, N>
where
    N: ArrayLength<pd::message::Atom<'pd>>,
{
    /// Create a new ObjectMessageEntry
    pub fn new(
        message_obj_id: object::Id,
        outlet_index: connection::Index,
        message: pd::Message<'pd, N>,
    ) -> Self {
        let message_entry: MessageEntry<'pd, N> = MessageEntry {
            first: message,
            second: outlet_index,
        };

        ObjectMessageEntry {
            first: message_obj_id,
            second: message_entry,
        }
    }
}

/// Ordered message queue
#[derive(Clone, Debug)]
pub struct OrderedQueue<'pd, N: ArrayLength<pd::message::Atom<'pd>>> {
    entries: List<ObjectMessageEntry<'pd, N>>,
}

impl<'pd, N> OrderedQueue<'pd, N>
where
    N: ArrayLength<pd::message::Atom<'pd>>,
{
    /// Create a new ordered message queue
    pub fn new() -> Self {
        OrderedQueue {
            entries: List::new(),
        }
    }

    /// Inserts the message into the ordered queue based on its scheduled time.
    pub fn insert_message(
        &mut self,
        message_obj_id: object::Id,
        outlet_index: connection::Index,
        message: pd::Message<'pd, N>,
    ) -> Result<(), Error> {
        let mut index = None;

        for (i, entry) in self.entries.iter().enumerate() {
            if message.get_timestamp() < entry.second.first.get_timestamp() {
                index = Some(i);
                break;
            }
        }

        let new_entry = ObjectMessageEntry::new(message_obj_id, outlet_index, message);

        // TODO(tarcieri): this is probably bogus, inefficient, and unnecessary
        if let Some(i) = index {
            self.entries.insert_at(i, new_entry)
        } else {
            self.entries.push_back(new_entry) // insert at end
        }
    }

    /// Removes the given message addressed to the given `message::Object` from the queue.
    pub fn remove_message(
        &mut self,
        message_obj_id: object::Id,
        outlet_index: connection::Index,
        message: &pd::Message<'pd, N>,
    ) {
        let mut index = None;

        for (i, entry) in self.entries.iter().enumerate() {
            if entry.first == message_obj_id
                && entry.second.first.eq(message)
                && entry.second.second == outlet_index
            {
                index = Some(i);
                break;
            }
        }

        if let Some(i) = index {
            self.entries.remove_at(i).unwrap();
        }
    }

    /// Peek at the first message in the queue
    pub fn peek(&self) -> Option<&ObjectMessageEntry<'pd, N>> {
        self.entries.peek()
    }

    /// Pop the first message off of the front of the queue
    // TODO(tarcieri): this is a bad name. We should use `Take` or something.
    pub fn pop(&mut self) -> Option<ObjectMessageEntry<'pd, N>> {
        self.entries.pop()
    }

    /// Is this message queue empty?
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }
}

impl<'pd, N> Default for OrderedQueue<'pd, N>
where
    N: ArrayLength<pd::message::Atom<'pd>>,
{
    fn default() -> Self {
        Self::new()
    }
}
