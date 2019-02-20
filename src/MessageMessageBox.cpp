/*
 *  Copyright 2009,2010,2011 Reality Jockey, Ltd.
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

#include "MessageMessageBox.h"
#include "PdGraph.h"

message::Object *MessageMessageBox::new_object(pd::Message *initString, PdGraph *graph) {
  return new MessageMessageBox(initString->get_symbol(0), graph);
}

/*
 * The message box is overloaded with many kinds of functionality.
 * A) The simplest case is one where only one message is specified, including a list of primitives
 * which should be included in one message. The list may also include variable indicies (in the form
 * of, e.g. $1, $2, etc.) which refer to those locations in the incoming message which triggers
 * the message box.
 * B) A slightly more complicated case is where several messages in the form of A) are separated
 * by a comma (','). Each of the messages is processed and sent independently from the message box
 * when it is triggered.
 * C) The most complex case is where messages in the form of A) are separated by a semicolon (';').
 * The first symbol is the name of a message receiver. The remainder of the string is converted
 * into a message.
 */
MessageMessageBox::MessageMessageBox(char *initString, PdGraph *graph) : message::Object(1, 1, graph) {
  // parse the entire initialisation string
  vector<string> messageInitListAll = utils::tokenize_string(initString, "\\;");
  
  // parse the first "message" for individual messages that should be sent from the outlet 
  vector<string> messageInitList = utils::tokenize_string((char *) messageInitListAll[0].c_str(), "\\,");
  for (int i = 0; i < messageInitList.size(); i++) {
    string initString = messageInitList[i];
    int maxElements = (initString.size()/2)+1;
    // NOTE(mhroth): though this is alloca is in a for loop, it is not expected that the compiler
    // will do anything funny, like unrolling the loop, thereby causing unexpected stack overflows
    pd::Message *message = PD_MESSAGE_ON_STACK(maxElements);
    // utils::tokenize_string does not remove the trailing ";" from the
    // original string. We should not process it because it will result in an empty message. 
    if (strcmp(initString.c_str(), ";") != 0) {
      char str[initString.size()+1]; strcpy(str, initString.c_str());
      message->from_string(0.0, maxElements, str);
      localMessageList.push_back(message->clone_on_heap());
    }
  }
  
  // parse the remainder of the init list for all remote messages
  for (int i = 1; i < messageInitListAll.size(); i++) {
    string initString = messageInitListAll[i];
    if (strcmp(initString.c_str(), ";") != 0) {
      // trim any leading spaces. This will otherwise message up the parsing
      initString = string(initString, initString.find_first_not_of(" "));
      
      string name = string(initString, 0, initString.find(" "));
      string messageString = string(initString, initString.find(" ")+1);
      int maxElements = (messageString.size()/2)+1;
      pd::Message *message = PD_MESSAGE_ON_STACK(maxElements);
      char str[messageString.size()+1]; strcpy(str, messageString.c_str());
      message->from_string(0.0, maxElements, str);
      MessageNamedDestination namedDestination = 
          Connection::new(utils::copy_string(name.c_str()), message->clone_on_heap());
      remoteMessageList.push_back(namedDestination);
    }
  }
}

MessageMessageBox::~MessageMessageBox() {
  // delete the message list and all of the messages in it
  for (int i = 0; i < localMessageList.size(); i++) {
    localMessageList.at(i)->free_message();
  }
  
  // delete the remote message list
  for (int i = 0; i < remoteMessageList.size(); i++) {
    MessageNamedDestination namedDestination = remoteMessageList.at(i);
    free(namedDestination.first);
    namedDestination.second->free_message();
  }
}

void MessageMessageBox::process_message(int inlet_index, pd::Message *message) {
#define RES_BUFFER_LENGTH 64
  char resolvedName[RES_BUFFER_LENGTH]; // resolution buffer for named destination
  
  // NOTE(mhroth): if any message has more than 64 elements, that's very bad
  pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(64);
  
  // send local messages
  for (int i = 0; i < localMessageList.size(); i++) {
    pd::Message *messageTemplate = localMessageList.at(i);
    int numElements = messageTemplate->get_num_elements();
    outgoing_message->from_timestamp(message->get_timestamp(), numElements);
    memcpy(outgoing_message->get_element(0), messageTemplate->get_element(0), numElements*sizeof(pd::message::Atom));
    for (int i = 0; i < numElements; i++) {
      if (messageTemplate->is_symbol(i)) {
        char *buffer = (char *) alloca(RES_BUFFER_LENGTH * sizeof(char));
        // TODO(mhroth): resolve string, but may be in stack buffer
        pd::Message::resolve_string(messageTemplate->get_symbol(i), message, 1, buffer, RES_BUFFER_LENGTH);
        outgoing_message->parseAndSetMessageElement(i, buffer); // buffer is resolved to float or string
      }
    }
    send_message(0, outgoing_message);
  }

  // send remote messages
  for (int i = 0; i < remoteMessageList.size(); i++) {
    MessageNamedDestination namedDestination = remoteMessageList.at(i);

    pd::Message::resolve_string(namedDestination.first, message, 1, resolvedName, RES_BUFFER_LENGTH);
    
    pd::Message *messageTemplate = namedDestination.second;
    int numElements = messageTemplate->get_num_elements();
    outgoing_message->from_timestamp(message->get_timestamp(), numElements);
    memcpy(outgoing_message->get_element(0), messageTemplate->get_element(0), numElements*sizeof(pd::message::Atom));
    for (int i = 0; i < numElements; i++) {
      if (messageTemplate->is_symbol(i)) {
        char *buffer = (char *) alloca(RES_BUFFER_LENGTH * sizeof(char));
        // TODO(mhroth): resolve string, but may be in stack buffer
        pd::Message::resolve_string(messageTemplate->get_symbol(i), message, 1, buffer, RES_BUFFER_LENGTH);
        outgoing_message->set_symbol(i, buffer);
      }
    }
    graph->send_message_to_named_receivers(resolvedName, outgoing_message);
  }
}
