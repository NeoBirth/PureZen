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

#ifndef _DSP_LOG_H_
#define _DSP_LOG_H_

#include "DspObject.h"

/** [log~], [log~ float] */
class DspLog : public DspObject {
    
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspLog(PdMessage *init_message, PdGraph *graph);
    ~DspLog();
  
    static const char *getObjectLabel();
    std::string toString();
  
    void onInletConnectionUpdate(unsigned int inlet_index);
  
  private:
    static void processSignal(DspObject *dspObject, int fromIndex, int toIndex);
    static void processScalar(DspObject *dspObject, int fromIndex, int toIndex);
    void process_message(int inlet_index, PdMessage *message);
  
    // this implementation is reproduced from http://www.musicdsp.org/showone.php?id=91
    inline float log2Approx(float x) {
      int y = (*(int *)&x); // input is assumed to be positive
      return (((y & 0x7f800000)>>23)-0x7f)+(y & 0x007fffff)/(float)0x800000;
    }
  
    float invLog2Base; // 1/log2(base)
};

inline std::string DspLog::toString() {
  return DspLog::getObjectLabel();
}

inline const char *DspLog::getObjectLabel() {
  return "log~";
}

#endif // _DSP_LOG_H_
