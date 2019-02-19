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

//! Pure Data messages
//!
//! ## Translation Notes
//!
//! This file was translated from the following ZenGarden C++ sources:
//!
//! - `PdMessage.cpp`
//! - `PdMessage.h`
//!
//! Original C++ classes/types were renamed/refactored as follows:
//!
//! - The `MessageAtom` class (from `PdMessage.h`) was renamed to `pd::message::Atom`.
//! - `PdMessage` was renamed to `pd::Message`.
//! - All `PdMessage::initWith*` initializers renamed to `Message::from_*`
//! - Several `PdMessage` parse errors changed to `Result` instead of silently
//!   ignoring them (but should we ignore them to match Pd / Zg behavior?)

mod atom;

pub use self::atom::Atom;

use crate::{
    error::{Error, ErrorKind},
    message::{element, Timestamp},
};
use core::{
    fmt::{self, Display, Write},
    ops, str,
};
use heapless::{self, ArrayLength};
use typenum::U1;

/// Pure Data messages
///
/// <https://puredata.info/dev/PdMessages>
#[derive(Clone, Debug)]
pub struct Message<'pd, N: ArrayLength<Atom<'pd>>> {
    /// Message timestamp
    timestamp: Timestamp,

    /// Atoms in the message
    atoms: heapless::Vec<Atom<'pd>, N>,
}

