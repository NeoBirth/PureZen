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

#include "MessageSwap.h"

MessageObject *MessageSwap::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageSwap(initMessage, graph);
}

MessageSwap::MessageSwap(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 2, graph) {
  left = 0.0f;
  right = initMessage->is_float(0) ? initMessage->get_float(0) : 0.0f;
}

MessageSwap::~MessageSwap() {
  // nothing to do
}

void MessageSwap::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          left = message->get_float(0);
          // allow fallthrough
        }
        case BANG: {
          PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
          outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), left);
          sendMessage(1, outgoingMessage); // send a message from outlet 1
          
          outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), right);
          sendMessage(0, outgoingMessage); // send a message from outlet 0
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        right = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
