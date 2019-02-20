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

#ifndef _DSP_OUTLET_H_
#define _DSP_OUTLET_H_

#include "DspObject.h"

class PdGraph;

/** [oulet~] */
/*
 * <code>DspOutlet</code> increases its efficiency (of making its input appear at the output of the
 * parent graph) by replacing its own inlet buffer as the outlet buffer of its parent-graph. In this
 * way, when audio streams are implicitly added at the outlet object's inlet, the result
 * automatically appears at the outlet buffer of the parent graph. Superfluous calls to
 * <code>memcpy()</code> are avoided.
 */
class DspOutlet : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspOutlet(PdGraph *graph);
    ~DspOutlet();
  
    static const char *get_object_label();
    std::string toString();
    object::Type get_object_type();

    bool isLeafNode();
  
    // [outlet~] does nothing with audio
    bool doesProcessAudio();
  
    float *get_dsp_buffer_at_outlet(int outlet_index);
  
    void set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index);
    bool canSetBufferAtOutlet(unsigned int outlet_index);
};

inline const char *DspOutlet::get_object_label() {
  return "outlet~";
}

inline object::Type DspOutlet::get_object_type() {
  return DSP_OUTLET;
}

inline bool DspOutlet::isLeafNode() {
  return true;
}
  
inline bool DspOutlet::doesProcessAudio() {
  return false;
}

inline std::string DspOutlet::toString() {
  return DspOutlet::get_object_label();
}

inline bool DspOutlet::canSetBufferAtOutlet(unsigned int outlet_index) {
  return false;
}

#endif // _DSP_OUTLET_H_
