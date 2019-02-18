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

#ifndef _MESSAGE_SEND_H_
#define _MESSAGE_SEND_H_

#include "MessageObject.h"

/** [send|s] */
class MessageSend : public MessageObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageSend(PdMessage *init_message, PdGraph *graph);
    ~MessageSend();
    
    static const char *getObjectLabel();
    std::string toString();
    object::Type get_object_type();
  
    void receiveMessage(int inlet_index, PdMessage *message);
  
  private:
    char *name;
  
};

inline const char *MessageSend::getObjectLabel() {
  return "send";
}

inline std::string MessageSend::toString() {
  return std::string(MessageSend::getObjectLabel()) + " " + name;
}

inline object::Type MessageSend::get_object_type() {
  return MESSAGE_SEND;
}

#endif // _MESSAGE_SEND_H_
