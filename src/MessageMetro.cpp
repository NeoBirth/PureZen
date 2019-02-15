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

#include "MessageMetro.h"
#include "PdGraph.h"

MessageObject *MessageMetro::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageMetro(initMessage, graph);
}

MessageMetro::MessageMetro(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  // default to interval of one second
  intervalInMs = initMessage->is_float(0) ? (double) initMessage->get_float(0) : 1000.0;
  pendingMessage = NULL;
}

MessageMetro::~MessageMetro() {
  // nothign to do. pendingMessage object is owned by context.
}

string MessageMetro::toString() {
  char str[snprintf(NULL, 0, "%s %g", getObjectLabel(), intervalInMs)+1];
  snprintf(str, sizeof(str), "%s %g", getObjectLabel(), intervalInMs);
  return string(str);
}

void MessageMetro::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->get_type(0)) {
        case FLOAT: {
          // any non-zero float may start the metro
          (message->get_float(0) == 0.0f) ? stopMetro() : startMetro(message->get_timestamp());
          break;
        }
        case SYMBOL: {
          if (message->is_symbol_str(0, "stop")) {
            stopMetro();
          }
          break;
        }
        case BANG: {
          startMetro(message->get_timestamp());
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
        intervalInMs = (double) message->get_float(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void MessageMetro::sendMessage(int outletIndex, PdMessage *message) {
  // schedule the pending message before the current one is sent so that if a stop message
  // arrives at this object while in this function, then the next message can be cancelled
  pendingMessage = PD_MESSAGE_ON_STACK(1);
  pendingMessage->initWithTimestampAndBang(message->get_timestamp() + intervalInMs);
  pendingMessage = graph->scheduleMessage(this, 0, pendingMessage);

  MessageObject::sendMessage(outletIndex, message);
}

void MessageMetro::startMetro(double timestamp) {
  // Ensure that there is no pending message for this metro. If there is, then cancel it.
  // This allows a metro to be banged multiple times and always restart the timing from the most
  // recently received bang.
  stopMetro();

  PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
  outgoingMessage->initWithTimestampAndBang(timestamp);
  sendMessage(0, outgoingMessage);
}

void MessageMetro::stopMetro() {
  if (pendingMessage != NULL) {
    graph->cancelMessage(this, 0, pendingMessage);
    pendingMessage = NULL;
  }
}
