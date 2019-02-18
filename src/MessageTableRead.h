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

#ifndef _MESSAGE_TABLE_READ_H_
#define _MESSAGE_TABLE_READ_H_

#include "MessageObject.h"
#include "TableReceiverInterface.h"

/** [tabread name] */
class MessageTableRead : public MessageObject, public TableReceiverInterface {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageTableRead(PdMessage *init_message, PdGraph *graph);
    ~MessageTableRead();
    
    static const char *getObjectLabel();
    std::string toString();
    object::Type get_object_type();
    
    char *getName();
    void setTable(MessageTable *table);
    
  private:
    void process_message(int inlet_index, PdMessage *message);
  
    char *name;
    MessageTable *table;
};

inline const char *MessageTableRead::getObjectLabel() {
  return "tabread";
}

inline object::Type MessageTableRead::get_object_type() {
  return MESSAGE_TABLE_READ;
}

inline std::string MessageTableRead::toString() {
  return std::string(MessageTableRead::getObjectLabel()) + " " + name;
}

#endif // _MESSAGE_TABLE_READ_H_
