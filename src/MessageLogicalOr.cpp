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

#include "MessageLogicalOr.h"

message::Object *MessageLogicalOr::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageLogicalOr(init_message, graph);
}

MessageLogicalOr::MessageLogicalOr(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  left = 0.0f;
  right = 0.0f;
}

MessageLogicalOr::~MessageLogicalOr() {
  // nothing to do
}

void MessageLogicalOr::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          left = message->get_float(0);
          // allow fallthrough
        }
        case BANG: {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_float(message->get_timestamp(),
              (left == 0.0f && right == 0.0f) ? 0.0f : 1.0f);
          send_message(0, outgoing_message);
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
