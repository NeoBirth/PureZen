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

#include "MessageUntil.h"

message::Object *MessageUntil::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageUntil(init_message, graph);
}

MessageUntil::MessageUntil(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  // nothing to do
}

MessageUntil::~MessageUntil() {
  // nothing to do
}

void MessageUntil::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          maxIterations = (unsigned int) message->get_float(0);
          break;
        }
        case BANG: {
          maxIterations = 0xFFFFFFFF; // maximum integer
          break;
        }
        default: {
          break;
        }
      }
      
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_bang(message->get_timestamp());
      for (unsigned int i = 0; i < maxIterations; i++) {
        send_message(0, outgoing_message);
      }
      
      break;
    }
    case 1: {
      if (message->is_bang(0)) {
        maxIterations = 0; // stops the for loop
      }
      break;
    }
    default: {
      break;
    }
  }
}
