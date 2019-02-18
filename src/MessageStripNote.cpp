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

#include "MessageStripNote.h"

message::Object *MessageStripNote::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageStripNote(init_message, graph);
}

MessageStripNote::MessageStripNote(pd::Message *init_message, PdGraph *graph) : message::Object(2, 2, graph) {
  // nothing to do
}

MessageStripNote::~MessageStripNote() {
  // nothing to do
}

void MessageStripNote::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 0) {
    if (message->is_float(0) && message->is_float(1) && message->get_float(1) > 0.0f) {
      pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
      outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(1));
      send_message(1, outgoing_message);

      outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(0));
      send_message(0, outgoing_message);
    }
  }
  // NOTE(mhroth): no idea what right inlet is for
}
