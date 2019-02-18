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

#ifndef _DSP_RECEIVE_H_
#define _DSP_RECEIVE_H_

#include "DspObject.h"

/** [receive~ symbol], [r~ symbol] */
class DspReceive : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspReceive(PdMessage *init_message, PdGraph *graph);
    ~DspReceive();
    
    const char *getName();
    static const char *getObjectLabel();
    std::string toString();
  
    object::Type get_object_type();
  
    void process_message(int inlet_index, PdMessage *message);
  
    bool canSetBufferAtOutlet(unsigned int outlet_index);
  
  private:
    static void processSignal(DspObject *dspObject, int fromIndex, int toIndex);
  
    char *name;
};

inline const char *DspReceive::getName() {
  return name;
}

inline const char *DspReceive::getObjectLabel() {
  return "receive~";
}
  
inline object::Type DspReceive::get_object_type() {
  return DSP_RECEIVE;
}
  
inline bool DspReceive::canSetBufferAtOutlet(unsigned int outlet_index) {
  return false;
}

inline std::string DspReceive::toString() {
  return string(DspReceive::getObjectLabel()) + " " + string(name);
}

#endif // _DSP_RECEIVE_H_
