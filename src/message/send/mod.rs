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

//! Message `send` (or `s`) command
//!
//! ## Translation Notes
//!
//! This file was translated from the following ZenGarden C++ sources:
//!
//! - `MessageSend.cpp`
//! - `MessageSend.h`
//!
//! Original C++ classes/types were renamed/refactored as follows:
//!
//! - `MessageSend` => `message::Send`

mod controller;

pub use self::controller::Controller;
use crate::{
    message::{
        self,
        object::{self, connection},
    },
    pd,
};
use core::{
    fmt::{self, Display},
    str::FromStr,
};
use heapless::{self, consts::*, ArrayLength};

/// Default name for `message::Send` objects
pub const DEFAULT_NAME: &str = "pz_default_sendreceive_name";

/// Maximum length of the send's name
#[allow(non_camel_case_types)]
type MAX_NAME_LENGTH = U32;

/// Name of this send
type Name = heapless::String<MAX_NAME_LENGTH>;

/// Message `send` (or `s`) command
// TODO(tarcieri): handle previous `MessageObject` superclass behaviors
pub struct Send {
    /// Name of this send
    name: Name,

    /// ID of the graph this send is associated with
    graph_id: pd::graph::Id,
}

impl Send {
    /// Create a new `message::Send`
    pub fn new<'pd, N>(init_message: &pd::Message<'pd, N>, graph_id: pd::graph::Id) -> Self
    where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        // TODO(tarcieri): handle previous object initialization
        // message::Object(init_message->is_symbol(0) ? 1 : 2, 0, graph) {

        let name = if init_message.is_symbol(0) {
            Name::from_str(init_message.get_symbol(0).unwrap())
        } else {
            Name::from_str(DEFAULT_NAME)
        }
        .unwrap();

        Send { name, graph_id }
    }

    /// Create a new message send object
    pub fn new_object<'pd, N>(
        _init_message: &pd::Message<'pd, N>,
        _graph: &pd::Graph,
    ) -> message::Object
    where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        // TODO(tarcieri): figure out how we're replacing object orientation
        //return new message::Send(init_message, graph);
        panic!("unimplemented");
    }

    /// Get the label for this object
    pub fn get_object_label(&self) -> &str {
        "send"
    }

    /// Get the type of this object
    pub fn get_object_type() -> object::Type {
        object::Type::MESSAGE_SEND
    }

    /// Receive a message to this send
    pub fn receive_message<'pd, N>(
        &mut self,
        context: &mut pd::Context,
        inlet_index: connection::Index,
        message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        match inlet_index {
            connection::Index(0) => context
                .get_graph_mut(self.graph_id)
                .unwrap()
                .send_message_to_named_receivers(self.name.as_str(), message),
            connection::Index(1) => {
                if message.is_symbol(0) {
                    self.name = Name::from_str(message.get_symbol(0).unwrap()).unwrap();
                }
            }
            _ => (), // TODO(tarcieri): handle this case?
        }
    }
}

impl Display for Send {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{} {}", self.get_object_label(), self.name)
    }
}
