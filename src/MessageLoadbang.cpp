/*
 *  Copyright 2009,2011 Reality Jockey, Ltd.
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

#include "MessageLoadbang.h"
#include "PdGraph.h"

message::Object *MessageLoadbang::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageLoadbang(graph);
}

MessageLoadbang::MessageLoadbang(PdGraph *graph) : message::Object(0, 1, graph) {
  pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
  outgoing_message->from_timestamp_and_bang(0.0);
  graph->schedule_message(this, 0, outgoing_message);
}

MessageLoadbang::~MessageLoadbang() {
  // nothing to do
}
