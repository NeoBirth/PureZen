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

/// Position of an object on a graphical canvas.
///
/// Coordinates may be positive or negative.
#[derive(Copy, Clone, Debug, Default, PartialEq)]
pub struct Coordinates {
    /// x-coordinate
    pub x: f32,

    /// y-coordinate
    pub y: f32,
}
