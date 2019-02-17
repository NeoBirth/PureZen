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

MessageObject *MessagePipe::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessagePipe(initMessage, graph);
}

MessagePipe::MessagePipe(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  delayMs = initMessage->is_float(0) ? (double) initMessage->get_float(0) : 0.0;
}

MessagePipe::~MessagePipe() {
  // nothing to do
}

bool MessagePipe::shouldDistributeMessageToInlets() {
  return false;
}

void MessagePipe::sendMessage(int outletIndex, PdMessage *message) {
  // remove the scheduled message from the list before it is sent
  scheduledMessagesList.remove(message);
  MessageObject::sendMessage(outletIndex, message);
}

void MessagePipe::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->get_type(0)) {
        case SYMBOL: {
          if (message->is_symbol_str(0, "flush")) {
            // cancel all scheduled messages and send them immediately
            for(list<PdMessage *>::iterator it = scheduledMessagesList.begin();
                it != scheduledMessagesList.end(); it++) {
              // send the message using the super class's sendMessage because otherwise the
              // list will be changed while iterating over it. Leads to badness.
              (*it)->set_timestamp(message->get_timestamp());
              MessageObject::sendMessage(0, *it);
              graph->cancelMessage(this, 0, *it); // cancel the scheduled message and free it from memory
            }
            scheduledMessagesList.clear();
            break;
          } else if (message->is_symbol_str(0, "clear")) {
            // cancel all scheduled messages
            for(list<PdMessage *>::iterator it = scheduledMessagesList.begin();
                it != scheduledMessagesList.end(); it++) {
              graph->cancelMessage(this, 0, *it);
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
          PdMessage *scheduledMessage = PD_MESSAGE_ON_STACK(numElements);
          scheduledMessage->from_timestamp(message->get_timestamp() + delayMs, numElements);
          memcpy(scheduledMessage->get_element(0), message->get_element(0), numElements * sizeof(pd::message::Atom));
          scheduledMessagesList.push_back(graph->scheduleMessage(this, 0, scheduledMessage));
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