impl<'pd, N> Message<'pd, N>
where
    N: ArrayLength<Atom<'pd>>,
{
    /// Initialize a message with the given timestamp.
    ///
    /// This is the primary constructor for all message objects.
    pub fn from_timestamp(timestamp: Timestamp) -> Self {
        let mut result = Self {
            timestamp,
            atoms: heapless::Vec::new(),
        };

        result.set_bang(0).unwrap(); // default value
        result
    }

    // TODO(tarcieri): this doesn't play well with lifetimes. Just use the two calls below directly
    // Initialize the message with a string, arguments, and a resolution buffer. The string will
    // be resolved into the buffer using the arguments. Any resolved strings in the message will
    // point into the resolution buffer. The buffer is generally intended to be a temporary storage
    // for such strings while objects are created.
    // pub fn from_string_and_args<ArgN, BufN>(
    //    init_string: &str,
    //    arguments: Option<&Message<'pd, ArgN>>,
    //    buffer: &'pd mut heapless::String<BufN>,
    // ) -> Self
    // where
    //    ArgN: ArrayLength<Atom>,
    //    BufN: ArrayLength<u8>,
    // {
    //    // Render arguments into the provided buffer
    //    Self::resolve_string(init_string, arguments, 0, buffer);
    //
    //    // Initialize using the init string after argument expansion
    //    Self::from_string(Timestamp::default(), buffer.as_str())
    // }

    /// Adds elements to the message by tokenizing the given string. If a token is numeric then it is
    /// automatically resolved to a float. Otherwise the string is interpreted as a symbol. Note that
    /// the `init_string` is tokenized and should be provided in a buffer which may be edited.
    pub fn from_string(timestamp: Timestamp, init_string: &'pd str) -> Result<Self, Error> {
        let mut result = Self::from_timestamp(timestamp);

        for (i, token) in init_string.split(" ;").enumerate() {
            if i >= N::to_usize() {
                Err(ErrorKind::BufferOverflow)?;
            }

            if let Ok(value) = token.parse::<f32>() {
                result.set_float(i, value)?;
            } else if token == "!" || token == "bang" || token == "" {
                result.set_bang(i)?;
            } else {
                // TODO(tarcieri): validate well-formed symbols?
                result.set_symbol(i, token)?;
            }
        }

        Ok(result)
    }

    /// Resolve arguments in a string with a given argument list into the given `buffer`.
    ///
    /// The `offset` parameter adjusts the resolution of the `$` argument.
    ///
    /// In the case of message boxes, `$0` refers to the graph id, which is the first argument in a
    /// graph's list of arguments. For all other objects, `$1` refers to the first (0th) element of
    /// the argument. The offset is used to distinguish between these
    /// cases, by offsetting the argument index resolution.
    pub fn resolve_string<ArgN, BufN>(
        init_string: &'pd str,
        arguments_opt: Option<&Message<'pd, ArgN>>,
        offset: usize,
        buffer: &mut heapless::String<BufN>,
    ) -> Result<(), Error>
    where
        ArgN: ArrayLength<Atom<'pd>>,
        BufN: ArrayLength<u8>,
    {
        if let Some(arguments) = arguments_opt {
            let num_arguments = arguments.get_num_elements();
            let mut init_string_args = init_string.split("\\$");

            if buffer.push_str(init_string_args.next().unwrap()).is_err() {
                Err(ErrorKind::BufferOverflow)?;
            }

            for argument in init_string_args {
                let mut argument_chars = argument.chars();

                let argument_index = argument_chars
                    .next()
                    .expect("no argument index given")
                    .to_digit(10)
                    .unwrap_or_else(|| panic!("invalid argument index!"))
                    as usize
                    - offset;

                // NOTE(tarcieri): Pd and Zg both write a 0 here. We error out.
                if argument_index >= num_arguments {
                    Err(ErrorKind::IndexOutOfBounds)?;
                }

                if let Some(atom) = arguments.get_element(argument_index) {
                    match *atom {
                        // TODO(tarcieri): verify this is equivalent to the original C++ (which uses `%g`)
                        Atom::Float(float) => buffer.write_fmt(format_args!("{}", float))?,
                        Atom::Symbol(symbol) => buffer.write_fmt(format_args!("{}", symbol))?,
                        _ => continue,
                    }
                }

                // write the remaining chars to the buffer
                buffer.write_str(argument_chars.as_str())?;
            }
        } else {
            buffer.write_str(init_string)?;
        }

        Ok(())
    }

    /// Converts symbolic elements referring to message element types (e.g. `float` or `f`) to those
    /// types. For instance, if an element has a symbolic value of `float`, then that element is
    /// converted into one of type `FLOAT`.
    pub fn resolve_symbols_to_type(&mut self) {
        for i in 0..self.atoms.len() {
            if self.is_symbol(i) {
                if self.is_symbol_str(i, "symbol") || self.is_symbol_str(i, "s") {
                    // do nothing, but leave the symbol as is
                } else if self.is_symbol_str(i, "anything") || self.is_symbol_str(i, "a") {
                    self.set_anything(i).unwrap();
                } else if self.is_symbol_str(i, "bang") || self.is_symbol_str(i, "b") {
                    self.set_bang(i).unwrap();
                } else if self.is_symbol_str(i, "float") || self.is_symbol_str(i, "f") {
                    self.set_float(i, 0.0).unwrap();
                } else if self.is_symbol_str(i, "list") || self.is_symbol_str(i, "l") {
                    self.set_list(i).unwrap();
                } else {
                    // if the symbol string is unknown, leave is as ANYTHING
                    self.set_anything(i).unwrap();
                }
            }
        }
    }

    /// Get the number of elements in this message
    pub fn get_num_elements(&self) -> usize {
        self.atoms.len()
    }

    /// Get the element at the particular lindex
    pub fn get_element(&self, index: usize) -> Option<&Atom> {
        self.atoms.get(index)
    }

    /// Is the atom at the given index equal to the other atom?
    // TODO(tarcieri): get rid of this and just have callers compare `Atom`s themselves
    pub fn atom_is_equal_to(&self, index: usize, other: &Atom) -> bool {
        self.get_element(index)
            .map(|atom| atom == other)
            .unwrap_or(false)
    }

    /// Set the global timestamp of this message (in milliseconds)
    pub fn set_timestamp(&mut self, timestamp: f64) {
        self.timestamp = timestamp.into();
    }

    /// Get the global timestamp of this message (in milliseconds)
    pub fn get_timestamp(&self) -> Timestamp {
        self.timestamp
    }

    /// Is the given message element a float?
    // TODO(tarcieri): have callers ask this about atoms directly
    pub fn is_float(&self, index: usize) -> bool {
        self.get_element(index)
            .map(|elem| elem.is_float())
            .unwrap_or(false)
    }

    /// Is the given message element a symbol?
    pub fn is_symbol(&self, index: usize) -> bool {
        // TODO(tarcieri): don't panic on out-of-bounds?
        self.get_element(index)
            .map(|elem| elem.is_symbol())
            .unwrap_or(false)
    }

    /// Is the given message element a symbol that matches the given test string?
    // TODO(tarcieri): have callers ask this about atoms directly
    pub fn is_symbol_str(&self, index: usize, test: &str) -> bool {
        self.get_element(index)
            .map(|elem| elem.is_symbol_str(test))
            .unwrap_or(false)
    }

    /// Is the given message element a bang?
    // TODO(tarcieri): have callers ask this about atoms directly
    pub fn is_bang(&self, index: usize) -> bool {
        self.get_element(index)
            .map(|elem| elem.is_bang())
            .unwrap_or(false)
    }

    /// Does this message match the given format string
    pub fn has_format(&self, format: &str) -> bool {
        if format.len() == self.atoms.len() {
            format.chars().enumerate().all(|(i, c)| match c {
                'f' => self.is_float(i),
                's' => self.is_symbol(i),
                'b' => self.is_bang(i),
                _ => false,
            })
        } else {
            false
        }
    }

    /// Get the type for the given message
    pub fn get_type(&self, index: usize) -> element::Type {
        // TODO(tarcieri): should we really return `ANYTHING` here for out-of-range?
        self.atoms
            .get(index)
            .map(|atom| atom.get_type())
            .unwrap_or(element::Type::ANYTHING)
    }

    /// Convenience function to get the float value from a particular message element
    // TODO(tarcieri): have callers get this from atoms directly
    pub fn get_float(&self, index: usize) -> Option<f32> {
        self.atoms.get(index).and_then(|atom| atom.get_float())
    }

    /// Get the symbol value for the given element
    // TODO(tarcieri): have callers ask this about atoms directly
    pub fn get_symbol(&self, index: usize) -> Option<&str> {
        self.get_element(index).and_then(|elem| elem.get_symbol())
    }

    /// Set an element to the given atom value
    ///
    /// Panics if the index exceeds the capacity of the buffer
    pub fn set_element(&mut self, index: usize, value: Atom<'pd>) -> Result<(), Error> {
        if let Some(elem) = self.atoms.get_mut(index) {
            *elem = value
        } else if self.atoms.len() == index {
            if self.atoms.push(value).is_err() {
                Err(ErrorKind::BufferOverflow)?;
            }
        } else {
            // TODO(tarcieri): return a result here
            panic!("set_element({}, ...) out-of-bounds / noncontiguous", index);
        }

        Ok(())
    }

    /// Convenience function to set a message element to the `ANYTHING` atom
    ///
    /// Panics if there isn't enough space in the atoms buffer.
    // TODO(tarcieri): have callers set atoms directly
    pub fn set_anything(&mut self, index: usize) -> Result<(), Error> {
        self.set_element(index, Atom::Anything)
    }

    /// Convenience function to set a message element to the `BANG` atom
    ///
    /// Panics if there isn't enough space in the atoms buffer.
    // TODO(tarcieri): have callers set atoms directly
    pub fn set_bang(&mut self, index: usize) -> Result<(), Error> {
        self.set_element(index, Atom::Bang)
    }

    /// Convenience function to set a message element to a float value.
    ///
    /// Panics if there isn't enough space in the atoms buffer.
    // TODO(tarcieri): have callers set atoms directly
    pub fn set_float(&mut self, index: usize, value: f32) -> Result<(), Error> {
        self.set_element(index, value.into())
    }

    /// Convenience function to set a message element to the `LIST` atom
    ///
    /// Panics if there isn't enough space in the atoms buffer.
    // TODO(tarcieri): have callers set atoms directly
    pub fn set_list(&mut self, index: usize) -> Result<(), Error> {
        self.set_element(index, Atom::List(&[]))
    }

    /// Convenience function to set a message element to a symbol value.
    ///
    /// Panics if there isn't enough space in the atoms buffer.
    // TODO(tarcieri): have callers set atoms directly
    pub fn set_symbol(&mut self, index: usize, symbol: &'pd str) -> Result<(), Error> {
        self.set_element(index, symbol.into())
    }

    /// Legacy function which copies messages to the heap
    // TODO(tarcieri): unimplemented, and we won't always have a heap!
    pub fn clone_on_heap(&self) {
        // TODO(tarcieri): unimplemented!
        panic!("unimplemented");
    }
}

