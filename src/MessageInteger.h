/*
 *  Copyright 2009, 2010 Reality Jockey, Ltd.
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

#ifndef _MESSAGE_INTEGER_H_
#define _MESSAGE_INTEGER_H_

#include "MessageObject.h"

/** [int], [int float] */
class MessageInteger : public MessageObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageInteger(PdMessage *init_message, PdGraph *graph);
    ~MessageInteger();

    static const char *getObjectLabel();
    std::string toString();

  private:
    void init(float constant);
    void process_message(int inlet_index, PdMessage *message);

    float constant;
};

inline const char *MessageInteger::getObjectLabel() {
  return "int";
};

inline std::string MessageInteger::toString() {
  return MessageInteger::getObjectLabel();
}

#endif // _MESSAGE_INTEGER_H_
