/*
 *  Copyright 2009,2010,2012 Reality Jockey, Ltd.
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

#ifndef _DSP_PHASOR_H_
#define _DSP_PHASOR_H_

#include "DspObject.h"

/** [phasor~], [phasor~ float] */
class DspPhasor : public DspObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspPhasor(PdMessage *init_message, PdGraph *graph); // and Phasorillator of default zero frequency
    ~DspPhasor();

    void onInletConnectionUpdate(unsigned int inlet_index);
    static const char *getObjectLabel();
    std::string toString();

  private:
    static void processSignal(DspObject *dspObject, int fromIndex, int toIndex);
    static void processScalar(DspObject *dspObject, int fromIndex, int toIndex);
    void process_message(int inlet_index, PdMessage *message);
  
    float frequency;
  
    #if __SSE3__
    __m64 inc; // the amount by which to increment indicies every step
    __m64 indicies; // the table lookup indicies
    #endif
};

inline const char *DspPhasor::getObjectLabel() {
  return "phasor~";
}

#endif // _DSP_PHASOR_H_
