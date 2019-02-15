/*
 *  Copyright 2010,2011 Reality Jockey, Ltd.
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

#include "MessageTableRead.h"
#include "PdGraph.h"

MessageObject *MessageTableRead::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageTableRead(initMessage, graph);
}

MessageTableRead::MessageTableRead(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  name = initMessage->is_symbol(0) ? utils::copy_string(initMessage->get_symbol(0)) : NULL;
  table = NULL; // registration process will set the correct pointer
}

MessageTableRead::~MessageTableRead() {
  free(name);
}

char *MessageTableRead::getName() {
  return name;
}

void MessageTableRead::setTable(MessageTable *aTable) {
  table = aTable;
}

void MessageTableRead::processMessage(int inletIndex, PdMessage *message) {
  switch (message->get_type(0)) {
    case FLOAT: {
      if (table != NULL) {
        int bufferLength = 0;
        float *buffer = table->getBuffer(&bufferLength);
        int index = (int) message->get_float(0);
        if (index >= 0 && index < bufferLength) {
          PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
          outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), buffer[index]);
          sendMessage(0, outgoingMessage);
        }
      }
      break;
    }
    case SYMBOL: {
      if (message->is_symbol_str(0, "set") && message->is_symbol(1)) {
        free(name);
        name = utils::copy_string(message->get_symbol(1));
        table = graph->getTable(name);
      }
      break;
    }
    default: {
      break;
    }
  }
}
