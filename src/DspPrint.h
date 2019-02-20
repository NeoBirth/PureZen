/*
 *  Copyright 2010 Reality Jockey, Ltd.
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

#ifndef _DSP_PRINT_H_
#define _DSP_PRINT_H_

#include "DspObject.h"

/** [print~], [print~ symbol] */
class DspPrint : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspPrint(PdMessage *init_message, PdGraph *graph);
    ~DspPrint();

    static const char *get_object_label();
    std::string toString();
    
  private:
    void process_message(int inlet_index, PdMessage *message);
    void processDspWithIndex(int fromIndex, int toIndex);
  
    char *name;
};

inline const char *DspPrint::get_object_label() {
  return "print~";
}

inline std::string DspPrint::toString() {
  return std::string(DspPrint::get_object_label()) + " " + name;
}

#endif // _DSP_PRINT_H_
