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

#include "MessageSwap.h"

message::Object *MessageSwap::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSwap(init_message, graph);
}

MessageSwap::MessageSwap(pd::Message *init_message, PdGraph *graph) : message::Object(2, 2, graph) {
  left = 0.0f;
  right = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
}

MessageSwap::~MessageSwap() {
  // nothing to do
}

void MessageSwap::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          left = message->get_float(0);
          // allow fallthrough
        }
        case BANG: {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_float(message->get_timestamp(), left);
          send_message(1, outgoing_message); // send a message from outlet 1
          
          outgoing_message->from_timestamp_and_float(message->get_timestamp(), right);
          send_message(0, outgoing_message); // send a message from outlet 0
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
        right = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
