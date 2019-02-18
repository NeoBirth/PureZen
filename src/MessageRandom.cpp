/*
 *  Copyright 2009,2011,2012 Reality Jockey, Ltd.
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

#include "MessageRandom.h"

message::Object *MessageRandom::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageRandom(init_message, graph);
}

MessageRandom::MessageRandom(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  max_inc = init_message->is_float(0) ? ((int) init_message->get_float(0))-1 : 1;
  twister = new MTRand();
}

MessageRandom::~MessageRandom() {
  delete twister;
}

void MessageRandom::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case SYMBOL: {
          if (message->is_symbol_str(0, "seed") && message->is_float(1)) {
            twister->seed((int) message->get_float(1)); // reset the seed
          }
          break;
        }
        case BANG: {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_float(message->get_timestamp(), (float) twister->randInt(max_inc));
          send_message(0, outgoing_message);
          break;
        }
        default: break;
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        max_inc = static_cast<int>(fmaxf(message->get_float(0) - 1.0f, 0.0f));
      }
      break;
    }
    default: break; // ERROR!
  }
}
