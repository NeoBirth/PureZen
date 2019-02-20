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

#include "MessagePipe.h"
#include "PdGraph.h"

message::Object *MessagePipe::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessagePipe(init_message, graph);
}

MessagePipe::MessagePipe(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  delayMs = init_message->is_float(0) ? (double) init_message->get_float(0) : 0.0;
}

MessagePipe::~MessagePipe() {
  // nothing to do
}

bool MessagePipe::should_distribute_message_to_inlets() {
  return false;
}

void MessagePipe::send_message(int outlet_index, pd::Message *message) {
  // remove the scheduled message from the list before it is sent
  scheduledMessagesList.remove(message);
  message::Object::send_message(outlet_index, message);
}

void MessagePipe::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case SYMBOL: {
          if (message->is_symbol_str(0, "flush")) {
            // cancel all scheduled messages and send them immediately
            for(list<pd::Message *>::iterator it = scheduledMessagesList.begin();
                it != scheduledMessagesList.end(); it++) {
              // send the message using the super class's send_message because otherwise the
              // list will be changed while iterating over it. Leads to badness.
              (*it)->set_timestamp(message->get_timestamp());
              message::Object::send_message(0, *it);
              graph->cancel_message(this, 0, *it); // cancel the scheduled message and free it from memory
            }
            scheduledMessagesList.clear();
            break;
          } else if (message->is_symbol_str(0, "clear")) {
            // cancel all scheduled messages
            for(list<pd::Message *>::iterator it = scheduledMessagesList.begin();
                it != scheduledMessagesList.end(); it++) {
              graph->cancel_message(this, 0, *it);
            }
            scheduledMessagesList.clear();
            break;
          }
          // allow fall-through
        }
        case FLOAT:
        case BANG: {
          // copy the message, update the timestamp, schedule it to be sent later
          int numElements = message->get_num_elements();
          pd::Message *scheduledMessage = PD_MESSAGE_ON_STACK(numElements);
          scheduledMessage->from_timestamp(message->get_timestamp() + delayMs, numElements);
          memcpy(scheduledMessage->get_element(0), message->get_element(0), numElements * sizeof(pd::message::Atom));
          scheduledMessagesList.push_back(graph->schedule_message(this, 0, scheduledMessage));
          break;
        }
        default: {
          break;
        }
      }
    }
    case 1: {
      if (message->is_float(0)) {
        delayMs = (double) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
