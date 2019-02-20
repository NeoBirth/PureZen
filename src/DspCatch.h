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

#ifndef _DSP_CATCH_H_
#define _DSP_CATCH_H_

#include "DspObject.h"

class DspThrow;

/**
 * [catch~ symbol]
 * Implements the receiver of a many-to-one audio connection.
 */
class DspCatch : public DspObject {
  
  public:
    static MessageObject *new_object(PdMessage *init_message, PdGraph *graph);
    DspCatch(PdMessage *init_message, PdGraph *graph);
    ~DspCatch();

    list<DspObject *> getProcessOrder();
  
    void add_throw(DspThrow *dspThrow);
    void removeThrow(DspThrow *dspThrow);
  
    const char *get_name() { return name; }
    static const char *get_object_label() { return "catch~"; }
    object::Type get_object_type() { return DSP_CATCH; }
    string toString();
  
  private:
    static void processNone(DspObject *dspObject, int fromIndex, int toIndex);
    static void processOne(DspObject *dspObject, int fromIndex, int toIndex);
    static void processMany(DspObject *dspObject, int fromIndex, int toIndex);
    
    char *name;
    list<DspThrow *> throw_list; // list of associated throw~ objects
};

#endif // _DSP_CATCH_H_
