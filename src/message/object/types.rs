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

//! Pure Data objects

/// All supported object types
// TODO(tarcieri): use CamelCase names
#[derive(Copy, Clone, Debug, Eq, Hash, PartialEq, PartialOrd, Ord)]
#[allow(non_camel_case_types)]
pub enum Type {
    /// DSP: ADC
    DSP_ADC,

    /// DSP: ADD
    DSP_ADD,

    /// DSP: Bandpass filter
    DSP_BANDPASS_FILTER,

    /// DSP: Catch
    DSP_CATCH,

    /// DSP: Clip
    DSP_CLIP,

    /// DSP: Cosine
    DSP_COSINE,

    /// DSP: DAC
    DSP_DAC,

    /// DSP: Table play
    DSP_TABLE_PLAY,

    /// DSP: Delay read
    DSP_DELAY_READ,

    /// DSP: Delay write
    DSP_DELAY_WRITE,

    /// DSP: Inlet
    DSP_INLET,

    /// DSP: Outlet
    DSP_OUTLET,

    /// DSP: Receive
    DSP_RECEIVE,

    /// DSP: Send
    DSP_SEND,

    /// DSP: Table read
    DSP_TABLE_READ,

    /// DSP: Table read 4
    DSP_TABLE_READ4,

    /// DSP: Throw
    DSP_THROW,

    /// DSP: Variable delay
    DSP_VARIABLE_DELAY,

    /// Message inlet
    MESSAGE_INLET,

    /// Message note in
    MESSAGE_NOTEIN,

    /// Message outlet
    MESSAGE_OUTLET,

    /// Message receive
    MESSAGE_RECEIVE,

    /// Message send
    MESSAGE_SEND,

    /// Message table
    MESSAGE_TABLE,

    /// Message table read
    MESSAGE_TABLE_READ,

    /// Message table write
    MESSAGE_TABLE_WRITE,

    /// Pure Data objects
    PURE_DATA,

    /// Unknown or unimportant
    UNKNOWN,
}
