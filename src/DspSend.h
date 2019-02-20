/*
 *  Copyright 2010,2012 Reality Jockey, Ltd.
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

#ifndef _DSP_SEND_H_
#define _DSP_SEND_H_

#include "DspObject.h"

/** [send~ symbol], [s~ symbol] */
class DspSend : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspSend(PdMessage *init_message, PdGraph *graph);
    ~DspSend();
    
    const char *get_name();
    static const char *get_object_label();
    std::string toString();
  
    object::Type get_object_type();
    
  private:
    static void processSignal(DspObject *dspObject, int fromIndex, int toIndex);
  
    char *name;
};

inline std::string DspSend::toString() {
  return string(DspSend::get_object_label()) + " " + string(name);
}

inline const char *DspSend::get_name() {
  return name;
}

inline const char *DspSend::get_object_label() {
  return "send~";
}

inline object::Type DspSend::get_object_type() {
  return DSP_SEND;
}

#endif // _DSP_SEND_H_
