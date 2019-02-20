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

#ifndef _MESSAGE_TABLE_WRITE_H_
#define _MESSAGE_TABLE_WRITE_H_

#include "MessageObject.h"
#include "TableReceiverInterface.h"

/** [tabwrite name] */
class MessageTableWrite : public MessageObject, public TableReceiverInterface {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageTableWrite(PdMessage *init_message, PdGraph *graph);
    ~MessageTableWrite();
    
    static const char *get_object_label();
    std::string toString();
    object::Type get_object_type();
    
    char *get_name();
    void set_table(MessageTable *table);
    bool shouldDistributeMessageToInlets();
    
  private:
    void process_message(int inlet_index, PdMessage *message);
    
    char *name;
    MessageTable *table;
    int index;
};

inline const char *MessageTableWrite::get_object_label() {
  return "tabwrite";
}

inline object::Type MessageTableWrite::get_object_type() {
  return MESSAGE_TABLE_WRITE;
}

inline std::string MessageTableWrite::toString() {
  return std::string(MessageTableWrite::get_object_label()) + " " + name;
}

#endif // _MESSAGE_TABLE_WRITE_H_
