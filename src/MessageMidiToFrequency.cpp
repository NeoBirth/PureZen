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

#include "MessageMidiToFrequency.h"

message::Object *MessageMidiToFrequency::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageMidiToFrequency(init_message, graph);
}

MessageMidiToFrequency::MessageMidiToFrequency(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  // nothing to do
}

MessageMidiToFrequency::~MessageMidiToFrequency() {
  // nothing to do
}

void MessageMidiToFrequency::process_message(int inlet_index, pd::Message *message) {
  if (message->is_float(0)) {
    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
    float value = 440.0f * powf(2.0f, (message->get_float(0) - 69.0f) / 12.0f);
    outgoing_message->from_timestamp_and_float(message->get_timestamp(), value);
    send_message(0, outgoing_message);
  }
}
