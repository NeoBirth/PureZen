/*
 *  Copyright 2009,2011,2012 Reality Jockey, Ltd.
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

#ifndef _DSP_ADC_H_
#define _DSP_ADC_H_

#include "DspObject.h"

class DspAdc : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspAdc(PdGraph *graph);
    ~DspAdc();
  
    static const char *get_object_label();
    std::string toString();
  
    // This object doesn't do anything with audio, it only provides buffers. It should not be included
    // in the dsp list
    bool doesProcessAudio() { return false; }
  
    bool canSetBufferAtOutlet(unsigned int outlet_index) { return false; }
  
    float *get_dsp_buffer_at_outlet(int outlet_index);
};

inline const char *DspAdc::get_object_label() {
  return "adc~";
}

inline std::string DspAdc::toString() {
  return DspAdc::get_object_label();
}

#endif // _DSP_ADC_H_
