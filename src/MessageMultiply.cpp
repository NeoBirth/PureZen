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

#include "MessageMultiply.h"

MessageObject *MessageMultiply::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageMultiply(initMessage, graph);
}

MessageMultiply::MessageMultiply(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  constant = initMessage->is_float(0) ? initMessage->get_float(0) : 0.0f;
  last = 0.0f;
}

MessageMultiply::~MessageMultiply() {
  // nothing to do
}

std::string MessageMultiply::toString() {
  char str[snprintf(NULL, 0, "* %g", constant)+1];
  snprintf(str, sizeof(str), "* %g", constant);
  return string(str);
}

void MessageMultiply::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          last = message->get_float(0) * constant;
          // allow fallthrough
        }
        case BANG: {
          PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
          outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), last);
          sendMessage(0, outgoingMessage);
          break;
        }
        default: return;
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
