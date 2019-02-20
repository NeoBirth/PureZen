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

#include "BufferPool.h"
#include "DspOutlet.h"
#include "PdGraph.h"

message::Object *DspOutlet::new_object(pd::Message *init_message, PdGraph *graph) {
  return new DspOutlet(graph);
}

DspOutlet::DspOutlet(PdGraph *graph) : DspObject(0, 1, 0, 1, graph) {
  // nothing to do
}

DspOutlet::~DspOutlet() {
  // nothing to do
}

float *DspOutlet::get_dsp_buffer_at_outlet(int outlet_index) {
  return (dspBufferAtInlet[0] == NULL) ? graph->get_buffer_pool()->get_zero_buffer() : dspBufferAtInlet[0];
}

void DspOutlet::set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index) {
  DspObject::set_dsp_buffer_at_inlet(buffer, inlet_index);
  
  // additionally reserve buffer to account for outgoing connections
  graph->get_buffer_pool()->reserveBuffer(buffer, outgoingDspConnections[0].size());
  
  // when the dsp buffer updates at a given inlet, inform all receiving objects
  list<Connection> dspConnections = outgoingDspConnections[0];
  for (list<Connection>::iterator it = dspConnections.begin(); it != dspConnections.end(); ++it) {
    Connection letPair = *it;
    DspObject *dspObject = reinterpret_cast<DspObject *>(letPair.first);
    dspObject->set_dsp_buffer_at_inlet(dspBufferAtInlet[0], letPair.second);
  }
}
