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

#include "MessagePrint.h"
#include "PdGraph.h"

MessageObject *MessagePrint::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessagePrint(initMessage, graph);
}

MessagePrint::MessagePrint(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 0, graph) {
  if (initMessage->is_symbol(0)) {
    name = initMessage->is_symbol_str(0, "-n") ? NULL : utils::copy_string(initMessage->get_symbol(0));
  } else {
    name = utils::copy_string((char *) "print");
  }
}

MessagePrint::~MessagePrint() {
  free(name);
}

void MessagePrint::processMessage(int inletIndex, PdMessage *message) {
  char *out = message->toString();
  if (name != NULL) {
    graph->printStd("[@ %.3fms] %s: %s", message->get_timestamp(), name, out);
  } else {
    graph->printStd("[@ %.3fms] %s", message->get_timestamp(), out);
  }
  free(out);
}
