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

#include "MessageTableRead.h"
#include "PdGraph.h"

message::Object *MessageTableRead::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageTableRead(init_message, graph);
}

MessageTableRead::MessageTableRead(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  name = init_message->is_symbol(0) ? utils::copy_string(init_message->get_symbol(0)) : NULL;
  table = NULL; // registration process will set the correct pointer
}

MessageTableRead::~MessageTableRead() {
  free(name);
}

char *MessageTableRead::get_name() {
  return name;
}

void MessageTableRead::set_table(MessageTable *aTable) {
  table = aTable;
}

void MessageTableRead::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      if (table != NULL) {
        int bufferLength = 0;
        float *buffer = table->getBuffer(&bufferLength);
        int index = (int) message->get_float(0);
        if (index >= 0 && index < bufferLength) {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_float(message->get_timestamp(), buffer[index]);
          send_message(0, outgoing_message);
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
}
