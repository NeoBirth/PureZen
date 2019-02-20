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

#ifndef _DSP_LINE_H_
#define _DSP_LINE_H_

#include "DspObject.h"

/** [line~] */
class DspLine : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspLine(PdMessage *init_message, PdGraph *graph);
    ~DspLine();
  
    static const char *get_object_label();
    std::string toString();
  
  private:
    void process_message(int inlet_index, PdMessage *message);
    void processDspWithIndex(int fromIndex, int toIndex);
  
    float target;
    float slope;
    float numSamplesToTarget;
    float lastOutputSample;
};

inline const char *DspLine::get_object_label() {
  return "line~";
}

inline std::string DspLine::toString() {
  return DspLine::get_object_label();
}

#endif // _DSP_LINE_H_
