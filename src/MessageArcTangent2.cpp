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

#include "MessageArcTangent2.h"

MessageObject *MessageArcTangent2::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageArcTangent2(initMessage, graph);
}

MessageArcTangent2::MessageArcTangent2(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  constant = initMessage->is_float(0) ? initMessage->get_float(0) : 0.0f;
}

MessageArcTangent2::~MessageArcTangent2() {
  // nothing to do
}

void MessageArcTangent2::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->is_float(0)) {
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
        outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), atan2f(message->get_float(0), constant));
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        constant = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
