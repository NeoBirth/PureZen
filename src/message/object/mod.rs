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

//! Message objects: core data type for Pure Data graphs
//!
//! ## Translation Notes
//!
//! This file was translated from the following ZenGarden C++ sources:
//!
//! - `MessageObject.cpp`
//!
//! The `MessageObject` class was renamed to `message::Object`.
//!
//! Where ZenGarden relied heavily on inheritance, PureZen is attempting to
//! refactor such usages into composition, following the
//! Entity-Component-System pattern.

pub mod connection;
mod coordinates;
mod types;

pub use self::{connection::Connection, coordinates::Coordinates, types::Type};

use crate::{
    allocator::{self, Allocated, Allocator},
    error::Error,
    pd,
};
use heapless::{self, ArrayLength};

/// Object identifiers (for use with `pd::State`)
#[derive(Copy, Clone, Debug, Eq, PartialEq, PartialOrd, Ord)]
pub struct Id(pub usize);

impl Id {
    /// Get a reference to the object corresponding to this `object::Id` from
    /// the given object allocator.
    pub fn get<A>(self, allocator: &A) -> &Object
    where
        A: Allocator<Object>,
    {
        allocator.get(self).unwrap_or_else(|| {
            panic!("use-after-free error fetching object: {:?}", self);
        })
    }

    /// Get a mutable reference to the object corresponding to this
    /// `object::Id` from the given object allocator.
    pub fn get_mut<A>(self, allocator: &mut A) -> &mut Object
    where
        A: Allocator<Object>,
    {
        allocator.get_mut(self).unwrap_or_else(|| {
            panic!("use-after-free error fetching object: {:?}", self);
        })
    }
}

impl From<usize> for Id {
    fn from(n: usize) -> Id {
        Id(n)
    }
}

impl From<Id> for usize {
    fn from(id: Id) -> usize {
        id.0
    }
}

impl allocator::Index for Id {}

/// Message objects
#[derive(Debug)]
pub struct Object {
    /// Identifier for this object
    id: Id,

    /// Coordinate position of an object on a graphical canvas
    coordinates: Coordinates,

    /// Incoming connections from other message objects
    incoming_connections: connection::Table,

    /// Outgoing connections to other message objects
    outgoing_connections: connection::Table,

    /// Flag indicating that this object has already been considered when ordering the process tree
    is_ordered: bool,
    // A pointer to the graph owning this object.
    // TODO(tarcieri): figure out if we need this, and possibly use a hypothetical `pd::graph::Id`
    // graph: &'_ pd::Graph,
}

// TODO(tarcieri): Parts of the below need to be extracted into a trait
// It was originally (as you might guess from the name) designed for OOP
// and implemented as a base class
impl Object {
    /// Create a new message object
    // TODO: find a way to reference the graph this object is connected to?
    pub fn new<A>(allocator: &mut A) -> &mut Self
    where
        A: Allocator<Self>,
    {
        allocator.allocate(|id| Self {
            id,
            coordinates: Coordinates::default(),
            incoming_connections: connection::Table::new(),
            outgoing_connections: connection::Table::new(),
            is_ordered: false,
        })
    }

    /// Returns the connection type of the given outlet
    pub fn get_connection_type(&self, _outlet_index: connection::Index) -> connection::Type {
        connection::Type::MESSAGE
    }

    /// Returns `true` if this object should distribute the elements of the
    /// incoming message across the inlets.
    ///
    /// A message is otherwise only distributed if the message arrivs on the
    /// left-most inlet and has more than one inlet. This function returns `true`
    /// by default and should be overridden to return `false` if this behaviour
    /// is not desired (e.g. as in the element::Type::of the `line` object).
    ///
    /// This behaviour is set to `false` for all `DspObject` objects.
    pub fn should_distribute_message_to_inlets(&self) -> bool {
        true
    }

    /// The generic entrypoint of a message into an object.
    ///
    /// This function usually either passes the message directly to `process_message` in the
    /// `element::Type` of an object which only processes messages, or queues the message for
    /// later processing.
    pub fn receive_message<'pd, N>(
        &self,
        inlet_index: connection::Index,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        let num_message_inlets = self.incoming_connections.len();

