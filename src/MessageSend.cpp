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

MessageObject *MessageSend::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageSend(initMessage, graph);
}

MessageSend::MessageSend(PdMessage *initMessage, PdGraph *graph) :
    MessageObject(initMessage->is_symbol(0) ? 1 : 2, 0, graph) {
  name = utils::copy_string(initMessage->is_symbol(0)
      ? initMessage->get_symbol(0) : "zg_default_sendreceive_name");
}

MessageSend::~MessageSend() {
  free(name);
}

void MessageSend::receiveMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: graph->sendMessageToNamedReceivers(name, message); break;
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
