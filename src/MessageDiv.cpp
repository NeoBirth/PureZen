/*
 *  Copyright 2012 Reality Jockey, Ltd.
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

#include "MessageDiv.h"

message::Object *MessageDiv::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageDiv(init_message, graph);
}

MessageDiv::MessageDiv(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 1.0f;
  if (constant == 0.0f) constant = 1.0f;
  else if (constant < 0.0f) constant = -constant;
}

MessageDiv::~MessageDiv() {
  // nothing to do
}

void MessageDiv::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_float(0)) {
        float f = message->get_float(0);
        if (f < 0.0f) f -= (constant-1.0f);
        float result = truncf(f/constant);
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), result);
        send_message(0, outgoing_message);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        constant = message->get_float(0);
        if (constant == 0.0f) constant = 1.0f;
        else if (constant < 0.0f) constant = -constant;
      }
      break;
    }
    default: break;
  }
}
