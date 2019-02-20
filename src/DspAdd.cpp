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
#include "DspAdd.h"

class PdGraph;

message::Object *DspAdd::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspAdd(init_message, graph);
}

DspAdd::DspAdd(pd::Message *init_message, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
  process_functionNoMessage = &processScalar;
}

DspAdd::~DspAdd() {
  // nothing to do
}

void DspAdd::onInletConnectionUpdate(unsigned int inlet_index) {
  process_function = (incomingDspConnections[0].size() > 0 && incomingDspConnections[1].size() > 0)
      ? &processSignal : &processScalar;
}

std::string DspAdd::toString() {
  const char *fmt = (constant == 0.0f) ? "%s" : "%s %g";
  char str[snprintf(NULL, 0, fmt, get_object_label(), constant)+1];
  snprintf(str, sizeof(str), fmt, get_object_label(), constant);
  return str;
}

void DspAdd::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 1 && message->is_float(0)) {
    constant = message->get_float(0);
  }
}

void DspAdd::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspAdd *d = reinterpret_cast<DspAdd *>(dspObject);
  ArrayArithmetic::add(d->dspBufferAtInlet[0] , d->dspBufferAtInlet[1],
      d->dspBufferAtOutlet[0], 0, toIndex);
}

void DspAdd::processScalar(DspObject *dspObject, int fromIndex, int toIndex) {
  DspAdd *d = reinterpret_cast<DspAdd *>(dspObject);
  ArrayArithmetic::add(d->dspBufferAtInlet[0] , d->constant,
      d->dspBufferAtOutlet[0], fromIndex, fromIndex);
}
