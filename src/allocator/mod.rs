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

//! Memory allocator traits/types suitable for use with both `no_std` and
//! ECS-like design patterns.

mod fixed_size;

pub use self::fixed_size::FixedSizeAllocator;

use core::{fmt::Debug, ops};

/// Indexed memory allocator providing O(1) access to contained values by
/// a `T::Id` type.
pub trait Allocator<T: Allocated>: ops::Index<T::Id> + ops::IndexMut<T::Id> {
    /// Allocate a slot in the buffer and call the given function to get a value
    ///
    /// Panics if there is no space available in the buffer.
    fn allocate<F>(&mut self, f: F) -> &mut T
    where
        F: Fn(T::Id) -> T;

    /// Get an object located at the given slot ID.
    fn get(&self, id: T::Id) -> Option<&T>;

    /// Get a mutable reference to an object located at the given slot ID.
    fn get_mut(&mut self, id: T::Id) -> Option<&mut T>;
}

/// Trait implemented by all types that can be used by this allocator
pub trait Allocated: Debug + Sized {
    /// Identifier type for this allocatable value
    type Id: Index;

    /// Get the `Id` associated with this object
    fn id(&self) -> Self::Id;
}

/// Trait implemented by index types used by this allocator
pub trait Index: Copy + Eq + Debug + From<usize> + Into<usize> {}
