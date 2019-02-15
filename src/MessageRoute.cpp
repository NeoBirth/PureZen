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

MessageObject *MessageRoute::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageRoute(initMessage, graph);
}

MessageRoute::MessageRoute(PdMessage *initMessage, PdGraph *graph) : 
    MessageObject(1, initMessage->get_num_elements()+1, graph) {
  routeMessage = initMessage->clone_on_heap();
}

MessageRoute::~MessageRoute() {
  routeMessage->freeMessage();
}

void MessageRoute::processMessage(int inletIndex, PdMessage *message) {
  int numRouteChecks = routeMessage->get_num_elements();
  int outletIndex = numRouteChecks; // by default, send the message out of the right outlet
  // find which indicator that message matches
  MessageAtom *messageAtom = message->get_element(0);
  for (int i = 0; i < numRouteChecks; i++) {
    if (routeMessage->atom_is_equal_to(i, messageAtom)) {
      outletIndex = i;
      break;
    }
  }
  
  if (outletIndex == numRouteChecks) {
    // no match found, forward on right oulet
    sendMessage(outletIndex, message);
  } else {
    // construct a new message to send from the given outlet
    int numElements = message->get_num_elements() - 1;
    PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(numElements);
    outgoingMessage->from_timestamp(message->get_timestamp(), numElements);
    memcpy(outgoingMessage->get_element(0), message->get_element(1), numElements*sizeof(MessageAtom));
    sendMessage(outletIndex, outgoingMessage);
  }
}
