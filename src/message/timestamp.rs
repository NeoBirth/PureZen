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

use core::{ops::Add, time::Duration};

/// Message timestamps
#[derive(Copy, Clone, Debug, Default, PartialEq, PartialOrd)]
pub struct Timestamp(pub(crate) f64);

impl Add<Duration> for Timestamp {
    type Output = Timestamp;

    fn add(self, other: Duration) -> Timestamp {
        Timestamp(self.0 + (other.as_secs() as f64 * 1000.0) + f64::from(other.subsec_millis()))
    }
}

impl From<f64> for Timestamp {
    fn from(ts: f64) -> Timestamp {
        Timestamp(ts)
    }
}

impl From<Timestamp> for f64 {
    fn from(ts: Timestamp) -> f64 {
        ts.0
    }
}
