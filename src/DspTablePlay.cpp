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

#include "DspTablePlay.h"
#include "MessageTable.h"
#include "PdGraph.h"

message::Object *DspTablePlay::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspTablePlay(init_message, graph);
}

DspTablePlay::DspTablePlay(pd::Message *init_message, PdGraph *graph) : DspObject(1, 0, 2, 1, graph) {
  name = init_message->is_symbol(0) ? utils::copy_string(init_message->get_symbol(0)) : NULL;
  table = NULL;
  outgoing_message = NULL;
  currentTableIndex = 0;
  endTableIndex = 0;
}

DspTablePlay::~DspTablePlay() {
  free(name);
}

void DspTablePlay::set_table(MessageTable *aTable) {
  table = aTable;
}

connection::Type DspTablePlay::get_connection_type(int outlet_index) {
  // the right outlet is a message outlet, otherwise this object is considered to output audio
  return (outlet_index == 1) ? MESSAGE : DSP;
}

void DspTablePlay::send_message(int outlet_index, pd::Message *message) {
  message::Object::send_message(outlet_index, message);
  outgoing_message = NULL;
}

void DspTablePlay::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      playTable((int) message->get_float(0),
          message->is_float(1) ? (int) message->get_float(1) : -1,
          message->get_timestamp());
      break;
    }
    case SYMBOL: {
      if (message->is_symbol_str(0, "set") && message->is_symbol(1)) {
        table = graph->get_table(message->get_symbol(1));
      }
      break;
    }
    case BANG: {
      playTable(0, -1, message->get_timestamp());
      break;
    }
    default: {
      break;
    }
  }
}

void DspTablePlay::playTable(int startIndex, int duration, double startTime) {
  if (startIndex >= 0 && duration >= -1) {
    if (outgoing_message != NULL) {
      // if the table is currently playing, i.e. there is an outstanding scheduled message, cancel it
      graph->cancel_message(this, 1, outgoing_message);
      outgoing_message = NULL;
    }
    int bufferLength = 0;
    table->getBuffer(&bufferLength);
    if (startIndex < bufferLength) {
      // sanity check that table should be played from a point before it ends
      currentTableIndex = startIndex;
      endTableIndex = (duration == -1) ? bufferLength : startIndex + duration;
      if (endTableIndex > bufferLength) {
        endTableIndex = bufferLength;
      }
      double durationMs = 1000.0 * ((double) (endTableIndex-startIndex)) / (double) graph->get_sample_rate();
      outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_bang(startTime + durationMs);
      outgoing_message = graph->schedule_message(this, 1, outgoing_message);
    } else {
      currentTableIndex = bufferLength;
    }
  }
}

void DspTablePlay::processDspWithIndex(int fromIndex, int toIndex) {
  if (table != NULL) {
    int bufferLength = 0;
    float *tableBuffer = table->getBuffer(&bufferLength);
    if (bufferLength < endTableIndex) {
      // in case the table length has been reset while tabplay~ is playing the buffer
      endTableIndex = bufferLength;
    }
    int duration = toIndex - fromIndex; // the duration of the output buffer to fill
    // the number of remaining samples to play in the table buffer
    int remainingTableSamples = endTableIndex - currentTableIndex;
    if (remainingTableSamples <= 0) {
      // if the entire buffer has already been read, fill the output with silence
      memset(dspBufferAtOutlet[0], 0, block_sizeInt * sizeof(float));
    } else if (duration <= remainingTableSamples) {
      if (duration == block_sizeInt) {
        // if the entire output must be filled and there are more than one buffer's worth of
        // samples still available from the table, just set the output buffer pointer
        memcpy(dspBufferAtOutlet[0], tableBuffer + currentTableIndex, block_sizeInt*sizeof(float));
        currentTableIndex += block_sizeInt;
      } else {
        // if the number of remaining samples in the table is more than the number of samples
        // which need to be read to the output buffer, but not the whole output buffer must be written
        memcpy(dspBufferAtOutlet[0] + fromIndex, tableBuffer + currentTableIndex, duration * sizeof(float));
        currentTableIndex += duration;
      }
    } else {
      // if the number of output buffer samples to fill is larger than the number of remaining table
      // samples, fill the output with the maximum available table samples, and fill in the remainder
      // with zero
      memcpy(dspBufferAtOutlet[0] + fromIndex, tableBuffer + currentTableIndex,
          remainingTableSamples * sizeof(float));
      memset(dspBufferAtOutlet[0] + fromIndex + remainingTableSamples, 0,
          (duration-remainingTableSamples) * sizeof(float));
      currentTableIndex += duration;
    }
  }
}
