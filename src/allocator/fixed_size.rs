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

//! Memory allocator which operates off a fixed-sized buffer
//!
//! ## Notes
//!
//! Inspired by the Entity-Component-System (ECS) patterns described in
//! "My RustConf 2018 Closing Keynote":
//!
//! <https://kyren.github.io/2018/09/14/rustconf-talk.html>
//!
//! TODO(tarcieri): switch to generational indexes? See above link.

use super::{Allocated, Allocator};
use core::ops;
use generic_array::{ArrayLength, GenericArray};

/// Allocator with a fixed-size buffer for use in `no_std` environments
#[derive(Debug)]
pub struct FixedSizeAllocator<T: Allocated, N: ArrayLength<Option<T>>> {
    /// Label for identifying this allocator
    label: &'static str,

    /// Internal buffer
    buffer: GenericArray<Option<T>, N>,
}

impl<T, N> FixedSizeAllocator<T, N>
where
    T: Allocated,
    N: ArrayLength<Option<T>>,
{
    /// Create a new state object, initialized to the default state
    pub fn new(label: &'static str) -> Self {
        Self {
            label,
            buffer: GenericArray::default(),
        }
    }
}

impl<T, N> Allocator<T> for FixedSizeAllocator<T, N>
where
    T: Allocated,
    N: ArrayLength<Option<T>>,
{
    /// Allocate a slot in the buffer and call the given function to get a value
    ///
    /// Panics if there is no space available in the buffer.
    fn allocate<F>(&mut self, f: F) -> &mut T
    where
        F: Fn(T::Id) -> T,
    {
        let mut slot_id: Option<usize> = None;

        // find a free slot ID
        for (i, slot) in self.buffer.iter().enumerate() {
            if slot.is_none() {
                slot_id = Some(i);
            }
        }

        // instantiate the value with the given ID
        if let Some(id) = slot_id {
            let value = f(id.into());
            debug_assert_eq!(
                id,
                value.id().into(),
                "{} ID mismatch with allocated slot",
                self.label
            );
            self.buffer[id] = Some(value);
            self.buffer[id].as_mut().unwrap()
        } else {
            panic!(
                "{} allocator exhausted ({} slots)",
                self.label,
                N::to_usize()
            );
        }
    }

    /// Get value located at the given slot ID.
    fn get(&self, id: T::Id) -> Option<&T> {
        assert!(
            id.into() < N::to_usize(),
            "{} index out of range (max: {}): {:?}",
            self.label,
            N::to_usize(),
            id
        );

        self.buffer.get(id.into()).and_then(|value| value.as_ref())
    }

    /// Get a mutable reference to an value located at the given slot ID.
    fn get_mut(&mut self, id: T::Id) -> Option<&mut T> {
        assert!(
            id.into() < N::to_usize(),
            "{} index out of range (max: {}): {:?}",
            self.label,
            N::to_usize(),
            id
        );
        self.buffer
            .get_mut(id.into())
            .and_then(|value| value.as_mut())
    }
}

impl<T, N> ops::Index<T::Id> for FixedSizeAllocator<T, N>
where
    T: Allocated,
    N: ArrayLength<Option<T>>,
{
    type Output = T;

    fn index(&self, index: T::Id) -> &T {
        // TODO: better error handling?
        self.buffer.index(index.into()).as_ref().unwrap()
    }
}

impl<T, N> ops::IndexMut<T::Id> for FixedSizeAllocator<T, N>
where
    T: Allocated,
    N: ArrayLength<Option<T>>,
{
    fn index_mut(&mut self, index: T::Id) -> &mut T {
        // TODO: better error handling?
        self.buffer.index_mut(index.into()).as_mut().unwrap()
    }
}
