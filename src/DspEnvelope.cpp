/*
 *  Copyright 2009,2010,2011 Reality Jockey, Ltd.
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
#include "DspEnvelope.h"
#include "PdGraph.h"

/** By default, the analysis window size is 1024 samples. */
#define DEFAULT_WINDOW_SIZE 1024

message::Object *DspEnvelope::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspEnvelope(init_message, graph);
}

DspEnvelope::DspEnvelope(pd::Message *init_message, PdGraph *graph) : DspObject(0, 1, 1, 0, graph) {
  if (init_message->is_float(0)) {
    if (init_message->is_float(1)) {
      // if two parameters are provided, set the window size and window interval
      windowSize = init_message->get_float(0);
      windowInterval = init_message->get_float(1);
    } else {
      // if one parameter is provided, set the window size
      windowSize = (int) init_message->get_float(0);
      setWindowInterval(windowSize/2);
    }
  } else {
    // otherwise, use default values for the window size and interval
    windowSize = DEFAULT_WINDOW_SIZE;
    windowInterval = windowSize / 2;
  }
  
  // NOTE(mhroth): I haven't thought very much if this fix could be better done. The issue is that
  // if the blocksize is large (e.g., larger than the windowInterval), then env~ will never send
  // a message. With more logic, a block size larger than the window size could be accomodated. But
  // I am too lazy to consider this option at the moment. Thus, currently the window size and interval
  // must be at least as large as the block size.
  if (windowSize < graph->get_block_size()) {
    graph->print_err("env~ window size must be at least as large as the block size. %i reset to %i.",
        windowSize, graph->get_block_size());
    windowSize = graph->get_block_size();
  }
  if (windowInterval < graph->get_block_size()) {
    graph->print_err("env~ window interval must be at least as large as the block size. %i reset to %i.",
        windowInterval, graph->get_block_size());
    windowInterval = graph->get_block_size();
  }
  
  process_function = &processSignal;
  
  initBuffers();
}

DspEnvelope::~DspEnvelope() {
  free(signalBuffer);
  free(hanningCoefficients);
}

string DspEnvelope::toString() {
  char str[snprintf(NULL, 0, "%s %i %i", get_object_label(), windowSize, windowInterval)+1];
  snprintf(str, sizeof(str), "%s %i %i", get_object_label(), windowSize, windowInterval);
  return string(str);
}

void DspEnvelope::setWindowInterval(int newInterval) {
  int i = newInterval % graph->get_block_size();
  if (i == 0) {
    // windowInterval is a multiple of block_size. Awesome :)
    this->windowInterval = newInterval;
  } else if (i <= graph->get_block_size()/2) {
    // windowInterval is closer to the smaller multiple of block_size
    this->windowInterval = (newInterval/graph->get_block_size())*graph->get_block_size();
  } else {
    // windowInterval is closer to the larger multiple of block_size
    this->windowInterval = ((newInterval/graph->get_block_size())+1)*graph->get_block_size();
  }
}

void DspEnvelope::initBuffers() {
  // ensure that the buffer is big enough to take the number of whole blocks needed to fill it
  numSamplesReceived = 0;
  numSamplesReceivedSinceLastInterval = 0;
  int numBlocksPerWindow = (windowSize % graph->get_block_size() == 0) ? (windowSize/graph->get_block_size()) : (windowSize/graph->get_block_size()) + 1;
  int bufferSize = numBlocksPerWindow * graph->get_block_size();
  signalBuffer = (float *) malloc(bufferSize * sizeof(float));
  hanningCoefficients = (float *) malloc(bufferSize * sizeof(float));
  float N_1 = (float) (windowSize - 1); // (N == windowSize) - 1
  float hanningSum = 0.0f;
  for (int i = 0; i < windowSize; i++) {
    // calcualte the hanning window coefficients
    hanningCoefficients[i] = 0.5f * (1.0f - cosf((2.0f * M_PI * (float) i) / N_1));
    hanningSum += hanningCoefficients[i];
  }
  for (int i = 0; i < windowSize; i++) {
    // normalise the hanning coefficients such that they represent a normalised weighted averaging
    hanningCoefficients[i] /= hanningSum;
  }
}

// windowSize and windowInterval are constrained to be multiples of the block size
void DspEnvelope::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
  DspEnvelope *d = reinterpret_cast<DspEnvelope *>(dspObject);
  
  // copy the input into the signal buffer
  memcpy(d->signalBuffer + d->numSamplesReceived, d->dspBufferAtInlet[0], toIndex*sizeof(float));
  d->numSamplesReceived += toIndex;
  d->numSamplesReceivedSinceLastInterval += toIndex;
  if (d->numSamplesReceived >= d->windowSize) {
    d->numSamplesReceived = 0;
  }
  if (d->numSamplesReceivedSinceLastInterval == d->windowInterval) {
    d->numSamplesReceivedSinceLastInterval -= d->windowInterval;
    // apply hanning window to signal and calculate Root Mean Square
    float rms = 0.0f;
    #if __APPLE__
    float rmsBuffer[d->windowSize];
    vDSP_vsq(d->signalBuffer, 1, rmsBuffer, 1, d->windowSize); // signalBuffer^2 
    vDSP_vmul(rmsBuffer, 1, d->hanningCoefficients, 1, rmsBuffer, 1, d->windowSize); // * hanning window
    vDSP_sve(rmsBuffer, 1, &rms, d->windowSize); // sum the result
    #else
    for (int i = 0; i < d->windowSize; ++i) {
      rms += d->signalBuffer[i] * d->signalBuffer[i] * d->hanningCoefficients[i];
    }
    #endif
    // finish RMS calculation. sqrt is removed as it can be combined with the log operation.
    // result is normalised such that 1 RMS == 100 dB
    rms = 10.0f * log10f(rms) + 100.0f;

    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
    // graph will schedule this at the beginning of the next block because the timestamp will be
    // behind the block start timestamp
    outgoing_message->from_timestamp_and_float(0.0, (rms < 0.0f) ? 0.0f : rms);
    d->graph->schedule_message(d, 0, outgoing_message);
  }
}
