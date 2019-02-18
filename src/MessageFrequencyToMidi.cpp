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

#include "MessageFrequencyToMidi.h"

message::Object *MessageFrequencyToMidi::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageFrequencyToMidi(graph);
}

MessageFrequencyToMidi::MessageFrequencyToMidi(PdGraph *graph) : message::Object(1, 1, graph) {
  // nothing to do
}

MessageFrequencyToMidi::~MessageFrequencyToMidi() {
  // nothing to do
}

void MessageFrequencyToMidi::process_message(int inlet_index, pd::Message *message) {
  if (message->is_float(0)) {
    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
    float f = message->get_float(0);
    float value = (f <= 0.0f) ? -1500.0f : (12.0f * (logf(f / 440.0f) / M_LN2)) + 69.0f;
    outgoing_message->from_timestamp_and_float(message->get_timestamp(), value);
    send_message(0, outgoing_message); // send a message from outlet 0
  }
}
