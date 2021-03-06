/*
 *  Copyright 2012 Reality Jockey, Ltd.
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

#include "MessageMakefilename.h"

message::Object *MessageMakefilename::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageMakefilename(init_message, graph);
}

MessageMakefilename::MessageMakefilename(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  format = utils::copy_string(init_message->is_symbol(0) ? init_message->get_symbol(0) : "");
}

MessageMakefilename::~MessageMakefilename() {
  free(format);
}

void MessageMakefilename::process_message(int inlet_index, pd::Message *message) {
  if (inlet_index == 0) {
    switch (message->get_type(0)) {
      case FLOAT: {
        char str[snprintf(NULL, 0, format, (int) message->get_float(0))+1];
        snprintf(str, sizeof(str), format, (int) message->get_float(0));
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), str);
        send_message(0, outgoing_message);
        break;
      }
      case SYMBOL: {
        if (message->is_symbol_str(0, "set") && message->is_symbol(1)) {
          free(format);
          format = utils::copy_string(message->get_symbol(1));
        } else {
          char str[snprintf(NULL, 0, format, message->get_symbol(0))+1];
          snprintf(str, sizeof(str), format, message->get_symbol(0));
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), str);
          send_message(0, outgoing_message);
        }
        break;
      }
      default: break;
    }
  }
}
