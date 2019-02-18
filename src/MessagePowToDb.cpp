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

#include "MessagePowToDb.h"

message::Object *MessagePowToDb::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessagePowToDb(init_message, graph);
}

MessagePowToDb::MessagePowToDb(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  // nothing to do
}

MessagePowToDb::~MessagePowToDb() {
  // nothing to do
}

void MessagePowToDb::process_message(int inlet_index, pd::Message *message) {
  if (message->is_float(0)) {
    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
    float f = message->get_float(0);
    outgoing_message->from_timestamp_and_float(message->get_timestamp(),
        (f <= 0.0f) ? 0.0f : 100.0f + 10.0f * log10f(f));
    send_message(0, outgoing_message);
  }
}
