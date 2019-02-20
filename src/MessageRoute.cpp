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

#include "MessageRoute.h"

message::Object *MessageRoute::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageRoute(init_message, graph);
}

MessageRoute::MessageRoute(pd::Message *init_message, PdGraph *graph) : 
    message::Object(1, init_message->get_num_elements()+1, graph) {
  routeMessage = init_message->clone_on_heap();
}

MessageRoute::~MessageRoute() {
  routeMessage->free_message();
}

void MessageRoute::process_message(int inlet_index, pd::Message *message) {
  int numRouteChecks = routeMessage->get_num_elements();
  int outlet_index = numRouteChecks; // by default, send the message out of the right outlet
  // find which indicator that message matches
  pd::message::Atom *messageAtom = message->get_element(0);
  for (int i = 0; i < numRouteChecks; i++) {
    if (routeMessage->atom_is_equal_to(i, messageAtom)) {
      outlet_index = i;
      break;
    }
  }
  
  if (outlet_index == numRouteChecks) {
    // no match found, forward on right oulet
    send_message(outlet_index, message);
  } else {
    // construct a new message to send from the given outlet
    int numElements = message->get_num_elements() - 1;
    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(numElements);
    outgoing_message->from_timestamp(message->get_timestamp(), numElements);
    memcpy(outgoing_message->get_element(0), message->get_element(1), numElements*sizeof(pd::message::Atom));
    send_message(outlet_index, outgoing_message);
  }
}
