/*
 *  Copyright 2010,2011,2012 Reality Jockey, Ltd.
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

#include "MessageValue.h"
#include "PdGraph.h"

message::Object *MessageValue::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageValue(init_message, graph);
}

MessageValue::MessageValue(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  if (init_message->is_symbol(0)) {
    name = utils::copy_string(init_message->get_symbol(0));
  } else {
    name = NULL;
    graph->printErr("Object \"value\" MUST be initialised with a name.");
  }
}

MessageValue::~MessageValue() {
  free(name);
}

void MessageValue::process_message(int inlet_index, pd::Message *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      graph->setValueForName(name, message->get_float(0));
      break;
    }
    case BANG: {
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), graph->getValueForName(name));
      send_message(0, outgoing_message);
      break;
    }
    default: break;
  }
}
