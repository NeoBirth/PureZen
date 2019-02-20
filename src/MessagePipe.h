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

#ifndef _MESSAGE_PIPE_H_
#define _MESSAGE_PIPE_H_

#include "MessageObject.h"

class MessagePipe : public MessageObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessagePipe(PdMessage *init_message, PdGraph *pdGraph);
    ~MessagePipe();
  
    static const char *get_object_label();
    std::string toString();
  
    bool shouldDistributeMessageToInlets();
  
    // override <code>sendMessage</code> in order to remove the message from the scheduled
    // message list as it is sent out.
    void sendMessage(int outlet_index, PdMessage *message);
    
  private:
    void process_message(int inlet_index, PdMessage *message);
  
    double delayMs; // the delay in milliseconds
    list<PdMessage *> scheduledMessagesList;
};

inline const char *MessagePipe::get_object_label() {
  return "pipe";
}

inline std::string MessagePipe::toString() {
  return MessagePipe::get_object_label();
}

#endif // _MESSAGE_PIPE_H_
