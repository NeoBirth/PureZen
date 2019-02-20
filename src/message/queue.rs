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

#include "OrderedMessageQueue.h"

OrderedMessageQueue::OrderedMessageQueue() {
  orderedMessageQueue = list<ObjectMessageConnection>();
}

OrderedMessageQueue::~OrderedMessageQueue() {
  // destroy all remaining inserted messages
  for (list<ObjectMessageConnection>::iterator it = orderedMessageQueue.begin();
       it != orderedMessageQueue.end(); ++it) {
    ObjectMessageConnection omlPair = *it;
    omlPair.second.first->freeMessage();
  }
}

void OrderedMessageQueue::insertMessage(message::Object *message_obj, int outlet_index, pd::Message *message) {
  ObjectMessageConnection omlPair = Connection::new(message_obj, Connection::new(message, outlet_index));
  for (list<ObjectMessageConnection>::iterator it = orderedMessageQueue.begin();
       it != orderedMessageQueue.end(); ++it) {
    if (message->get_timestamp() < it->second.first->get_timestamp()) {
      orderedMessageQueue.insert(it, omlPair);
      return;
    }
  }
  orderedMessageQueue.push_back(omlPair); // insert at end
}

void OrderedMessageQueue::removeMessage(message::Object *message_obj, int outlet_index, pd::Message *message) {
  for (list<ObjectMessageConnection>::iterator it = orderedMessageQueue.begin();
       it != orderedMessageQueue.end(); ++it) {
    ObjectMessageConnection omlPair = *it;
    if (omlPair.first == message_obj &&
        omlPair.second.first == message &&
        omlPair.second.second == outlet_index) {
      orderedMessageQueue.erase(it);
      return;
    }
  }
}

ObjectMessageConnection OrderedMessageQueue::peek() {
  return orderedMessageQueue.front();
}

void OrderedMessageQueue::pop() {
  orderedMessageQueue.pop_front();
}

bool OrderedMessageQueue::empty() {
  return orderedMessageQueue.empty();
}
