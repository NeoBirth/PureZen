/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#include "MessageClip.h"

MessageObject *MessageClip::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageClip(initMessage, graph);
}

MessageClip::MessageClip(PdMessage *initMessage, PdGraph *graph) : MessageObject(3, 1, graph) {
  if (initMessage->is_float(0)) {
    if (initMessage->is_float(1)) {
      init(initMessage->get_float(0), initMessage->get_float(1));
    } else {
      init(initMessage->get_float(0), 0.0f);
    }
  } else {
    init(0.0f, 0.0f);
  }
}

MessageClip::~MessageClip() {
  // nothing to do
}

void MessageClip::init(float lowerBound, float upperBound) {
  this->lowerBound = lowerBound;
  this->upperBound = upperBound;
}

void MessageClip::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->is_float(0)) {
        float output = message->get_float(0);
        if (output < lowerBound) {
          output = lowerBound;
        } else if (output > upperBound) {
          output = upperBound;
        }
        PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
        outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), output);
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    case 1: {
      if (message->is_float(0)) {
        lowerBound = message->get_float(0);
      }
      break;
    }
    case 2: {
      if (message->is_float(0)) {
        upperBound = message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
