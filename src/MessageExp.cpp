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

#include "MessageExp.h"

MessageObject *MessageExp::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageExp(initMessage, graph);
}

MessageExp::MessageExp(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  // nothing to do
}

MessageExp::~MessageExp() {
  // nothing to do
}

void MessageExp::processMessage(int inletIndex, PdMessage *message) {
  if (message->is_float(0)) {
    PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
    outgoingMessage->initWithTimestampAndFloat(message->get_timestamp(), expf(message->get_float(0)));
    sendMessage(0, outgoingMessage);    
  }
}
