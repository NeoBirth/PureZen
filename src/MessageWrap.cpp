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

#include "MessageWrap.h"

message::Object *MessageWrap::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageWrap(init_message, graph);
}

// TODO(mhroth): This object is almost definitely NOT working correctly
MessageWrap::MessageWrap(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  switch (init_message->get_num_elements()) {
    case 0: {
      lower = 0.0f;
      upper = 1.0f;
      break;
    }
    case 1: {
      lower = 0.0f;
      upper = init_message->is_float(0) ? init_message->get_float(0) : 1.0f;
      break;
    }
    case 2: {
      lower = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
      upper = init_message->is_float(1) ? init_message->get_float(1) : 1.0f;
      if (upper < lower) {
        float temp = upper;
        upper = lower;
        lower = temp;
      }
      break;
    }
    default: {
      break;
    }
  }
}

MessageWrap::~MessageWrap() {
  // nothing to do
}

void MessageWrap::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_float(0)) {
        value = message->get_float(0);
        range = upper - lower;
        if (upper <= value)  {
          while (upper <= value) {
            value = value - range;
          }
        } else if (value < lower)  {
          while (value < lower) {
            value = value + range;
          }
        }
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), value);
        send_message(0, outgoing_message);
      }
      break;
    }  
    case 1: {
      if (message->is_float(0)) {
        if (message->get_num_elements() == 1) {
          lower = message->get_float(0);
          upper = 0.0f;
        } else if (message->get_num_elements() == 2) {
          lower = message->get_float(0);
          upper = message->get_float(1);
        }
        if (upper < lower) {
          float temp = upper;
          upper = lower;
          lower = temp;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}
