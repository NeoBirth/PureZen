/*
 *  Copyright 2009,2012 Reality Jockey, Ltd.
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

#ifndef _MESSAGE_RECEIVE_H_
#define _MESSAGE_RECEIVE_H_

#include "RemoteMessageReceiver.h"

/** [receive|r] */
class MessageReceive : public RemoteMessageReceiver {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageReceive(PdMessage *init_message, PdGraph *graph);
    ~MessageReceive();
    
    static const char *get_object_label();
    std::string toString();
    object::Type get_object_type();
  
    void receive_message(int inlet_index, PdMessage *message);
};

inline const char *MessageReceive::get_object_label() {
  return "receive";
}

inline std::string MessageReceive::toString() {
  return std::string(MessageReceive::get_object_label()) + " " + name;
}

inline object::Type MessageReceive::get_object_type() {
  return MESSAGE_RECEIVE;
}

#endif // _MESSAGE_RECEIVE_H_
