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

#include "MessageClip.h"

message::Object *MessageClip::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageClip(init_message, graph);
}

MessageClip::MessageClip(pd::Message *init_message, PdGraph *graph) : message::Object(3, 1, graph) {
  if (init_message->is_float(0)) {
    if (init_message->is_float(1)) {
      init(init_message->get_float(0), init_message->get_float(1));
    } else {
      init(init_message->get_float(0), 0.0f);
    }
  } else {
    init(0.0f, 0.0f);
  }
}

MessageClip::~MessageClip() {
  // nothing to do
}

void MessageClip::init(float lowerBound, float upperBound) {
  this->lowerBound = lowerBound;
  this->upperBound = upperBound;
}

void MessageClip::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_float(0)) {
        float output = message->get_float(0);
        if (output < lowerBound) {
          output = lowerBound;
        } else if (output > upperBound) {
          output = upperBound;
        }
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), output);
        send_message(0, outgoing_message);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        lowerBound = message->get_float(0);
      }
      break;
    }
    case 2: {
      if (message->is_float(0)) {
        upperBound = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
