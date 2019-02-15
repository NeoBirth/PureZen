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

#include "MessagePack.h"
#include "PdGraph.h"

MessageObject *MessagePack::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessagePack(initMessage, graph);
}

MessagePack::MessagePack(PdMessage *initMessage, PdGraph *graph) :
    MessageObject(initMessage->get_num_elements(), 1, graph) {
  int numElements = initMessage->get_num_elements();
  PdMessage *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(0.0, numElements);
  memcpy(message->get_element(0), initMessage->get_element(0), numElements*sizeof(MessageAtom));
  message->resolve_symbols_to_type();
  outgoingMessage = message->clone_on_heap();
}

MessagePack::~MessagePack() {
  outgoingMessage->freeMessage();
}

string MessagePack::toString() {
  std::string out = MessagePack::getObjectLabel();
  for (int i = 0; i < outgoingMessage->get_num_elements(); i++) {
    switch (outgoingMessage->get_type(i)) {
      case FLOAT: out += " f"; break;
      case SYMBOL: out += " s"; break;
      case BANG: out += " b"; break;
      case LIST: out += " l"; break;
      case ANYTHING:
      default: out += " a"; break;
    }
  }
  return out;
}

void MessagePack::processMessage(int inletIndex, PdMessage *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      if (outgoingMessage->is_float(inletIndex)) {
        outgoingMessage->set_float(inletIndex, message->get_float(0));
        onBangAtInlet(inletIndex, message->get_timestamp());
      } else {
        graph->printErr("pack: type mismatch: %s expected but got %s at inlet %i.\n",
            utils::message_element_type_to_string(outgoingMessage->get_type(inletIndex)),
            utils::message_element_type_to_string(message->get_type(0)),
            inletIndex + 1);
        return;
      }
      break;
    }
    case SYMBOL: {
      if (outgoingMessage->is_symbol(inletIndex)) {
        // NOTE(mhroth): this approach can lead to a lot of fragemented memory if symbols are
        // replaced often
        free(outgoingMessage->get_symbol(inletIndex)); // free the preexisting symbol on the heap
        // create a new symbol on the heap and store it in the outgoing message
        outgoingMessage->set_symbol(inletIndex, utils::copy_string(message->get_symbol(0)));
        onBangAtInlet(inletIndex, message->get_timestamp());
      } else {
        graph->printErr("pack: type mismatch: %s expected but got %s at inlet %i.\n",
            utils::message_element_type_to_string(outgoingMessage->get_type(inletIndex)),
            utils::message_element_type_to_string(message->get_type(0)),
            inletIndex + 1);
        return;
      }
      break;
    }
    case BANG: {
      onBangAtInlet(inletIndex, message->get_timestamp());
      break;
    }
    default: {
      break;
    }
  }
}

void MessagePack::onBangAtInlet(int inletIndex, double timestamp) {
  if (inletIndex == 0) {
    // send the outgoing message
    outgoingMessage->set_timestamp(timestamp);
    sendMessage(0, outgoingMessage);
  }
}
