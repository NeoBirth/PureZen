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
#include "MessageTable.h"
#include "PdGraph.h"

#define DEFAULT_BUFFER_LENGTH 1024

message::Object *MessageTable::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageTable(init_message, graph);
}

MessageTable::MessageTable(pd::Message *init_message, PdGraph *graph) : RemoteMessageReceiver(0, 0, graph) {
  if (init_message->is_symbol(0)) {
    name = utils::copy_string(init_message->get_symbol(0));
    // by default, the buffer length is 1024. The buffer should never be NULL.
    bufferLength = init_message->is_float(1) ? (int) init_message->get_float(1) : DEFAULT_BUFFER_LENGTH;
    buffer = (float *) calloc(bufferLength, sizeof(float));
  } else {
    name = NULL;
    buffer = NULL;
    bufferLength = 0;
    graph->print_err("Object \"table\" must be initialised with a name.");
  }
}

MessageTable::~MessageTable() {
  free(name);
  free(buffer);
}

float *MessageTable::getBuffer(int *bufferLength) {
  *bufferLength = this->bufferLength;
  return buffer;
}

float *MessageTable::resizeBuffer(int newBufferLength) {
  if (newBufferLength > 0) {
    // the new buffer length must be positive
    buffer = (float *) realloc(buffer, newBufferLength * sizeof(float));
    if (newBufferLength > bufferLength) {
      // clear the new portion of the buffer
      memset(buffer+bufferLength, 0, (newBufferLength-bufferLength) * sizeof(float));
    }
    bufferLength = newBufferLength;
    // NOTE(mhroth): this code does not check to see if the returned buffer from realloc
    // is non-NULL. It assumes that realloc is always successful. This is slightly dangerous and
    // could lead to problems.
  }
  return buffer;
}

void MessageTable::process_message(int inlet_index, pd::Message *message) {
  // TODO(mhroth): process all of the commands which can be sent to tables
  if (message->is_symbol_str(0, "read")) {
    if (message->is_symbol_str(1))  {
      // read the file and fill the table
    }
  } else if (message->is_symbol_str(0, "write")) {
    // write the contents of the table to file
  } else if (message->is_symbol_str(0, "normalize")) {
    // normalise the contents of the table to the given value. Default to 1.
    #if __APPLE__
    float sum = 0.0f;
    vDSP_sve(buffer, 1, &sum, bufferLength);
    if (sum != 0.0f) {
      // ensure that element do not change sign during normalisation
      sum = fabsf(sum) / (message->is_float(1) ? message->get_float(1) : 1.0f);
      vDSP_vsdiv(buffer, 1, &sum, buffer, 1, bufferLength);
    }
    #else
    float sum = 0.0f;
    for (int i = 0; i < bufferLength; i++) {
      sum += buffer[i];
    }
    if (sum != 0.0f) {
      sum = fabsf(sum) / (message->is_float(1) ? message->get_float(1) : 1.0f);
      for (int i = 0; i < bufferLength; i++) {
        buffer[i] /= sum;
      }
    }
    #endif
  } else if (message->is_symbol_str(0, "resize")) {
    if (message->is_float(1)) {
      int newBufferLength = (int) message->get_float(1);
      resizeBuffer(newBufferLength);
    }
  }
}
