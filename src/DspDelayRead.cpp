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
#include "DspDelayRead.h"
#include "DspDelayWrite.h"
#include "PdGraph.h"

message::Object *DspDelayRead::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspDelayRead(init_message, graph);
}

DspDelayRead::DspDelayRead(pd::Message *init_message, PdGraph *graph) : DelayReceiver(1, 0, 0, 1, graph) {
  if (init_message->is_symbol(0) && init_message->is_float(1)) {
    name = utils::copy_string(init_message->get_symbol(0));
    delayInSamples = utils::millisecondsToSamples(init_message->get_float(1), graph->get_sample_rate());
  } else {
    graph->print_err("delread~ must be initialised in the format [delread~ name delay].");
    delayInSamples = 0.0f;
  }
  process_function = &processSignal;
  
  // TODO(mhroth): implement process function for case of receiving messages
//  process_functionNoMessage = &processScalar;
}

DspDelayRead::~DspDelayRead() {
  // nothing to do
}

void DspDelayRead::onInletConnectionUpdate(unsigned int inlet_index) {
//  process_function = (incoming_connections[0].size() > 0) ? &processScalar : &processSignal;
}

void DspDelayRead::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 0 && message->is_float(0)) {
    // update the delay time
    delayInSamples = utils::millisecondsToSamples(message->get_float(0), graph->get_sample_rate());
  }
}

void DspDelayRead::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspDelayRead *d = reinterpret_cast<DspDelayRead *>(dspObject);
  
  int headIndex = 0;
  int bufferLength = 0;
  float *buffer = d->delayline->getBuffer(&headIndex, &bufferLength);
  
  // this handles the most common case. Messages are rarely sent to delread~.
  int delayIndex = headIndex - toIndex - ((int) d->delayInSamples);
  if (delayIndex < 0) {
    delayIndex += bufferLength;
  }
  if (delayIndex > bufferLength - toIndex) {
    int samplesInBuffer = bufferLength - delayIndex; // samples remaining in the buffer that belong in this block
    memcpy(d->dspBufferAtOutlet[0], buffer + delayIndex, samplesInBuffer * sizeof(float));
    memcpy(d->dspBufferAtOutlet[0] + samplesInBuffer, buffer, (toIndex - samplesInBuffer) * sizeof(float));
  } else {
    memcpy(d->dspBufferAtOutlet[0], buffer + delayIndex, toIndex*sizeof(float));
  }
}
