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

#include "MessageTimer.h"

MessageObject *MessageTimer::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageTimer(initMessage, graph);
}

MessageTimer::MessageTimer(PdMessage *initMessage, PdGraph *grap) : MessageObject(2, 1, graph) {
  timestampStart = 0.0;
}

MessageTimer::~MessageTimer() {
  // nothing to do
}

void MessageTimer::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->is_bang(0)) {
        timestampStart = message->get_timestamp();
      }
      break;
    }
    case 1: {
      if (message->is_bang(0)) {
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
        outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(),
            (float) (message->get_timestamp() - timestampStart));
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    default: {
      break;
    }
  }
}
