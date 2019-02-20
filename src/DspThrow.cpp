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

#include "DspThrow.h"
#include "pd::Context.h"
#include "PdGraph.h"

message::Object *DspThrow::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspThrow(init_message, graph);
}

DspThrow::DspThrow(pd::Message *init_message, PdGraph *graph) : DspObject(0, 1, 0, 0, graph) {
  if (init_message->is_symbol(0)) {
    name = utils::copy_string(init_message->get_symbol(0));
    buffer = ALLOC_ALIGNED_BUFFER(graph->get_block_size() * sizeof(float));
  } else {
    name = NULL;
    buffer = NULL;
    graph->print_err("throw~ may not be initialised without a name. \"set\" message not supported.");
  }
  process_function = &processSignal;
}

DspThrow::~DspThrow() {
  FREE_ALIGNED_BUFFER(buffer);
  free(name);
}

void DspThrow::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 0 && message->is_symbol_str(0, "set") && message->is_symbol(1)) {
    graph->print_err("throw~ does not support the \"set\" message.");
  }
}

void DspThrow::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspThrow *d = reinterpret_cast<DspThrow *>(dspObject);
  memcpy(d->buffer, d->dspBufferAtInlet[0], toIndex*sizeof(float));
}

bool DspThrow::is_leaf_node() {
  return graph->getContext()->get_dsp_catch(name) == NULL;
}
