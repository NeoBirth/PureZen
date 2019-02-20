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

message::Object *MessageTrigger::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageTrigger(init_message, graph);
}

MessageTrigger::MessageTrigger(pd::Message *init_message, PdGraph *graph) :
    message::Object(1, init_message->get_num_elements(), graph) {
  // resolve the symbols to type in a copy of the original message on the stack. That way the
  // symbol pointers don't get lost when replace with new pd::message::Atom types.
  int numElements = init_message->get_num_elements();
  pd::Message *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(0.0, numElements);
  memcpy(message->get_element(0), init_message->get_element(0), numElements*sizeof(pd::message::Atom));
  message->resolve_symbols_to_type();
  castMessage = message->clone_on_heap();
}

MessageTrigger::~MessageTrigger() {
  castMessage->free_message();
}

string MessageTrigger::toString() {
  std::string out = MessageTrigger::get_object_label();
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

void MessageTrigger::process_message(int inlet_index, pd::Message *message) {
  pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
  int numMessageOutlets = outgoing_connections.size();
  for (int i = numMessageOutlets-1; i >= 0; i--) { // send messages from outlets right-to-left
    // TODO(mhroth): There is currently no support for converting to a LIST type
    switch (message->get_type(0)) { // converting from...
      case FLOAT: {
        switch (castMessage->get_type(i)) { // converting to...
          case ANYTHING:
          case FLOAT: {
            outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(0));
            send_message(i, outgoing_message);
            break;
          }
          case SYMBOL: {
            outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), (char *) "float");
            send_message(i, outgoing_message);
            break;
          }
          case BANG: {
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          } default: {
            // send bang
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          }
        }
        break;
      }
      case SYMBOL: {
        switch (castMessage->get_type(i)) {
          case FLOAT: {
            graph->print_err("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case ANYTHING: {
            outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), message->get_symbol(0));
            send_message(i, outgoing_message);
            break;
          }
          case SYMBOL: {
            graph->print_err("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case BANG: {
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          }
          default: {
            // send bang
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          }
        }
        break;
      }
      case BANG: {
        switch (castMessage->get_type(i)) {
          case FLOAT: {
            outgoing_message->from_timestamp_and_float(message->get_timestamp(), 0.0f);
            send_message(i, outgoing_message);
            break;
          }
          case SYMBOL: {
            outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), (char *) "symbol");
            send_message(i, outgoing_message);
            break;
          }
          case ANYTHING:
          case BANG: {
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          }
          default: {
            // send bang, error
            outgoing_message->from_timestamp_and_bang(message->get_timestamp());
            send_message(i, outgoing_message);
            break;
          }
        }
        break;
      }
      default: {
        // produce a bang if the input type is unknown (error)
        outgoing_message->from_timestamp_and_bang(message->get_timestamp());
        send_message(i, outgoing_message);
        break;
      }
    }
  }
}
