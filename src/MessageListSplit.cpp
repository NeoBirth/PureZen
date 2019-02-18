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

#include <string.h>
#include "MessageListSplit.h"

MessageListSplit::MessageListSplit(pd::Message *init_message, PdGraph *graph) : message::Object(2, 3, graph) {
  splitIndex = init_message->is_float(0) ? (int) init_message->get_float(0) : 0;
}

MessageListSplit::~MessageListSplit() {
  // nothing to do
}

void MessageListSplit::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      int numElements = message->get_num_elements();
      if (numElements <= splitIndex) {
        // if there aren't enough elements to split on, forward the message on the third outlet
        send_message(2, message);
      } else {
        int numElems = numElements-splitIndex;
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(numElems);
        outgoing_message->from_timestamp(message->get_timestamp(), numElems);
        memcpy(outgoing_message->get_element(0), message->get_element(splitIndex), numElems * sizeof(pd::message::Atom));
        send_message(1, outgoing_message);
        
        outgoing_message = PD_MESSAGE_ON_STACK(splitIndex);
        outgoing_message->from_timestamp(message->get_timestamp(), splitIndex);
        memcpy(outgoing_message->get_element(0), message->get_element(0), splitIndex * sizeof(pd::message::Atom));
        send_message(0, outgoing_message);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        // split index may not be negative
        splitIndex = (message->get_float(0) < 0.0f) ? 0 : (int) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
