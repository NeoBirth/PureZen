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

#include "MessageDelay.h"
#include "PdGraph.h"

message::Object *MessageDelay::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageDelay(init_message, graph);
}

MessageDelay::MessageDelay(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  delayMs = init_message->is_float(0) ? (double) init_message->get_float(0) : 0.0;
  scheduledMessage = NULL;
}

MessageDelay::~MessageDelay() {
  // nothing to do
}

void MessageDelay::cancelScheduledMessageIfExists() {
  if (scheduledMessage != NULL) {
    graph->cancelMessage(this, 0, scheduledMessage);
    scheduledMessage = NULL;
  }
}

void MessageDelay::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case SYMBOL: {
          if (message->is_symbol_str(0, "stop")) {
            cancelScheduledMessageIfExists();
            break;
          }
          // allow fall-through
        }
        case FLOAT:
        case BANG: {
          cancelScheduledMessageIfExists();
          scheduledMessage = PD_MESSAGE_ON_STACK(1);
          scheduledMessage->from_timestamp_and_bang(message->get_timestamp() + delayMs);
          scheduledMessage = graph->schedule_message(this, 0, scheduledMessage);
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
        // if an outstanding message exists when the delay is reset, the message is cancelled
        cancelScheduledMessageIfExists();
        delayMs = (double) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void MessageDelay::send_message(int outlet_index, pd::Message *message) {
  if (message == scheduledMessage) {
    // now that we know that this message is being sent, we don't have to worry about it anymore
    scheduledMessage = NULL;
  }
  message::Object::send_message(outlet_index, message);
}
