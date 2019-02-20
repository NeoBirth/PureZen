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

#include "ArrayArithmetic.h"
#include "BufferPool.h"
#include "DspCatch.h"
#include "DspThrow.h"
#include "pd::Context.h"
#include "PdGraph.h"


message::Object *DspCatch::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspCatch(init_message, graph);
}

DspCatch::DspCatch(pd::Message *init_message, PdGraph *graph) : DspObject(0, 0, 0, 1, graph) {
  if (init_message->is_symbol(0)) {
    name = utils::copy_string(init_message->get_symbol(0));
  } else {
    name = NULL;
    graph->print_err("catch~ must be initialised with a name.");
  }
  process_function = &processNone;
}

DspCatch::~DspCatch() {
  free(name);
}

string DspCatch::toString() {
  char str[snprintf(NULL, 0, "%s %s", get_object_label(), name)+1];
  snprintf(str, sizeof(str), "%s %s", get_object_label(), name);
  return string(str);
}

void DspCatch::add_throw(DspThrow *dspThrow) {
  if (!strcmp(dspThrow->get_name(), name)) { // make sure that the throw~ really does match this catch~
    throw_list.push_back(dspThrow); // NOTE(mhroth): no dupicate detection
    
    // update the process function
    switch (throw_list.size()) {
      case 0: process_function = &processNone; break;
      case 1: process_function = &processOne; break;
      default: process_function = &processMany; break;
    }
  }
}

void DspCatch::removeThrow(DspThrow *dspThrow) {
  if (!strcmp(dspThrow->get_name(), name)) {
    throw_list.remove(dspThrow);
    
    switch (throw_list.size()) {
      case 0: process_function = &processNone; break;
      case 1: process_function = &processOne; break;
      default: process_function = &processMany; break;
    }
  }
}

void DspCatch::processNone(DspObject *dspObject, int fromIndex, int toIndex) {
  DspCatch *d = reinterpret_cast<DspCatch *>(dspObject);
  memset(d->dspBufferAtOutlet[0], 0, toIndex*sizeof(float));
}

void DspCatch::processOne(DspObject *dspObject, int fromIndex, int toIndex) {
  DspCatch *d = reinterpret_cast<DspCatch *>(dspObject);
  DspThrow *dspThrow = d->throw_list.front();
  memcpy(d->dspBufferAtOutlet[0], dspThrow->getBuffer(), toIndex*sizeof(float));
}

// process at least two throw~s
void DspCatch::processMany(DspObject *dspObject, int fromIndex, int toIndex) {
  DspCatch *d = reinterpret_cast<DspCatch *>(dspObject);
  list<DspThrow *>::iterator it = d->throw_list.begin();
  ArrayArithmetic::add((*it++)->getBuffer(), (*it++)->getBuffer(), d->dspBufferAtOutlet[0], 0, toIndex);
  while (it != d->throw_list.end()) {
    ArrayArithmetic::add(d->dspBufferAtOutlet[0], (*it++)->getBuffer(), d->dspBufferAtOutlet[0],
        0, toIndex);
  };
}

// catch objects should be processed after their corresponding throw object even though
// there is no connection between them
list<DspObject *> DspCatch::get_process_order() {
  if (is_ordered) {
    // if this object has already been ordered, then move on
    return list<DspObject *>();
  } else {
    is_ordered = true;
    list<DspObject *> processList;
    
    for (std::list<DspThrow *>::iterator throwIt = throw_list.begin(); throwIt != throw_list.end(); ++throwIt) {
      list<DspObject *> parentProcessList = (*throwIt)->get_process_order();
      // combine the process lists
      processList.splice(processList.end(), parentProcessList);
    }
    
    // set the outlet buffers
    for (int i = 0; i < getNumDspOutlets(); i++) {
      if (canSetBufferAtOutlet(i)) {
        float *buffer = graph->getContext()->get_buffer_pool()->getBuffer(outgoingDspConnections[i].size());
        setDspBufferAtOutlet(buffer, i);
      }
    }
    
    processList.push_back(this);
    return processList;
  }
}

