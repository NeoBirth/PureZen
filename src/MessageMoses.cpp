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

#include "MessageMoses.h"

message::Object *MessageMoses::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageMoses(init_message, graph);
}

MessageMoses::MessageMoses(pd::Message *init_message, PdGraph *graph) : message::Object(2, 2, graph) {
  constant = init_message->is_float(0) ? init_message->get_float(0) : 0.0f;
}

MessageMoses::~MessageMoses() {
  // nothing to do
}

void MessageMoses::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_float(0)) {
        send_message((message->get_float(0) < constant) ? 0 : 1, message);
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
