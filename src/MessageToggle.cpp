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

#include "MessageToggle.h"

message::Object *MessageToggle::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageToggle(init_message, graph);
}

MessageToggle::MessageToggle(pd::Message *initString, PdGraph *graph) : message::Object(1, 1, graph) {
  isOn = false;
  onOutput = 1.0f;
}

MessageToggle::~MessageToggle() {
  // nothing to do
}

const char *MessageToggle::getObjectLabel() {
  return "toggle";
}

void MessageToggle::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      isOn = (message->get_float(0) != 0.0f);
      if (isOn) onOutput = message->get_float(0);
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), isOn ? onOutput : 0.0f);
      send_message(0, outgoing_message);
      break;
    }
    case BANG: {
      isOn = !isOn;
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), isOn ? onOutput : 0.0f);
      send_message(0, outgoing_message);
      break;
    }
    case SYMBOL: {
      if (message->is_symbol_str(0, "set")) {
        if (message->is_float(1)) {
          isOn = (message->get_float(1) != 0.0f);
          if (isOn) onOutput = message->get_float(1);
        }
      }
      break;
    }
    default: break;
  }
}
