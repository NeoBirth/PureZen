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

#include "MessageTableWrite.h"
#include "PdGraph.h"

message::Object *MessageTableWrite::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageTableWrite(init_message, graph);
}

MessageTableWrite::MessageTableWrite(pd::Message *init_message, PdGraph *graph) : message::Object(2, 0, graph) {
  name = init_message->is_symbol(0) ? utils::copy_string(init_message->get_symbol(0)) : NULL;
  table = NULL;
  index = 0;
}

MessageTableWrite::~MessageTableWrite() {
  free(name);
}

char *MessageTableWrite::get_name() {
  return name;
}

void MessageTableWrite::set_table(MessageTable *aTable) {
  table = aTable;
}

bool MessageTableWrite::should_distribute_message_to_inlets() {
  return false;
}

void MessageTableWrite::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          if (table != NULL) {
            int bufferLength = 0;
            float *buffer = table->getBuffer(&bufferLength);
            if (index >= 0 && index < bufferLength) {
              buffer[index] = message->get_float(0);
            }
          }
          break;
        }
        case SYMBOL: {
          if (message->is_symbol_str(0, "set") && message->is_symbol(1)) {
            free(name);
            name = utils::copy_string(message->get_symbol(1));
            table = graph->get_table(name);
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        index = (int) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
