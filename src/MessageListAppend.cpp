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

#include "MessageListAppend.h"
#include "MessageListLength.h"
#include "MessageListPrepend.h"
#include "MessageListSplit.h"
#include "MessageListTrim.h"

// MessageListAppend is the default factor for all list objects
MessageObject *MessageListAppend::newObject(PdMessage *initMessage, PdGraph *graph) {
  if (initMessage->is_symbol(0)) {
    if (initMessage->is_symbol_str(0, "append") ||
        initMessage->is_symbol_str(0, "prepend") ||
        initMessage->is_symbol_str(0, "split")) {
      int numElements = initMessage->get_num_elements()-1;
      PdMessage *message = PD_MESSAGE_ON_STACK(numElements);
      message->from_timestamp(0.0, numElements);
      memcpy(message->get_element(0), initMessage->get_element(1), numElements*sizeof(pd::message::Atom));
      MessageObject *messageObject = NULL;
      if (initMessage->is_symbol_str(0, "append")) {
        messageObject = new MessageListAppend(message, graph);
      } else if (initMessage->is_symbol_str(0, "prepend")) {
        messageObject = new MessageListPrepend(message, graph);
      } else if (initMessage->is_symbol_str(0, "split")) {
        messageObject = new MessageListSplit(message, graph);
      }
      return messageObject;
    } else if (initMessage->is_symbol_str(0, "trim")) {
      // trim and length do not act on the initMessage
      return new MessageListTrim(initMessage, graph);
    } else if (initMessage->is_symbol_str(0, "length")) {
      return new MessageListLength(initMessage, graph);
    } else {
      return new MessageListAppend(initMessage, graph);
    }
  } else {
    return new MessageListAppend(initMessage, graph);
  }
}

MessageListAppend::MessageListAppend(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  appendMessage = initMessage->clone_on_heap();
}

MessageListAppend::~MessageListAppend() {
  appendMessage->freeMessage();
}

bool MessageListAppend::shouldDistributeMessageToInlets() {
  return false;
}

void MessageListAppend::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      // if the incoming message is a bang, then it is considered to be a list of length zero
      int numMessageElements = (!message->is_bang(0)) ? message->get_num_elements() : 0;
      int numAppendElements = appendMessage->get_num_elements();
      int numTotalElements = numMessageElements + numAppendElements;
      if (numTotalElements > 0) {
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(numTotalElements);
        outgoingMessage->from_timestamp(message->get_timestamp(), numTotalElements);
        memcpy(outgoingMessage->get_element(0), message->get_element(0), numMessageElements*sizeof(pd::message::Atom));
        memcpy(outgoingMessage->get_element(numMessageElements), appendMessage->get_element(0), numAppendElements*sizeof(pd::message::Atom));
        sendMessage(0, outgoingMessage);
      } else {
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
        outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    case 1: {
      if (message->is_bang(0)) {
        // bangs are considered a list of size zero
        appendMessage->freeMessage();
        PdMessage *message = PD_MESSAGE_ON_STACK(0);
        message->from_timestamp(0.0, 0);
        appendMessage = message->clone_on_heap();
      } else {
        appendMessage->freeMessage();
        appendMessage = message->clone_on_heap();
      }
      break;
    }
    default: {
      break;
    }
  }
}
