/*
 *  Copyright 2009,2010,2012 Reality Jockey, Ltd.
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
#include "DspDelayWrite.h"
#include "PdGraph.h"

message::Object *DspDelayWrite::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspDelayWrite(init_message, graph);
}

DspDelayWrite::DspDelayWrite(pd::Message *init_message, PdGraph *graph) : DspObject(0, 1, 0, 0, graph) {
  if (init_message->is_symbol(0) && init_message->is_float(1)) {
    bufferLength = (int) ceilf(utils::millisecondsToSamples(init_message->get_float(1), 
        graph->get_sample_rate())); 
    if (bufferLength % block_sizeInt != 0) {
      bufferLength = ((bufferLength/block_sizeInt)+2) * block_sizeInt;
    } else {
      bufferLength += block_sizeInt;
    }
    headIndex = 0;
    // buffer[bufferLength] == buffer[0], which makes calculation in vd~ easier
    int numBufferLengthBytes = (bufferLength+1)*sizeof(float);
    dspBufferAtOutlet[0] = ALLOC_ALIGNED_BUFFER(numBufferLengthBytes);
    memset(dspBufferAtOutlet[0], 0, numBufferLengthBytes); // zero the delay buffer
    name = utils::copy_string(init_message->get_symbol(0));
  } else {
    graph->print_err("ERROR: delwrite~ must be initialised as [delwrite~ name delay].");
    headIndex = 0;
    bufferLength = 0;
    name = NULL;
  }
  process_function = &processSignal;
}

DspDelayWrite::~DspDelayWrite() {
  free(name);
  FREE_ALIGNED_BUFFER(dspBufferAtOutlet[0]);
  dspBufferAtOutlet[0] = NULL;
}

void DspDelayWrite::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspDelayWrite *d = reinterpret_cast<DspDelayWrite *>(dspObject);
  
  // copy inlet buffer to delay buffer
  memcpy(d->dspBufferAtOutlet[0] + d->headIndex, d->dspBufferAtInlet[0], toIndex*sizeof(float));
  if (d->headIndex == 0) d->dspBufferAtOutlet[0][d->bufferLength] = d->dspBufferAtOutlet[0][0];
  d->headIndex += toIndex;
  if (d->headIndex >= d->bufferLength) d->headIndex = 0;
}
