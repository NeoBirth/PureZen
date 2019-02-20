/*
 *  Copyright 2010,2011,2012 Reality Jockey, Ltd.
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

#ifndef _DSP_INLET_H_
#define _DSP_INLET_H_

#include "DspObject.h"

class PdGraph;

/** [inlet~] */
/* <code>DspInlet</code> uses much the same strategy of buffer replacement as <code>DspOutlet</code>.
 * In this case, the parent-graph's inlet buffer replaces this object's outlet buffer. Thus, when
 * the parent graph fills its inlet buffer, this object's outlet buffer is immediately filled
 * and no further computations must be done.
 */
class DspInlet : public DspObject {
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspInlet(PdGraph *graph);
    ~DspInlet();
  
    static const char *get_object_label();
    std::string toString();
    object::Type get_object_type();
  
    list<DspObject *> getProcessOrder();
    list<DspObject *> getProcessOrderFromInlet();
  
    // [inlet~] does nothing with audio
    bool doesProcessAudio();
  
    void set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index);
    bool canSetBufferAtOutlet(unsigned int outlet_index);
    float *get_dsp_buffer_at_outlet(int outlet_index);
};

inline bool DspInlet::canSetBufferAtOutlet(unsigned int outlet_index) {
  return false;
}

inline bool DspInlet::doesProcessAudio() {
  return false;
}

inline std::string DspInlet::toString() {
  return DspInlet::get_object_label();
}

inline const char *DspInlet::get_object_label() {
  return "inlet~";
}

inline object::Type DspInlet::get_object_type() {
  return DSP_INLET;
}

#endif // _DSP_INLET_H_
