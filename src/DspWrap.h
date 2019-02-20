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

#ifndef _DSP_WRAP_H_
#define _DSP_WRAP_H_

#include "DspObject.h"

/** [wrap~] */
class DspWrap : public DspObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspWrap(PdMessage *init_message, PdGraph *graph);
    ~DspWrap();

    static const char *get_object_label();
    std::string toString();
  
  private:
    static void processSignal(DspObject *dspObject, int fromIndex, int toIndex);
};

inline std::string DspWrap::toString() {
  return DspWrap::get_object_label();
}

inline const char *DspWrap::get_object_label() {
  return "wrap~";
}

#endif // _DSP_WRAP_H_
