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

#include "MessageGreaterThan.h"

message::Object *MessageGreaterThan::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageGreaterThan(init_message, graph);
}

MessageGreaterThan::MessageGreaterThan(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
  lastOutput = 0.0f;
}

MessageGreaterThan::~MessageGreaterThan() {
  // nothing to do
}

void MessageGreaterThan::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          lastOutput = (message->get_float(0) > constant) ? 1.0f : 0.0f;
          // allow fallthrough
        }
        case BANG: {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_float(message->get_timestamp(), lastOutput);
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
        constant = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

std::string MessageGreaterThan::toString() {
  char str[snprintf(NULL, 0, "> %g", constant)+1];
  snprintf(str, sizeof(str), "> %g", constant);
  return string(str);
}
