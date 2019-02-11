//! Use preludes for heap-backed data structures from either `alloc` or `std`

#[cfg(all(feature = "alloc", not(feature = "std")))]
pub use alloc::prelude::*;

#[cfg(feature = "std")]
pub use std::prelude::v1::*;
