/*
 *  Copyright 2010 Reality Jockey, Ltd.
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

#include "MessageSwitch.h"
#include "PdGraph.h"

message::Object *MessageSwitch::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSwitch(init_message, graph);
}

MessageSwitch::MessageSwitch(pd::Message *init_message, PdGraph *graph) : message::Object(1, 0, graph) {
  // TODO(mhroth): parse object arguments 
}

MessageSwitch::~MessageSwitch() {
  // nothing to do
}

void MessageSwitch::process_message(int inlet_index, pd::Message *message) {
  if (message->is_float(0)) {
    graph->setSwitch(message->get_float(0) != 0.0f);
  }
}
