/*
 *  Copyright 2010,2011 Reality Jockey, Ltd.
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

#include "MessageText.h"

message::Object *MessageText::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageText(init_message, graph);
}

MessageText::MessageText(pd::Message *init_message, PdGraph *graph) : message::Object(0, 0, graph) {
  comment = init_message->is_symbol(0) ? utils::copy_string(init_message->get_symbol(0)) : NULL;
}

MessageText::~MessageText() {
  free(comment);
}
