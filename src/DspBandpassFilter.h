/*
 *  Copyright 2009,2010,2011,2012 Reality Jockey, Ltd.
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

#ifndef _DSP_BAND_PASS_FILTER_H_
#define _DSP_BAND_PASS_FILTER_H_

#include "DspFilter.h"

/**
 * [bp float float]
 * Implements the bp~ (2-pole) dsp object.
 */
class DspBandpassFilter : public DspFilter {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspBandpassFilter(PdMessage *init_message, PdGraph *graph);
    ~DspBandpassFilter();
  
    static const char *get_object_label();
    std::string toString();
    
  private:
    void process_message(int inlet_index, PdMessage *message);
    void calcFiltCoeff(float fc, float q);

    float fc; // the center frequency
    float q;
};

inline std::string DspBandpassFilter::toString() {
  return DspBandpassFilter::get_object_label();
}

inline const char *DspBandpassFilter::get_object_label() {
  return "bp~";
}

#endif // _DSP_BAND_PASS_FILTER_H_
