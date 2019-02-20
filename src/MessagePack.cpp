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

message::Object *MessagePack::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessagePack(init_message, graph);
}

MessagePack::MessagePack(pd::Message *init_message, PdGraph *graph) :
    message::Object(init_message->get_num_elements(), 1, graph) {
  int numElements = init_message->get_num_elements();
  pd::Message *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(0.0, numElements);
  memcpy(message->get_element(0), init_message->get_element(0), numElements*sizeof(pd::message::Atom));
  message->resolve_symbols_to_type();
  outgoing_message = message->clone_on_heap();
}

MessagePack::~MessagePack() {
  outgoing_message->free_message();
}

string MessagePack::toString() {
  std::string out = MessagePack::get_object_label();
  for (int i = 0; i < outgoing_message->get_num_elements(); i++) {
    switch (outgoing_message->get_type(i)) {
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

void MessagePack::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      if (outgoing_message->is_float(inlet_index)) {
        outgoing_message->set_float(inlet_index, message->get_float(0));
        onBangAtInlet(inlet_index, message->get_timestamp());
      } else {
        graph->print_err("pack: type mismatch: %s expected but got %s at inlet %i.\n",
            utils::message_element_type_to_string(outgoing_message->get_type(inlet_index)),
            utils::message_element_type_to_string(message->get_type(0)),
            inlet_index + 1);
        return;
      }
      break;
    }
    case SYMBOL: {
      if (outgoing_message->is_symbol(inlet_index)) {
        // NOTE(mhroth): this approach can lead to a lot of fragemented memory if symbols are
        // replaced often
        free(outgoing_message->get_symbol(inlet_index)); // free the preexisting symbol on the heap
        // create a new symbol on the heap and store it in the outgoing message
        outgoing_message->set_symbol(inlet_index, utils::copy_string(message->get_symbol(0)));
        onBangAtInlet(inlet_index, message->get_timestamp());
      } else {
        graph->print_err("pack: type mismatch: %s expected but got %s at inlet %i.\n",
            utils::message_element_type_to_string(outgoing_message->get_type(inlet_index)),
            utils::message_element_type_to_string(message->get_type(0)),
            inlet_index + 1);
        return;
      }
      break;
    }
    case BANG: {
      onBangAtInlet(inlet_index, message->get_timestamp());
      break;
    }
    default: {
      break;
    }
  }
}

void MessagePack::onBangAtInlet(int inlet_index, double timestamp) {
  if (inlet_index == 0) {
    // send the outgoing message
    outgoing_message->set_timestamp(timestamp);
    send_message(0, outgoing_message);
  }
}
