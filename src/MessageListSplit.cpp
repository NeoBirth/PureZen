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

MessageListSplit::MessageListSplit(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 3, graph) {
  splitIndex = initMessage->is_float(0) ? (int) initMessage->get_float(0) : 0;
}

MessageListSplit::~MessageListSplit() {
  // nothing to do
}

void MessageListSplit::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      int numElements = message->get_num_elements();
      if (numElements <= splitIndex) {
        // if there aren't enough elements to split on, forward the message on the third outlet
        sendMessage(2, message);
      } else {
        int numElems = numElements-splitIndex;
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(numElems);
        outgoingMessage->from_timestamp(message->get_timestamp(), numElems);
        memcpy(outgoingMessage->get_element(0), message->get_element(splitIndex), numElems * sizeof(pd::message::Atom));
        sendMessage(1, outgoingMessage);
        
        outgoingMessage = PD_MESSAGE_ON_STACK(splitIndex);
        outgoingMessage->from_timestamp(message->get_timestamp(), splitIndex);
        memcpy(outgoingMessage->get_element(0), message->get_element(0), splitIndex * sizeof(pd::message::Atom));
        sendMessage(0, outgoingMessage);
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