impl<'pd> Message<'pd, U1> {
    /// Initialize a message with a timestamp and `Atom`
    pub fn from_timestamp_and_atom(timestamp: Timestamp, atom: Atom<'pd>) -> Self {
        let mut result = Self::from_timestamp(timestamp);
        result[0] = atom;
        result
    }

    /// Initialize a message with a timestamp and a "bang"
    pub fn from_timestamp_and_bang(timestamp: Timestamp) -> Self {
        Self::from_timestamp(timestamp)
    }

    /// Initialize a message with a timestamp and a floating point value
    // TODO(tarcieri): Unify all `from_timestamp_and_*` into `from_timestamp_and_atom`
    pub fn from_timestamp_and_float(timestamp: Timestamp, value: f32) -> Self {
        Self::from_timestamp_and_atom(timestamp, value.into())
    }

    /// Innitialize a message with a timestamp and the given "symbol"
    // TODO(tarcieri): Unify all `from_timestamp_and_*` into `from_timestamp_and_atom`
    pub fn from_timestamp_and_symbol(timestamp: Timestamp, symbol: &'pd str) -> Self {
        Self::from_timestamp_and_atom(timestamp, symbol.into())
    }
}

impl<'pd, N> Display for Message<'pd, N>
where
    N: ArrayLength<Atom<'pd>>,
{
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for (i, atom) in self.atoms.iter().enumerate() {
            write!(f, "{}", atom)?;

            if i != self.atoms.len() - 1 {
                write!(f, " ")?;
            }
        }

        Ok(())
    }
}

