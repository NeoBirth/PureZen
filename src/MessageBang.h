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

#ifndef _MESSAGE_BANG_H_
#define _MESSAGE_BANG_H_

#include "MessageObject.h"

/** [bang], [bng] */
class MessageBang : public MessageObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageBang(PdGraph *graph);
    ~MessageBang();

    static const char *getObjectLabel();
    std::string toString();

  private:
    void process_message(int inlet_index, PdMessage *message);
};

inline std::string MessageBang::toString() {
  return MessageBang::getObjectLabel();
}

inline const char *MessageBang::getObjectLabel() {
  return "bang";
}

#endif // _MESSAGE_BANG_H_
