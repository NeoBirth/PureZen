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

use core::fmt;

/// Error type
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub struct Error {
    /// Kinds of errors
    kind: ErrorKind,
}

impl Error {
    /// Obtain the ErrorKind for this Error
    pub fn kind(self) -> ErrorKind {
        self.kind
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.kind)
    }
}

/// Kinds of errors
#[derive(Copy, Clone, Debug, Eq, Fail, PartialEq)]
pub enum ErrorKind {
    /// Exceeded the capacity of an internal buffer
    #[fail(display = "buffer overflow")]
    BufferOverflow,

    /// Index out-of-bounds
    #[fail(display = "index out of bounds")]
    IndexOutOfBounds,

    /// Error parsing a Pure Data program
    #[fail(display = "parse error")]
    ParseError,
}

impl From<ErrorKind> for Error {
    fn from(kind: ErrorKind) -> Error {
        Error { kind }
    }
}

impl From<fmt::Error> for Error {
    fn from(_error: fmt::Error) -> Error {
        // TODO(tarcieri): is this always the case?
        ErrorKind::BufferOverflow.into()
    }
}
