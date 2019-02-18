/*
 *  Copyright 2009,2010,2011,2012 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 *
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ArrayArithmetic.h"
#include "DspDivide.h"
#include "PdGraph.h"

message::Object *DspDivide::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspDivide(init_message, graph);
}

DspDivide::DspDivide(pd::Message *init_message, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
  processFunction = &processScalar;
  processFunctionNoMessage = &processScalar;
}

DspDivide::~DspDivide() {
  // nothing to do
}

void DspDivide::onInletConnectionUpdate(unsigned int inlet_index) {
  processFunction = (incomingDspConnections[0].size() > 0 && incomingDspConnections[1].size() > 0)
      ? &processSignal : &processScalar;
}

string DspDivide::toString() {
  const char *fmt = (constant == 0.0f) ? "%s" : "%s %g";
  char str[snprintf(NULL, 0, fmt, getObjectLabel(), constant)+1];
  snprintf(str, sizeof(str), fmt, getObjectLabel(), constant);
  return string(str);
}

void DspDivide::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 1) {
    if (message->is_float(0)) {
      if (message->get_float(0) == 0.0f) {
        constant = 1.0f;
        graph->printErr("%s received message which set divisor to zero. Divisor set to 1.0f.", getObjectLabel());
      } else {
        constant = message->get_float(0);
      }
    }
  }
}

void DspDivide::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspDivide *d = reinterpret_cast<DspDivide *>(dspObject);
  ArrayArithmetic::divide(d->dspBufferAtInlet[0], d->dspBufferAtInlet[1],
      d->dspBufferAtOutlet[0], 0, toIndex);
}

void DspDivide::processScalar(DspObject *dspObject, int fromIndex, int toIndex) {
  DspDivide *d = reinterpret_cast<DspDivide *>(dspObject);
  ArrayArithmetic::divide(d->dspBufferAtInlet[0], d->constant, d->dspBufferAtOutlet[0], fromIndex, toIndex);
}
