/*
 *  Copyright 2010,2011 Reality Jockey, Ltd.
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

#include "BufferPool.h"
#include "DspReceive.h"
#include "PdGraph.h"

message::Object *DspReceive::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspReceive(init_message, graph);
}

DspReceive::DspReceive(pd::Message *init_message, PdGraph *graph) : DspObject(1, 0, 0, 1, graph) {
  if (init_message->is_symbol(0)) {
    name = utils::copy_string(init_message->get_symbol(0));
    dspBufferAtOutlet[0] = ALLOC_ALIGNED_BUFFER(graph->get_block_size() * sizeof(float));
  } else {
    name = NULL;
    graph->print_err("receive~ not initialised with a name.");
  }
  process_function = &processSignal;

  // this pointer contains the send buffer
  // default to zero buffer
  dspBufferAtInlet[0] = graph->get_buffer_pool()->get_zero_buffer();
}

DspReceive::~DspReceive() {
  free(name);
  FREE_ALIGNED_BUFFER(dspBufferAtOutlet[0]);
}

void DspReceive::process_message(int inlet_index, pd::Message *message) {
  if (message->has_format("ss") && message->is_symbol_str(0, "set")) {
    graph->print_err("[receive~ %s]: message \"set %s\" is not supported.", name, message->get_symbol(1));
  }
}

void DspReceive::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspReceive *d = reinterpret_cast<DspReceive *>(dspObject);
  memcpy(d->dspBufferAtOutlet[0], d->dspBufferAtInlet[0], toIndex*sizeof(float));
}
