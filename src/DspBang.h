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

#ifndef _DSP_BANG_H_
#define _DSP_BANG_H_

#include "DspObject.h"

/** [bang~] */
class DspBang : public DspObject {

  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspBang(PdMessage *init_message, PdGraph *graph);
    ~DspBang();

    static const char *getObjectLabel();
    std::string toString();

    connection::Type get_connection_type(int outlet_index) { return MESSAGE; }

  private:
    static void processDsp(DspObject *dspObject, int fromIndex, int toIndex);
};

inline std::string DspBang::toString() {
  return DspBang::getObjectLabel();
}

inline const char *DspBang::getObjectLabel() {
  return "bang~";
}

#endif // _DSP_BANG_H_
