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

#include "MessageInlet.h"
#include "PdGraph.h"

message::Object *MessageInlet::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageInlet(graph);
}

// MessageInlet is initialised with an inlet because it manages connections from outside of the
// containing graph.
MessageInlet::MessageInlet(PdGraph *graph) : message::Object(1, 1, graph) {
  // nothing to do
}

MessageInlet::~MessageInlet() {
  // nothing to do
}

object::Type MessageInlet::get_object_type() {
  return MESSAGE_INLET;
}

void MessageInlet::receive_message(int inlet_index, pd::Message *message) {
  send_message(0, message);
}

list<DspObject *> MessageInlet::get_process_order() {
  // a MessageInlet always returns an empty list as it does not process any audio
  return list<DspObject *>();
}

list<DspObject *> MessageInlet::get_process_orderFromInlet() {
  list<DspObject *> processList;
  for (list<Connection>::iterator it = incoming_connections[0].begin();
      it != incoming_connections[0].end(); ++it) {
    Connection obj_let_pair = *it;
    list<DspObject *> parentProcessList = obj_let_pair.first->get_process_order();
    processList.splice(processList.end(), parentProcessList);
  }
  return processList;
}
