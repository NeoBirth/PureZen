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

#include <stdio.h>
#include "MessageNotein.h"

message::Object *MessageNotein::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageNotein(init_message, graph);
}

MessageNotein::MessageNotein(pd::Message *init_message, PdGraph *graph) :
    RemoteMessageReceiver(0, 3, graph) {
  if (init_message->is_float(0) &&
      (init_message->get_float(0) >= 1.0f && init_message->get_float(0) <= 16.0f)) {
    // channel provided (Pd channels are indexed from 1, while ZG channels are indexed from 0)
    channel = (int) (init_message->get_float(0)-1.0f);
    name = (char *) calloc(13, sizeof(char));
    sprintf(name, "zg_notein_%i", channel);
  } else {
    // no channel provided, use omni
    channel = -1;
    name = utils::copy_string((char *) "zg_notein_omni");
  }
}

MessageNotein::~MessageNotein() {
  free(name);
}

int MessageNotein::getChannel() {
  return channel;
}

bool MessageNotein::isOmni() {
  return (channel == -1);
}

void MessageNotein::process_message(int inlet_index, pd::Message *message) {
  pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
  
  if (isOmni()) {
    // send channel
    outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(2));
    send_message(2, outgoing_message);
  }
  
  // send velocity
  outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(1));
  send_message(1, outgoing_message);
  
  // send note
  outgoing_message->from_timestamp_and_float(message->get_timestamp(), message->get_float(0));
  send_message(0, outgoing_message);
}
