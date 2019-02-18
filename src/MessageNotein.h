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

#ifndef _MESSAGE_NOTEIN_H_
#define _MESSAGE_NOTEIN_H_

#include "RemoteMessageReceiver.h"

/** [notein], [notein float] */
class MessageNotein : public RemoteMessageReceiver {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    MessageNotein(PdMessage *init_message, PdGraph *graph);
    ~MessageNotein();
    
    static const char *getObjectLabel();
    std::string toString();
    object::Type get_object_type();
  
    /** Returns the zero-indexed midi channel which this object outputs. -1 if omni. */
    int getChannel();
    
  private:
    void process_message(int inlet_index, PdMessage *message);
    bool isOmni();

    int channel;
};

inline const char *MessageNotein::getObjectLabel() {
  return "notein";
}

inline std::string MessageNotein::toString() {
  return MessageNotein::getObjectLabel();
}

inline object::Type MessageNotein::get_object_type() {
  return MESSAGE_NOTEIN;
}

#endif // _MESSAGE_NOTEIN_H_
