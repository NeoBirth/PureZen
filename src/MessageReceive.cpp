/*
 *  Copyright 2009,2012 Reality Jockey, Ltd.
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

#include "MessageReceive.h"

message::Object *MessageReceive::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageReceive(init_message, graph);
}

MessageReceive::MessageReceive(pd::Message *init_message, PdGraph *graph) :
    RemoteMessageReceiver(0, 1, graph) {
  // a receive object can be instantiated with no name. It receives a default.
  name = utils::copy_string(init_message->is_symbol(0)
      ? init_message->get_symbol(0) : "zg_default_sendreceive_name");
}

MessageReceive::~MessageReceive() {
  free(name);
}

void MessageReceive::receive_message(int inlet_index, pd::Message *message) {
  send_message(0, message); // send all received messages to the outlet
}
