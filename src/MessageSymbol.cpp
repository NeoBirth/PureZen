/*
 *  Copyright 2009,2010,2011,2012 Reality Jockey, Ltd.
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

#include "MessageSymbol.h"

message::Object *MessageSymbol::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSymbol(init_message, graph);
}

MessageSymbol::MessageSymbol(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  if (init_message->is_symbol(0)) {
    copy_string(init_message->get_symbol(0));
  } else {
    memset(symbol, 0, SYMBOL_BUFFER_LENGTH * sizeof(char));
  }
}

MessageSymbol::~MessageSymbol() {
  // nothing to do
}

bool MessageSymbol::copy_string(const char *s) {
  if (strlen(s) < SYMBOL_BUFFER_LENGTH) {
    strcpy(symbol, s);
    return true;
  } else {
    return false;
  }
}

void MessageSymbol::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      switch (message->get_type(0)) {
        case SYMBOL: {
          copy_string(message->get_symbol(0));
          // allow fallthrough
        }
        case BANG: {
          pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
          outgoing_message->from_timestamp_and_symbol(message->get_timestamp(), symbol);
          send_message(0, outgoing_message);
          break;
        }
        default: break;
      }
      break;
    }
    case 1: {
      if (message->is_symbol(0)) {
        copy_string(message->get_symbol(0));
      }
      break;
    }
    default: break;
  }
}
