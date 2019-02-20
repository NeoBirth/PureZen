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

#include "MessageSelect.h"
#include "PdGraph.h"

message::Object *MessageSelect::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSelect(init_message, graph);
}

MessageSelect::MessageSelect(pd::Message *init_message, PdGraph *graph) : 
    message::Object((init_message->get_num_elements() < 2) ? 2 : 1, 
                  (init_message->get_num_elements() < 2) ? 2 : init_message->get_num_elements()+1, graph) {
  selectorMessage = init_message->clone_on_heap();
}

MessageSelect::~MessageSelect() {
  selectorMessage->free_message();
}

void MessageSelect::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      pd::message::Atom *messageElement = message->get_element(0);
      int numSelectors = selectorMessage->get_num_elements();
      for (int i = 0; i < numSelectors; i++) {
        if (selectorMessage->atom_is_equal_to(i, messageElement)) {
          // send bang from matching outlet
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_bang(message->get_timestamp());
          send_message(i, outgoing_message);
          return;
        }
      }

      // message does not match any selector. Send it out to of the last outlet.
      send_message(numSelectors, message);
      break;
    }
    case 1: {
      // TODO(mhroth): be able to set the selector
      graph->print_err("select currently does not support setting the selector via the right inlet.\n");
      break;
    }
    default: {
      break;
    }
  }
}
