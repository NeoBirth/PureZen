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

#ifndef _MESSAGE_MESSAGE_BOX_H_
#define _MESSAGE_MESSAGE_BOX_H_

#include "MessageObject.h"

typedef std::pair<char *, PdMessage *> MessageNamedDestination;

/** Implements the functionality of Pd's message box. */
class MessageMessageBox : public MessageObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageMessageBox(char *initString, PdGraph *graph);
    ~MessageMessageBox();
  
    static const char *getObjectLabel();
    std::string toString();
  
  private:
    void process_message(int inlet_index, PdMessage *message);
  
    vector<PdMessage *> localMessageList;
    vector<MessageNamedDestination> remoteMessageList;
};

inline const char *MessageMessageBox::getObjectLabel() {
  return "msg";
}

inline std::string MessageMessageBox::toString() {
  return MessageMessageBox::getObjectLabel();
}

#endif // _MESSAGE_MESSAGE_BOX_H_
