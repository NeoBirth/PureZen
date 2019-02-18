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

#ifndef _DSP_TABLE_READ4_H_
#define _DSP_TABLE_READ4_H_

#include "DspObject.h"
#include "TableReceiverInterface.h"

/**
 * [tabread4~ name]
 * This is a linear-interpolating table reader.
 */
class DspTableRead4 : public DspObject, public TableReceiverInterface {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspTableRead4(PdMessage *init_message, PdGraph *graph);
    ~DspTableRead4();
  
    static const char *getObjectLabel();
    std::string toString();
    object::Type get_object_type();
  
    char *getName();
    void setTable(MessageTable *table);
  
  private:
    void process_message(int inlet_index, PdMessage *message);
    void processDspWithIndex(int fromIndex, int toIndex);
  
    float offset;
    char *name;
    MessageTable *table;
};

inline std::string DspTableRead4::toString() {
  return DspTableRead4::getObjectLabel();
}

inline const char *DspTableRead4::getObjectLabel() {
  return "tabread4~";
}

inline object::Type DspTableRead4::get_object_type() {
  return DSP_TABLE_READ4;
}

inline char *DspTableRead4::getName() {
  return name;
}

#endif // _DSP_TABLE_READ4_H_
