/*
 *  Copyright 2009,2010,2012 Reality Jockey, Ltd.
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

#include "MessageSend.h"
#include "PdGraph.h"

message::Object *MessageSend::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSend(init_message, graph);
}

MessageSend::MessageSend(pd::Message *init_message, PdGraph *graph) :
    message::Object(init_message->is_symbol(0) ? 1 : 2, 0, graph) {
  name = utils::copy_string(init_message->is_symbol(0)
      ? init_message->get_symbol(0) : "zg_default_sendreceive_name");
}

MessageSend::~MessageSend() {
  free(name);
}

void MessageSend::receive_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: graph->send_messageToNamedReceivers(name, message); break;
    case 1: {
      if (message->is_symbol(0)) {
        free(name);
        name = utils::copy_string(message->get_symbol(0));
      }
      break;
    }
    default: break;
  }
}
