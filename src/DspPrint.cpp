/*
 *  Copyright 2010 Reality Jockey, Ltd.
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

#include "DspPrint.h"
#include "PdGraph.h"

message::Object *DspPrint::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspPrint(init_message, graph);
}

DspPrint::DspPrint(pd::Message *init_message, PdGraph *graph) : DspObject(1, 1, 0, 0, graph) {
  name = utils::copy_string(init_message->is_symbol(0) ? init_message->get_symbol(0) : (char *) "print~");
}

DspPrint::~DspPrint() {
  free(name);
}

void DspPrint::process_message(int inlet_index, pd::Message *message) {
  if (message->is_bang(0)) {
    int bufferMaxIndex = block_sizeInt - 1;
    int totalLength = snprintf(NULL, 0, "%s:\n", name);
    float *input_buffer = dspBufferAtInlet[0];
    for (int i = 0; i < bufferMaxIndex; i++) {
      totalLength += snprintf(NULL, 0, "%g ", input_buffer[i]);
    }
    totalLength += snprintf(NULL, 0, "%g", input_buffer[bufferMaxIndex]);
    
    char buffer[totalLength];
    
    int pos = snprintf(buffer, totalLength, "%s:\n", name);
    for (int i = 0; i < bufferMaxIndex; i++) {
      pos += snprintf(buffer + pos, totalLength, "%g ", input_buffer[i]);
    }
    snprintf(buffer + pos, totalLength, "%g", input_buffer[bufferMaxIndex]);
    
    graph->print_std(buffer);
  }
}

void DspPrint::processDspWithIndex(int fromIndex, int toIndex) {
  // nothing to do
}
