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

#include "MessageSpigot.h"

MessageObject *MessageSpigot::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageSpigot(initMessage, graph);
}

MessageSpigot::MessageSpigot(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  constant = initMessage->is_float(0) ? initMessage->get_float(0) : 0.0f;
}

MessageSpigot::~MessageSpigot() {
  // nothing to do
}

void MessageSpigot::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (constant != 0.0f) {
        sendMessage(0, message); // if the spigot is on, pass on the message
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
