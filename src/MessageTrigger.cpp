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

#include "MessageTrigger.h"
#include "PdGraph.h"

MessageObject *MessageTrigger::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageTrigger(initMessage, graph);
}

MessageTrigger::MessageTrigger(PdMessage *initMessage, PdGraph *graph) :
    MessageObject(1, initMessage->get_num_elements(), graph) {
  // resolve the symbols to type in a copy of the original message on the stack. That way the
  // symbol pointers don't get lost when replace with new pd::message::Atom types.
  int numElements = initMessage->get_num_elements();
  PdMessage *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(0.0, numElements);
  memcpy(message->get_element(0), initMessage->get_element(0), numElements*sizeof(pd::message::Atom));
  message->resolve_symbols_to_type();
  castMessage = message->clone_on_heap();
}

MessageTrigger::~MessageTrigger() {
  castMessage->freeMessage();
}

string MessageTrigger::toString() {
  std::string out = MessageTrigger::getObjectLabel();
  for (int i = 0; i < castMessage->get_num_elements(); i++) {
    switch (castMessage->get_type(i)) {
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

void MessageTrigger::processMessage(int inletIndex, PdMessage *message) {
  PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
  int numMessageOutlets = outgoingMessageConnections.size();
  for (int i = numMessageOutlets-1; i >= 0; i--) { // send messages from outlets right-to-left
    // TODO(mhroth): There is currently no support for converting to a LIST type
    switch (message->get_type(0)) { // converting from...
      case FLOAT: {
        switch (castMessage->get_type(i)) { // converting to...
          case ANYTHING:
          case FLOAT: {
            outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), message->get_float(0));
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            outgoingMessage->initWithTimestampAndSymbol(message->get_timestamp(), (char *) "float");
            sendMessage(i, outgoingMessage);
            break;
          }
          case BANG: {
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          } default: {
            // send bang
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      case SYMBOL: {
        switch (castMessage->get_type(i)) {
          case FLOAT: {
            graph->printErr("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case ANYTHING: {
            outgoingMessage->initWithTimestampAndSymbol(message->get_timestamp(), message->get_symbol(0));
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            graph->printErr("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case BANG: {
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          default: {
            // send bang
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      case BANG: {
        switch (castMessage->get_type(i)) {
          case FLOAT: {
            outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), 0.0f);
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            outgoingMessage->initWithTimestampAndSymbol(message->get_timestamp(), (char *) "symbol");
            sendMessage(i, outgoingMessage);
            break;
          }
          case ANYTHING:
          case BANG: {
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          default: {
            // send bang, error
            outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      default: {
        // produce a bang if the input type is unknown (error)
        outgoingMessage->initWithTimestampAndBang(message->get_timestamp());
        sendMessage(i, outgoingMessage);
        break;
      }
    }
  }
}
