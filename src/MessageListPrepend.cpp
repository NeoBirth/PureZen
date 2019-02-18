/*
 *  Copyright 2010,2011 Reality Jockey, Ltd.
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

#include "MessageListPrepend.h"

MessageListPrepend::MessageListPrepend(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  prependMessage = init_message->clone_on_heap();
}

MessageListPrepend::~MessageListPrepend() {
  prependMessage->freeMessage();
}

void MessageListPrepend::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      int numPrependElements = prependMessage->get_num_elements();
      int numMessageElements = message->get_num_elements();
      int numElements = numPrependElements + numMessageElements;
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(numElements);
      outgoing_message->from_timestamp(message->get_timestamp(), numElements);
      memcpy(outgoing_message->get_element(0), prependMessage->get_element(0), numPrependElements * sizeof(pd::message::Atom));
      memcpy(outgoing_message->get_element(numPrependElements), message->get_element(0), numMessageElements * sizeof(pd::message::Atom));
      send_message(0, outgoing_message);
      break;
    }
    case 1: {
      // NOTE(mhroth): would be faster to copy in place rather than destroying and creating memory
      // can change if it becomes a problem
      prependMessage->freeMessage();
      prependMessage = message->clone_on_heap();
      break;
    }
    default: {
      break;
    }
  }
}
