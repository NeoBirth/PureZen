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

#include "MessageRemainder.h"

message::Object *MessageRemainder::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageRemainder(init_message, graph);
}

MessageRemainder::MessageRemainder(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  constant = init_message->is_float(0) ? (int) init_message->get_float(0) : 0;
}

MessageRemainder::~MessageRemainder() {
  // nothing to do
}

void MessageRemainder::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_float(0)) {
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        float remainder = (constant == 0.0f) ? 0.0f : (float) ((int) message->get_float(0) % constant);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), remainder);
        send_message(0, outgoing_message);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        constant = (int) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

std::string MessageRemainder::toString() {
  char str[snprintf(NULL, 0, "% %g", constant)+1];
  snprintf(str, sizeof(str), "% %g", constant);
  return string(str);
}
