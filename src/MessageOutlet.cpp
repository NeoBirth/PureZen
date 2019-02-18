/*
 *  Copyright 2009,2011 Reality Jockey, Ltd.
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

#include "MessageOutlet.h"
#include "PdGraph.h"

message::Object *MessageOutlet::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageOutlet(init_message, graph);
}

// MessageOutlets is initialised with one outlet because it handles all outgoing connections
// for the containing graph.
MessageOutlet::MessageOutlet(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  coordinates.x = 0;
}

MessageOutlet::~MessageOutlet() {
  // nothing to do
}

bool MessageOutlet::is_leaf_node() {
  return true;
}

object::Type MessageOutlet::get_object_type() {
  return MESSAGE_OUTLET;
}

void MessageOutlet::receive_message(int inlet_index, pd::Message *message) {
  send_message(0, message);
}
