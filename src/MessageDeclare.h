/*
 *  Copyright 2010 Reality Jockey, Ltd.
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

#ifndef _MESSAGE_DECLARE_H_
#define _MESSAGE_DECLARE_H_

#include "MessageObject.h"

/**
 * [declare -path symbol], [declare -stdpath symbol], [declare -lib symbol], [declare -stdlib symbol]
 * This object does absolutely nothing. Declare objects have an effect by adding a "#X declare" statement
 * to the start of the netlist, rather than doing anything themselves. However, they must be added
 * to the node list as they are indexed.
 */
class MessageDeclare : public MessageObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageDeclare(PdMessage *init_message, PdGraph *graph);
    ~MessageDeclare();
    
    static const char *getObjectLabel();
    std::string toString();
    
  private:
    char *target;
    char *argument;
};

inline const char *MessageDeclare::getObjectLabel() {
  return "declare";
}

inline std::string MessageDeclare::toString() {
  return MessageDeclare::getObjectLabel();
}

#endif // _MESSAGE_DECLARE_H_
