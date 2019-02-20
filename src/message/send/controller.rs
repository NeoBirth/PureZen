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

//! Message send controller

use crate::{message::object::connection, pd, Error, ErrorKind};
use heapless::{self, consts::*, ArrayLength};

/// Special index for referencing the system "pd" receiver
// TODO(tarcieri): use an enum for this?
const SYSTEM_NAME_INDEX: usize = 0x7FFF_FFFF;

/// Size of the send stack
// TODO(tarcieri): chosen completely arbitrarily. Fine tune this
#[allow(non_camel_case_types)]
type SEND_STACK_SIZE = U64;

/// Set type used in the internal implementation of `Controller`
// TODO(tarcieri): find better data structures for this?
#[derive(Clone, Debug)]
struct Set<T> {
    members: heapless::Vec<T, U32>,
}

impl<T> Set<T> {
    /// Create a new set
    pub fn new() -> Self {
        Set {
            members: heapless::Vec::new(),
        }
    }

    /// Insert an element into this set
    pub fn insert(&mut self, element: T) {
        assert!(self.members.push(element).is_ok())
    }

    /// Erase a value from this set
    pub fn erase(&mut self, _element: T) {
        panic!("unimplemented!")
    }
}

/// Placeholder for `RemoteMessageReceiver`
// TODO(tarcieri): translate C++ type
#[derive(Clone, Debug)]
pub struct RemoteMessageReceiver {}

impl RemoteMessageReceiver {
    /// TODO(tarcieri): actually implement this type
    pub fn receive_message<'pd, N>(
        &mut self,
        _index: connection::Index,
        _message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        panic!("unimplemented!")
    }

    /// TODO(tarcieri): actually implement this type
    pub fn get_name(&self) -> &str {
        panic!("unimplemented!")
    }
}

/// Message send controller object.
///
/// Because of features such as external message injection and implicit
/// message sending from message boxes, it must be possible to `send` a
/// message to associated `receive`ers without explicitly requiring a `send`
/// object to exist in the graph. The `message::send::Controller` is intended
/// as a central dispatcher for all sent messages, delivering a message to all
/// associated `receive`ers without requiring the existence of a `send` object.
///
/// It can deliver messages via the usual `receive_message()` function, where
/// the inlet index identifies a receiver name. The index for a receiver name
/// can be determined with `get_name_index()`. This is useful for messages that
/// must be scheduled in the global message queue and must then be dispatched
/// via the `receive_message()` function.
///
/// Alternatively, a message can be sent to receivers using `receive_message()`
/// with name and message arguments (instead of inlet index and message).
/// Messages sent using this alternative will be sent right away (avoiding the
/// message queue).
// TODO(tarcieri): handle previous `MessageObject` superclass behaviors
#[derive(Clone, Debug)]
pub struct Controller {
    // : public MessageObject
    /// Stack of senders
    // TODO(zg): explore using a hash table with receiver name keys and `List` values
    send_stack: heapless::Vec<(heapless::String<U32>, Set<RemoteMessageReceiver>), SEND_STACK_SIZE>,

    /// Set of external receivers
    external_receiver_set: Set<heapless::String<U32>>,
}

impl Controller {
    /// Create a new `message::send::Controller`
    pub fn new() -> Self {
        // message::Object(0, 0, NULL) {
        let send_stack = heapless::Vec::new();
        let external_receiver_set = Set::new();
        Controller {
            send_stack,
            external_receiver_set,
        }
    }

    /// Returns `true` if a receiver by that name is already registered, or
    /// `false` otherwise.
    pub fn receiver_exists(&self, receiver_name: &str) -> bool {
        self.get_name_index(receiver_name).is_some()
    }

    /// Returns the index to which the given receiver name is referenced.
    /// Used with `receive_message(usize, &pd::Message<_>)`.
    pub fn get_name_index(&self, receiver_name: &str) -> Option<connection::Index> {
        if receiver_name == "pd" {
            // a special case for sending messages to the system
            return Some(connection::Index(SYSTEM_NAME_INDEX));
        }

        for i in 0..self.send_stack.len() {
            if self.send_stack[i].0 == receiver_name {
                return Some(connection::Index(i));
            }
        }

        None
    }

    /// Sends the message on to all receivers with the given name
    pub fn receive_message<'pd, N>(
        &mut self,
        context: &mut pd::Context<'pd>,
        name: &str,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        if let Some(index) = self.get_name_index(name) {
            // if the receiver name is not registered, nothing to do
            self.send_message(context, index, message);
        }

        // check to see if the receiver name has been registered as an external receiver
        if self
            .external_receiver_set
            .members
            .iter()
            .any(|member| member.as_str() == name)
        {
            // TODO(tarcieri): callback function support
            // context.callback_function(ZG_RECEIVER_MESSAGE, context.callback_user_data, &(name, message));
        }
    }

    /// Send a message to the given outlet index
    pub fn send_message<'pd, N>(
        &mut self,
        context: &mut pd::Context<'pd>,
        outlet_index: connection::Index,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        if outlet_index == connection::Index(SYSTEM_NAME_INDEX) {
            context.receive_system_message(message);
        } else {
            for receiver in self.send_stack[outlet_index.0].1.members.iter_mut() {
                receiver.receive_message(connection::Index(0), message);
            }
        }
    }

    /// Add a receiver to this send controller
    pub fn add_receiver(&mut self, receiver: RemoteMessageReceiver) -> Result<(), Error> {
        let name_index = match self.get_name_index(receiver.get_name()) {
            Some(i) => i.0,
            None => {
                let mut remote_set = Set::new();
                remote_set.insert(receiver.clone());

                if self
                    .send_stack
                    .push((receiver.get_name().into(), remote_set))
                    .is_err()
                {
                    // TODO(tarcieri) is this really the only error case?
                    Err(ErrorKind::BufferOverflow)?;
                }
                self.send_stack.len() - 1
            }
        };

        self.send_stack[name_index].1.insert(receiver.clone());
        Ok(())
    }

    /// Remove a receiver from this send controller
    // NOTE(mhroth): once the receiver set has been created, it should not
    // be erased anymore from the send_stack.
    //
    // `pd::Context` depends on the `name_index` to be constant for all
    // receiver names once they are defined, as a message destined for
    // that receiver may already be in the message queue with the given
    // index. If the indicies change, then message will be sent to the
    // wrong receiver set.
    // TODO(mhroth): This is a stupid constraint. Fix it.
    pub fn remove_receiver(&mut self, receiver: RemoteMessageReceiver) {
        if let Some(name_index) = self.get_name_index(receiver.get_name()) {
            self.send_stack[name_index.0].1.erase(receiver);
        }
    }

    /// Register an external message receiver with this send controller
    pub fn register_external_receiver(&mut self, receiver_name: &str) {
        // sets only contain unique items
        self.external_receiver_set.insert(receiver_name.into());
    }

    /// Unregister an external message receiver from this send controller
    pub fn unregister_external_receiver(&mut self, _receiver_name: &str) {
        // TODO(tarcieri): implement this
        panic!("unimplemented");
        // self.external_receiver_set.erase(receiver_name);
    }
}

impl Default for Controller {
    fn default() -> Self {
        Self::new()
    }
}