        if inlet_index == connection::Index(0)
            && num_message_inlets > 1
            && message.get_num_elements() > 1
            && self.should_distribute_message_to_inlets()
        {
            // if the message should be distributed across the inlets
            let max_inlet_to_distribute = if message.get_num_elements() < num_message_inlets {
                message.get_num_elements()
            } else {
                num_message_inlets
            };

            // send to right-most inlet first
            for i in (max_inlet_to_distribute..0).rev() {
                match message.get_element(i) {
                    Some(pd::message::Atom::Float(n)) => self.process_message(
                        connection::Index(i),
                        &pd::Message::from_timestamp_and_float(message.get_timestamp(), *n),
                    ),
                    Some(pd::message::Atom::Symbol(s)) => self.process_message(
                        connection::Index(i),
                        &pd::Message::from_timestamp_and_symbol(message.get_timestamp(), s),
                    ),
                    _ => (), // TODO(tarcieri): is this an error case we should panic on or handle?
                }
            }
        } else {
            // otherwise just send the message through normally
            self.process_message(inlet_index, message);
        }
    }

    /// Sends the given message to all connected objects at the given outlet index.
    ///
    /// This function can be overridden in order to take some other action, such as additionally
    /// scheduling a new message as in the case of `MessageMetro`.
    // TODO(tarcieri): probably can't hold `&mut pd::Context` and `&self` (borrowed from context)
    // To resolve this, usages of `send_message` need to be converted from OOP patterns to ECS
    // which act on `object::Id` rather than references. For example, we can get the iterator for
    // all objects we want to send a message to, clone it, and then stop borrowing the source
    pub fn send_message<'pd, N>(
        &self,
        context: &mut pd::Context<'pd>,
        outlet_index: connection::Index,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        self.outgoing_connections
            .send_message(context, outlet_index, message)
    }

    /// The message logic of an object
    pub fn process_message<'pd, N>(
        &self,
        _inlet_index: connection::Index,
        _message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        // By default there is nothing to process.
    }

    /// Establish a connection from another object to this object
    pub fn add_connection_from_object_to_inlet(
        &mut self,
        message_obj_id: Id,
        outlet_index: connection::Index,
        inlet_index: connection::Index,
    ) -> Result<(), Error> {
        // TODO(tarcieri): ensure the connection to this object must be `connection::Type::MESSAGE`
        // if message_obj.get_connection_type(outlet_index) == connection::Type::MESSAGE { ...

        self.incoming_connections[inlet_index]
            .push_back(Connection::new(message_obj_id, outlet_index))
    }

    /// Establish a connection to another object from this object
    pub fn add_connection_to_object_from_outlet(
        &mut self,
        message_obj_id: Id,
        inlet_index: connection::Index,
        outlet_index: connection::Index,
    ) -> Result<(), Error> {
        // TODO(tarcieri): ensure the connection to this object must be `connection::Type::MESSAGE`
        //debug_assert_eq!(
        //    message_obj.get_connection_type(outlet_index),
        //    connection::Type::MESSAGE
        //);

        self.outgoing_connections[outlet_index]
            .push_back(Connection::new(message_obj_id, inlet_index))
    }

    /// Remove a connection from another object to this object.
    ///
    /// This function does not remove the connection reference at the connecting object.
    /// It must be removed separately.
    pub fn remove_connection_from_object_to_inlet(
        &mut self,
        message_obj_id: Id,
        outlet_index: connection::Index,
        inlet_index: connection::Index,
    ) -> Result<(), Error> {
        // TODO(tarcieri): ensure the connection to this object must be `connection::Type::MESSAGE`
        //debug_assert_eq!(
        //    message_obj.get_connection_type(outlet_index),
        //    connection::Type::MESSAGE
        //);

        // NOTE(zg): does this work? (tarcieri: I hope so!)
        self.incoming_connections[inlet_index].remove(Connection::new(message_obj_id, outlet_index))
    }

    /// Remove a connection to another object from this object.
    ///
    /// This function does not remove the connection reference at the connecting object.
    /// It must be removed separately.
    pub fn remove_connection_to_object_from_outlet(
        &mut self,
        message_obj: Id,
        inlet_index: connection::Index,
        outlet_index: connection::Index,
    ) -> Result<(), Error> {
        self.outgoing_connections[outlet_index].remove(Connection::new(message_obj, inlet_index))
    }

    /// Get incoming connections
    pub fn get_incoming_connections(&self, inlet_index: connection::Index) -> &connection::List {
        &self.incoming_connections[inlet_index]
    }

    /// Get outgoing connections
    pub fn get_outgoing_connections(&self, outlet_index: connection::Index) -> &connection::List {
        &self.outgoing_connections[outlet_index]
    }

    /// Get the number of message inlets
    pub fn get_num_inlets(&self) -> usize {
        self.incoming_connections.len()
    }

    /// Get the number of message outlets
    pub fn get_num_outlets(&self) -> usize {
        self.outgoing_connections.len()
    }

    /// Get the type of this object
    pub fn get_object_type() -> Type {
        Type::UNKNOWN
    }

    // TODO: return a `pd::graph::ID` here?
    // pub fn get_graph(&self) -> &pd::Graph {
    //    &self.graph
    // }

    /// Get the position of an object on a graphical canvas.
    pub fn get_coordinates(&self) -> Coordinates {
        self.coordinates
    }

    /// Set the position of an object on a graphical canvas.
    pub fn set_coordinates(&mut self, coordinates: Coordinates) {
        self.coordinates = coordinates
    }

    /// Returns `true` if this object is a leaf in the Pd tree. `false` otherwise.
    ///
    /// This function is used only while computing the process order of objects. For this reason it also
    /// returns true in the cases when the object is send, send~, or throw~.
    pub fn is_leaf_node(&self) -> bool {
        self.outgoing_connections.iter().all(|list| list.is_empty())
    }

    // TODO(tarcieri): commented out until we have DspObject support
    // Returns an ordered list of all parent objects of this object.
    // TODO(tarcieri): guessed a max size. is it bogus? Perhaps we need heap-allocated arrays
    // pub fn get_process_order(&mut self) -> heapless::Vec<U32, DspObject> {
    //     let mut process_list = heapless::Vec::new();
    //
    //     if !self.is_ordered {
    //         self.is_ordered = true;
    //
    //         // NOTE(zg): this object is not added to the process list as `message::Object`
    //         // types do not process audio
    //         for i in 0..self.incoming_connections.len() {
    //             for obj_let_pair in self.incoming_connections[i..] {
    //                  process_list.extend_from_slice(obj_let_pair.object().get_process_order());
    //             }
    //         }
    //     }
    //
    //     process_list
    // }

    /// Reset the `is_ordered` flag to `false`. This is necessary in order to
    /// recompute the process order.
    pub fn reset_ordered_flag(&mut self) {
        self.is_ordered = false;
    }

    /// Analogue of `update_outgoing_message_connection`, but for incoming messages
    pub fn update_incoming_message_connection(
        &self,
        _message_obj_id: Id,
        _old_outlet_index: connection::Index,
        _inlet_index: connection::Index,
        _new_outlet_index: connection::Index,
    ) {
        // TODO(tarcieri): this C++ code was commented out in the original source. Is it necessary?
        // vector<Connection>self.incoming_connections_list =self.incoming_connections[inlet_index];
        // for (int i = 0; i <self.incoming_connections_list.size(); i++) {
        //   Connection *obj_let_pair = (Connection *)self.incoming_connections_list->get(i);
        //   if (obj_let_pair->object == message_obj &&
        //       obj_let_pair->index == old_outlet_index) {
        //     obj_let_pair->index = new_outlet_index;
        //     return;
        //   }
        // }
    }

    /// The destination inlet of an outgoing message connection can change if an inlet object
    /// in a graph is moved (and the inlet ordering changes).
    ///
    /// The connection index change has no effect on the graph ordering and thus it is not necessary
    /// to remove and readd a connection.
    ///
    /// However, the connection must be updated such that message will still be addressed to the
    /// correct inlet.
    pub fn update_outgoing_message_connection(
        &self,
        _message_obj_id: Id,
        _old_inlet_index: connection::Index,
        _outlet_index: connection::Index,
        _new_inlet_index: connection::Index,
    ) {
        // TODO(tarcieri): this C++ code was commented out in the original source. Is it necessary?
        // List *outgoing_connections_list = (List *) self.outgoing_connections[outlet_index];
        // int numConnections = self.outgoing_connections_list->size();
        // for (int i = 0; i < numConnections; i++) {
        //   Connection *obj_let_pair = (Connection *) self.outgoing_connections_list->get(i);
        //   if (obj_let_pair->object == message_obj &&
        //       obj_let_pair->index == old_inlet_index) {
        //     obj_let_pair->index = new_inlet_index;
        //     return;
        //   }
        // }
    }
}

/// Trait implemented by all types that can be used by this allocator
impl Allocated for Object {
    type Id = Id;

    fn id(&self) -> Id {
        self.id
    }
}
