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

//! Pure Data (Pd)-related types

mod context;
pub(crate) mod message;

pub use self::{context::Context, message::Message};

use crate::allocator::Allocated;
use heapless::ArrayLength;

/// TODO(tarcieri): Placeholder until the C++ `PdGraph` is translated
#[derive(Debug)]
pub struct Graph {
    id: self::graph::Id,
}

impl Graph {
    /// TODO(tarcieri): actually implement this
    pub fn send_message_to_named_receivers<'pd, N>(
        &mut self,
        _name: &str,
        _message: &Message<'pd, N>,
    ) where
        N: ArrayLength<message::Atom<'pd>>,
    {
        panic!("unimplemented");
    }

    /// TODO(tarcieri): actually implement this
    pub fn process_function(&mut self, _graph: &Self, _v1: usize, _v2: usize) {
        panic!("unimplemented");
    }
}

/// Trait implemented by all types that can be used by this allocator
impl Allocated for Graph {
    type Id = self::graph::Id;

    fn id(&self) -> self::graph::Id {
        self.id
    }
}

/// TODO(tarcieri): Placeholder for graph IDs
pub mod graph {
    use crate::allocator;

    /// TODO(tarcieri): Placeholder for graph IDs
    #[derive(Copy, Clone, Debug, Default, Eq, PartialEq, PartialOrd, Ord)]
    pub struct Id(pub usize);

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
}
