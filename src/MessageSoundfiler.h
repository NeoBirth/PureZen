/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#ifndef _MESSAGE_SOUNDFILER_H_
#define _MESSAGE_SOUNDFILER_H_

#include "MessageObject.h"

/** [soundfiler~] */
class MessageSoundfiler : public MessageObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageSoundfiler(PdMessage *init_message, PdGraph *pdGraph);
    ~MessageSoundfiler();
  
    static const char *get_object_label();
    std::string toString();
    
  private:
    void process_message(int inlet_index, PdMessage *message);
};

inline const char *MessageSoundfiler::get_object_label() {
  return "soundfiler";
}

inline std::string MessageSoundfiler::toString() {
  return MessageSoundfiler::get_object_label();
}

#endif // _MESSAGE_SOUNDFILER_H_
