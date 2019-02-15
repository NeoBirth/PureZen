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

#ifndef _PD_MESSAGE_ELEMENT_H_
#define _PD_MESSAGE_ELEMENT_H_

#include <string.h>
#include "MessageElementType.h"

#define SYMBOL_BUFFER_LENGTH 56

/** Implements a Pd message element. */
class MessageElement {

  public:
    MessageElement();
    MessageElement(float newConstant);
    MessageElement(char *newSymbol);
    ~MessageElement();

    message::element::Type getType();
    bool is_float();
    bool is_smbol();
    bool is_bang();
    bool is_symbol_anything_or_a();
    bool is_symbol_bang_or_b();
    bool is_symbol_float_or_f();
    bool is_symbol_list_or_l();
    bool isSymbolSymbolOrS();

    void set_float(float constant);
    float get_float();

    void set_symbol(char *symbol);
    char *get_symbol();

    void set_bang();

    /** Sets the type of this element to ANYTHING. What that means is otherwise undefined. */
    void set_anything();

    /** Sets the type of this element to LIST. What that means is otherwise undefined. */
    void set_list();

    MessageElement *copy();

    bool equals(MessageElement *messageElement);

  private:
    message::element::Type currentType;
    float constant;
    char symbol[SYMBOL_BUFFER_LENGTH];
};

#endif // _PD_MESSAGE_ELEMENT_H_
