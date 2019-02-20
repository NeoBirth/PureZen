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

#include "MessageListAppend.h"
#include "MessageListLength.h"
#include "MessageListPrepend.h"
#include "MessageListSplit.h"
#include "MessageListTrim.h"

// MessageListAppend is the default factor for all list objects
message::Object *MessageListAppend::new_object(pd::Message *init_message, PdGraph *graph) {
  if (init_message->is_symbol(0)) {
    if (init_message->is_symbol_str(0, "append") ||
        init_message->is_symbol_str(0, "prepend") ||
        init_message->is_symbol_str(0, "split")) {
      int numElements = init_message->get_num_elements()-1;
      pd::Message *message = PD_MESSAGE_ON_STACK(numElements);
      message->from_timestamp(0.0, numElements);
      memcpy(message->get_element(0), init_message->get_element(1), numElements*sizeof(pd::message::Atom));
      message::Object *message_obj = NULL;
      if (init_message->is_symbol_str(0, "append")) {
        message_obj = new MessageListAppend(message, graph);
      } else if (init_message->is_symbol_str(0, "prepend")) {
        message_obj = new MessageListPrepend(message, graph);
      } else if (init_message->is_symbol_str(0, "split")) {
        message_obj = new MessageListSplit(message, graph);
      }
      return message_obj;
    } else if (init_message->is_symbol_str(0, "trim")) {
      // trim and length do not act on the init_message
      return new MessageListTrim(init_message, graph);
    } else if (init_message->is_symbol_str(0, "length")) {
      return new MessageListLength(init_message, graph);
    } else {
      return new MessageListAppend(init_message, graph);
    }
  } else {
    return new MessageListAppend(init_message, graph);
  }
}

MessageListAppend::MessageListAppend(pd::Message *init_message, PdGraph *graph) : message::Object(2, 1, graph) {
  appendMessage = init_message->clone_on_heap();
}

MessageListAppend::~MessageListAppend() {
  appendMessage->free_message();
}

bool MessageListAppend::should_distribute_message_to_inlets() {
  return false;
}

void MessageListAppend::process_message(int inlet_index, pd::Message *message) {
  switch (inlet_index) {
    case 0: {
      // if the incoming message is a bang, then it is considered to be a list of length zero
      int numMessageElements = (!message->is_bang(0)) ? message->get_num_elements() : 0;
      int numAppendElements = appendMessage->get_num_elements();
      int numTotalElements = numMessageElements + numAppendElements;
      if (numTotalElements > 0) {
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(numTotalElements);
        outgoing_message->from_timestamp(message->get_timestamp(), numTotalElements);
        memcpy(outgoing_message->get_element(0), message->get_element(0), numMessageElements*sizeof(pd::message::Atom));
        memcpy(outgoing_message->get_element(numMessageElements), appendMessage->get_element(0), numAppendElements*sizeof(pd::message::Atom));
        send_message(0, outgoing_message);
      } else {
        pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
        outgoing_message->from_timestamp_and_bang(message->get_timestamp());
        send_message(0, outgoing_message);
      }
      break;
    }
    case 1: {
      if (message->is_bang(0)) {
        // bangs are considered a list of size zero
        appendMessage->free_message();
        pd::Message *message = PD_MESSAGE_ON_STACK(0);
        message->from_timestamp(0.0, 0);
        appendMessage = message->clone_on_heap();
      } else {
        appendMessage->free_message();
        appendMessage = message->clone_on_heap();
      }
      break;
    }
    default: {
      break;
    }
  }
}
