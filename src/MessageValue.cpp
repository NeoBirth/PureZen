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

MessageObject *MessageValue::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageValue(initMessage, graph);
}

MessageValue::MessageValue(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  if (initMessage->is_symbol(0)) {
    name = utils::copy_string(initMessage->get_symbol(0));
  } else {
    name = NULL;
    graph->printErr("Object \"value\" MUST be initialised with a name.");
  }
}

MessageValue::~MessageValue() {
  free(name);
}

void MessageValue::processMessage(int inletIndex, PdMessage *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      graph->setValueForName(name, message->get_float(0));
      break;
    }
    case BANG: {
      PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
      outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), graph->getValueForName(name));
      sendMessage(0, outgoingMessage);
      break;
    }
    default: break;
  }
}
