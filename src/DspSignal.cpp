/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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
#include "DspSignal.h"
#include "PdGraph.h"

message::Object *DspSignal::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspSignal(init_message, graph);
}

DspSignal::DspSignal(pd::Message *init_message, PdGraph *graph) : DspObject(1, 0, 0, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
  process_function = &processScalar;
  process_functionNoMessage = &processScalar;
}

DspSignal::~DspSignal() {
  // nothing to do
}

string DspSignal::toString() {
  char str[snprintf(NULL, 0, "%s %g", get_object_label(), constant)+1];
  snprintf(str, sizeof(str), "%s %g", get_object_label(), constant);
  return string(str);
}

void DspSignal::process_message(int inlet_index, pd::Message *message) {
  if (message->is_float(0)) {
    constant = message->get_float(0);
  }
}

void DspSignal::processScalar(DspObject *dspObject, int fromIndex, int toIndex) {
  DspSignal *d = reinterpret_cast<DspSignal *>(dspObject);
  ArrayArithmetic::fill(d->dspBufferAtOutlet[0], d->constant, fromIndex, toIndex);
}
