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

#include "MessageUnpack.h"
#include "PdGraph.h"

MessageObject *MessageUnpack::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageUnpack(initMessage, graph);
}

MessageUnpack::MessageUnpack(PdMessage *initMessage, PdGraph *graph) :
    MessageObject(1, (initMessage->get_num_elements() < 2) ? 2 : initMessage->get_num_elements(), graph) {
  if (initMessage->get_num_elements() < 2) {
    // if unpack is not initialised with anything, assume two "anything" outputs
    templateMessage = PD_MESSAGE_ON_STACK(2);
    templateMessage->from_timestamp(0.0, 2);
    templateMessage->set_anything(0);
    templateMessage->set_anything(1);
    templateMessage = templateMessage->clone_on_heap();
  } else {
    templateMessage = initMessage->clone_on_heap();
    templateMessage->resolve_symbols_to_type();
  }
}

MessageUnpack::~MessageUnpack() {
  templateMessage->freeMessage();
}

string MessageUnpack::toString() {
  std::string out = MessageUnpack::getObjectLabel();
  for (int i = 0; i < templateMessage->get_num_elements(); i++) {
    switch (templateMessage->get_type(i)) {
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

void MessageUnpack::processMessage(int inletIndex, PdMessage *message) {
  int numElements = message->get_num_elements();
  if (templateMessage->get_num_elements() < message->get_num_elements()) {
    numElements = templateMessage->get_num_elements();
  }
  PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
  for (int i = numElements-1; i >= 0; i--) {
    elementType = templateMessage->get_type(i);
    if (elementType == message->get_type(i) || elementType == ANYTHING) {
      switch (elementType) {
        case FLOAT: {
          outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), message->get_float(i));
          sendMessage(i, outgoingMessage);
          break;
        }
        case SYMBOL: {
          outgoingMessage->initWithTimestampAndSymbol(message->get_timestamp(), message->get_symbol(i));
          sendMessage(i, outgoingMessage);
          break;
        }
        case ANYTHING: {
          switch (message->get_type(i)) {
            case FLOAT: {
              outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), message->get_float(i));
              break;
            }
            case SYMBOL: {
              outgoingMessage->initWithTimestampAndSymbol(message->get_timestamp(), message->get_symbol(i));
              break;
            }
            default: {
              break;
            }
          }
          sendMessage(i, outgoingMessage);
        }
        default: {
          break;
        }
      }
    } else {
      graph->printErr("unpack: type mismatch: %s expected but got %s.",
          utils::message_element_type_to_string(elementType),
          utils::message_element_type_to_string(message->get_type(i)));
    }
  }
}
