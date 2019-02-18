/*
 *  Copyright 2011 Reality Jockey, Ltd.
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

#include "MessageCputime.h"

message::Object *MessageCputime::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageCputime(init_message, graph);
}

MessageCputime::MessageCputime(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  // nothing to do
}

MessageCputime::~MessageCputime() {
  // nothing to do
}

void MessageCputime::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      if (message->is_bang(0)) {
        gettimeofday(&start, NULL);
      }
      break;
    }
    case 1: {
      if (message->is_bang(0)) {
        timeval end;
        gettimeofday(&end, NULL);
        double elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0; // sec to ms
        elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0; // us to ms
        
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_float(message->get_timestamp(), (float) elapsedTime);
        send_message(0, outgoing_message);
      }
      break;
    }
    default: break;
  }
}