impl<'pd, N> ops::Index<usize> for Message<'pd, N>
where
    N: ArrayLength<Atom<'pd>>,
{
    type Output = Atom<'pd>;

    fn index(&self, index: usize) -> &Atom<'pd> {
        self.atoms.index(index)
    }
}

impl<'pd, N> ops::IndexMut<usize> for Message<'pd, N>
where
    N: ArrayLength<Atom<'pd>>,
{
    fn index_mut(&mut self, index: usize) -> &mut Atom<'pd> {
        self.atoms.index_mut(index)
    }
}

#[cfg(test)]
mod tests {
    use super::{Atom, Message};
    use crate::{error::ErrorKind, message::Timestamp};
    use std::string::ToString;
    use typenum::{U1, U4};

    /// Example timestamp value
    const TIMESTAMP: Timestamp = Timestamp(0.0);

    #[test]
    fn from_timestamp() {
        let message = Message::<U1>::from_timestamp(TIMESTAMP);

        assert_eq!(message.get_timestamp(), TIMESTAMP);
        assert_eq!(message.get_num_elements(), 1);
        assert_eq!(message[0], Atom::Bang);
    }

    #[test]
    fn from_string_containing_float() {
        let message = Message::<U1>::from_string(TIMESTAMP, "3.14").unwrap();

        assert_eq!(message.get_timestamp(), TIMESTAMP);
        assert_eq!(message.get_num_elements(), 1);
        assert_eq!(message[0], Atom::Float(3.14));
    }

    #[test]
    fn from_string_containing_bang() {
        for bang in &["!", "bang"] {
            let message = Message::<U1>::from_string(TIMESTAMP, bang).unwrap();

            assert_eq!(message.get_timestamp(), TIMESTAMP);
            assert_eq!(message.get_num_elements(), 1);
            assert_eq!(message[0], Atom::Bang);
        }
    }

    #[test]
    fn from_string_containing_symbol() {
        for symbol in &["float", "list", "symbol"] {
            let message = Message::<U1>::from_string(TIMESTAMP, symbol).unwrap();

            assert_eq!(message.get_timestamp(), TIMESTAMP);
            assert_eq!(message.get_num_elements(), 1);
            assert_eq!(message[0], Atom::Symbol(symbol));
        }
    }

    #[test]
    fn from_string_empty() {
        let message = Message::<U1>::from_string(TIMESTAMP, "").unwrap();

        assert_eq!(message.get_timestamp(), TIMESTAMP);
        assert_eq!(message.get_num_elements(), 1);

        // TODO(tarcieri): is this really the desired behavior, or should this be an error?
        assert_eq!(message[0], Atom::Bang);
    }

    #[test]
    fn from_string_multi_float() {
        let message = Message::<U4>::from_string(TIMESTAMP, "1 ;2 ;3 ;4").unwrap();

        assert_eq!(message.get_timestamp(), TIMESTAMP);
        assert_eq!(message.get_num_elements(), 4);

        for i in 1..4 {
            assert_eq!(message[i - 1], Atom::Float(i as f32))
        }
    }

    #[test]
    fn from_string_element_overflow() {
        assert_eq!(
            ErrorKind::BufferOverflow,
            Message::<U1>::from_string(TIMESTAMP, "1 ;2 ;3 ;4")
                .err()
                .unwrap()
                .kind()
        );
    }

    #[test]
    fn to_string() {
        let message = Message::<U4>::from_string(TIMESTAMP, "1 ;2 ;3 ;4").unwrap();
        assert_eq!(message.to_string(), "1 2 3 4");
    }
}
