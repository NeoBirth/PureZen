/*
 *  Copyright 2010,2012 Reality Jockey, Ltd.
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

#include "DspSnapshot.h"
#include "PdGraph.h"

message::Object *DspSnapshot::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspSnapshot(init_message, graph);
}

DspSnapshot::DspSnapshot(pd::Message *init_message, PdGraph *graph) : DspObject(1, 1, 1, 0, graph) {
  process_function = &processNull;
  process_functionNoMessage = &processNull;
}

DspSnapshot::~DspSnapshot() {
  // nothing to do
}

connection::Type DspSnapshot::get_connection_type(int outlet_index) {
  return MESSAGE;
}

void DspSnapshot::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case SYMBOL: {
      graph->print_err("[snapshot~] does not support the \"%s\" message.", message->get_symbol(0));
      break;
    }
    case BANG: {
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      double blockIndex = graph->getBlockIndex(message);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), dspBufferAtInlet[0][(int) blockIndex]);
      send_message(0, outgoing_message);
      break;
    }
    default: break;
  }
}

void DspSnapshot::processNull(DspObject *dspObject, int fromIndex, int toIndex) {
  // nothing to do. snapshot~ simply waits to process a message
}
