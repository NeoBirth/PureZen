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

#include "MessageChange.h"

message::Object *MessageChange::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageChange(init_message, graph);
}

MessageChange::MessageChange(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
   prevValue = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
}

MessageChange::~MessageChange() {
  // nothing to do
}

void MessageChange::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      // output only if input is different than what is already there
      float messageValue = message->get_float(0);
      if (messageValue != prevValue) {
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), messageValue);
        prevValue = messageValue;
        send_message(0, outgoing_message);
      }
      break;
    }
    case BANG: {
      // force output
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), prevValue);
      send_message(0, outgoing_message);
      break;
    }
    case SYMBOL: {
      if (message->is_symbol_str(0, "set") && message->is_float(1)) {
        prevValue = message->get_float(1);
      }
      break;
    }
    default: {
      break;
    }
  }
}
