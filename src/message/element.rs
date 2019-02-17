//
// Copyright © 2009-2019 NeoBirth Developers, Reality Jockey, Ltd.
//
// This file is part of PureZen (a fork of ZenGarden)
//
// PureZen is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PureZen is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with PureZen.  If not, see <http://www.gnu.org/licenses/>.
//

//! Message Elements: Components of a Pure Data message

/// Types of message elements
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Type {
  /// Any value (placeholder for uninitialized values)
  // TODO: eliminate this state by always initializing all message elements
  ANYTHING,

  /// `bang`: Typically used to trigger an object to perform an action.
  BANG,

  /// `float`: Single-precision floating point value
  FLOAT,

  /// `list`: Lists of other atoms
  LIST,

  /// `symbol`: Symbol messages (i.e. keywords)
  SYMBOL,
}

MessageElement::MessageElement() {
  constant = 0.0f;
  memset(symbol, 0, SYMBOL_BUFFER_LENGTH);
  set_bang();
}

MessageElement::MessageElement(float constant) {
  memset(symbol, 0, SYMBOL_BUFFER_LENGTH);
  set_float(constant);
}

MessageElement::MessageElement(char *newSymbol) {
  constant = 0.0f;
  memset(symbol, 0, SYMBOL_BUFFER_LENGTH);
  set_symbol(newSymbol);
}


MessageElement::~MessageElement() {
  // nothing to do
}

message::element::Type MessageElement::get_type() {
  return currentType;
}

bool MessageElement::is_float() {
  return (currentType == FLOAT);
}

bool MessageElement::is_symbol() {
  return (currentType == SYMBOL);
}

bool MessageElement::is_symbol_anything_or_a() {
  return (currentType == SYMBOL && (strcmp(symbol, "anything") == 0 || strcmp(symbol, "a") == 0));
}

bool MessageElement::is_symbol_bang_or_b() {
  return (currentType == SYMBOL && (strcmp(symbol, "bang") == 0 || strcmp(symbol, "b") == 0));
}

bool MessageElement::is_symbol_float_or_f() {
  return (currentType == SYMBOL && (strcmp(symbol, "float") == 0 || strcmp(symbol, "f") == 0));
}

bool MessageElement::is_symbol_list_or_l() {
  return (currentType == SYMBOL && (strcmp(symbol, "list") == 0 || strcmp(symbol, "l") == 0));
}

bool MessageElement::is_symbol_symbol_or_s() {
  return (currentType == SYMBOL && (strcmp(symbol, "symbol") == 0 || strcmp(symbol, "s") == 0));
}

bool MessageElement::is_bang() {
  return (currentType == BANG);
}

void MessageElement::set_float(float newConstant) {
  constant = newConstant;
  currentType = FLOAT;
}

float MessageElement::get_float() {
  return constant;
}

void MessageElement::set_symbol(char *newSymbol) {
  if (strlen(newSymbol) < SYMBOL_BUFFER_LENGTH-1) {
    strcpy(symbol, newSymbol);
  } else {
    // This should never ever ever happen.
    printf("A symbol added to a message exceeds the symbol buffer length: strlen(%s) > %i",
        newSymbol, SYMBOL_BUFFER_LENGTH-1);
  }
  currentType = SYMBOL;
}

char *MessageElement::get_symbol() {
  return symbol;
}

void MessageElement::set_bang() {
  currentType = BANG;
}

void MessageElement::set_anything() {
  currentType = ANYTHING;
}

void MessageElement::setList() {
  currentType = LIST;
}

MessageElement *MessageElement::copy() {
  switch (currentType) {
    case FLOAT: {
      return new MessageElement(constant);
    }
    case SYMBOL: {
      return new MessageElement(symbol);
    }
    case BANG: {
      return new MessageElement();
    }
    default: {
      return NULL;
    }
  }
}

bool MessageElement::equals(MessageElement *messageElement) {
  if (messageElement->get_type() == currentType) {
    switch (currentType) {
      case FLOAT: {
        return (constant == messageElement->get_float());
      }
      case SYMBOL: {
        return (strcmp(symbol, messageElement->get_symbol()) == 0);
      }
      case BANG: {
        return true;
      }
      default: {
        return false;
      }
    }
  } else {
    return false;
  }
}
