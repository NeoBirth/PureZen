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
#include "DspMultiply.h"

class PdGraph;

message::Object *DspMultiply::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspMultiply(init_message, graph);
}

DspMultiply::DspMultiply(pd::Message *init_message, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
  inputConstant = 0.0f;
  process_functionNoMessage = &processScalar;
}

DspMultiply::~DspMultiply() {
  // nothing to do
}

string DspMultiply::toString() {
  const char *fmt = (constant == 0.0f) ? "%s" : "%s %g";
  char str[snprintf(NULL, 0, fmt, get_object_label(), constant)+1];
  snprintf(str, sizeof(str), fmt, get_object_label(), constant);
  return string(str);
}

void DspMultiply::onInletConnectionUpdate(unsigned int inlet_index) {
  if (incomingDspConnections[0].size() > 0 && incomingDspConnections[1].size() > 0) {
    process_function = &processSignal;
  } else {
    // because onInletConnectionUpdate can only be called at block boundaries, it is guaranteed
    // that no messages will be in the message queue.
    process_function = &processScalar;
  }
}

void DspMultiply::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: if (message->is_float(0)) inputConstant = message->get_float(0); break;
    case 1: if (message->is_float(0)) constant = message->get_float(0); break;
    default: break;
  }
}

void DspMultiply::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspMultiply *d = reinterpret_cast<DspMultiply *>(dspObject);
  ArrayArithmetic::multiply(d->dspBufferAtInlet[0] , d->dspBufferAtInlet[1],
      d->dspBufferAtOutlet[0], 0, toIndex);
}

void DspMultiply::processScalar(DspObject *dspObject, int fromIndex, int toIndex) {
  DspMultiply *d = reinterpret_cast<DspMultiply *>(dspObject);
  ArrayArithmetic::multiply(d->dspBufferAtInlet[0] , d->constant,
      d->dspBufferAtOutlet[0], fromIndex, toIndex);
}
