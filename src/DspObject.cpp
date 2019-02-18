/*
 *  Copyright 2009,2010,2011,2012 Reality Jockey, Ltd.
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
#include "DspImplicitAdd.h"
#include "DspObject.h"
#include "PdGraph.h"


#pragma mark - Constructor/Destructor

DspObject::DspObject(int numMessageInlets, int numDspInlets, int numMessageOutlets, int numDspOutlets, PdGraph *graph) :
    message::Object(numMessageInlets, numMessageOutlets, graph) {
  init(numDspInlets, numDspOutlets, graph->getBlockSize());
}

DspObject::DspObject(int numMessageInlets, int numDspInlets, int numMessageOutlets, int numDspOutlets, int block_size, PdGraph *graph) : 
    message::Object(numMessageInlets, numMessageOutlets, graph) {
  init(numDspInlets, numDspOutlets, block_size);
}

void DspObject::init(int numDspInlets, int numDspOutlets, int block_size) {
  block_sizeInt = block_size;
  processFunction = &processFunctionDefaultNoMessage;
  processFunctionNoMessage = &processFunctionDefaultNoMessage;
  
  // initialise the incoming dsp connections list
  incomingDspConnections = vector<list<Connection> >(numDspInlets);
  
  // initialise the outgoing dsp connections list
  outgoingDspConnections = vector<list<Connection> >(numDspOutlets);

  memset(dspBufferAtInlet, 0, sizeof(float *) * 3);
  if (numDspInlets > 2) dspBufferAtInlet[2] = (float *) calloc(numDspInlets-2, sizeof(float *));
  
  memset(dspBufferAtOutlet, 0, sizeof(float *) * 3);
  if (numDspOutlets > 2) dspBufferAtOutlet[2] = (float *) calloc(numDspOutlets-2, sizeof(float *));
}

DspObject::~DspObject() {  
  
  // delete any messages still pending
  clearMessageQueue();
  
  // inlet and outlet buffers are managed by the BufferPool
  if (getNumDspInlets() > 2) free(dspBufferAtInlet[2]);
  if (getNumDspOutlets() > 2) free(dspBufferAtOutlet[2]);
}


#pragma mark -

connection::Type DspObject::get_connection_type(int outlet_index) {
  return DSP;
}

float *DspObject::getDspBufferAtInlet(int inlet_index) {
  return (inlet_index < 2)
      ? dspBufferAtInlet[inlet_index] : ((float **) dspBufferAtInlet[2])[inlet_index-2];
}

float *DspObject::getDspBufferAtOutlet(int outlet_index) {
  if (outlet_index < 2) return dspBufferAtOutlet[outlet_index];
  else return ((float **) dspBufferAtOutlet[2])[outlet_index-2];
}

list<Connection> DspObject::get_incoming_connections(unsigned int inlet_index) {
  list<Connection> messageConnectionList = message::Object::get_incoming_connections(inlet_index);
  list<Connection> dspConnectionList = incomingDspConnections.empty()
      ? list<Connection>() : incomingDspConnections[inlet_index];
  messageConnectionList.insert(messageConnectionList.end(), dspConnectionList.begin(), dspConnectionList.end());
  return messageConnectionList;
}

list<Connection> DspObject::getIncomingDspConnections(unsigned int inlet_index) {
  return (inlet_index < incomingDspConnections.size()) ? incomingDspConnections[inlet_index] : list<Connection>();
}

list<Connection> DspObject::get_outgoing_connections(unsigned int outlet_index) {
  list<Connection> messageConnectionList = message::Object::get_outgoing_connections(outlet_index);
  list<Connection> dspConnectionList = outgoingDspConnections.empty()
      ? list<Connection>() : outgoingDspConnections[outlet_index];
  messageConnectionList.insert(messageConnectionList.end(), dspConnectionList.begin(), dspConnectionList.end());
  return messageConnectionList;
}

list<Connection> DspObject::getOutgoingDspConnections(unsigned int outlet_index) {
  return outgoingDspConnections[outlet_index];
}


#pragma mark - Add/Remove Connections

void DspObject::add_connection_from_object_to_inlet(message::Object *message_obj, int outlet_index, int inlet_index) {
  message::Object::add_connection_from_object_to_inlet(message_obj, outlet_index, inlet_index);
  
  if (message_obj->get_connection_type(outlet_index) == DSP) {
    list<Connection> *connections = &incomingDspConnections[inlet_index];
    Connection obj_let_pair = Connection::new(message_obj, outlet_index);
    connections->push_back(obj_let_pair);
  }
  
  onInletConnectionUpdate(inlet_index);
}

void DspObject::remove_connection_from_object_to_inlet(message::Object *message_obj, int outlet_index, int inlet_index) {
  if (message_obj->get_connection_type(outlet_index) == DSP) {
    list<Connection> *connections = &incomingDspConnections[inlet_index];
    Connection obj_let_pair = Connection::new(message_obj, outlet_index);
    connections->remove(obj_let_pair); // NOTE(mhroth): does this work?
  } else {
    message::Object::remove_connection_from_object_to_inlet(message_obj, outlet_index, inlet_index);
  }
  
  onInletConnectionUpdate(inlet_index);
}

void DspObject::onInletConnectionUpdate(unsigned int inlet_index) {
  // nothing to do
}

void DspObject::add_connection_to_object_from_outlet(message::Object *message_obj, int inlet_index, int outlet_index) {
  message::Object::add_connection_to_object_from_outlet(message_obj, inlet_index, outlet_index);
  
  // TODO(mhroth): it is assumed here that the input connection type of the destination object is DSP. Correct?
  if (get_connection_type(outlet_index) == DSP) {
    list<Connection> *connections = &outgoingDspConnections[outlet_index];
    Connection obj_let_pair = Connection::new(message_obj, inlet_index);
    connections->push_back(obj_let_pair);
  }
}

void DspObject::remove_connection_to_object_from_outlet(message::Object *message_obj, int inlet_index, int outlet_index) {
  if (get_connection_type(outlet_index) == MESSAGE) {
    message::Object::remove_connection_to_object_from_outlet(message_obj, inlet_index, outlet_index);
  } else {
    list<Connection> *outgoing_connections = &outgoingDspConnections[outlet_index];
    Connection obj_let_pair = Connection::new(message_obj, inlet_index);
    outgoing_connections->remove(obj_let_pair);
  }
}

void DspObject::setDspBufferAtInlet(float *buffer, unsigned int inlet_index) {
  if (inlet_index < 2) dspBufferAtInlet[inlet_index] = buffer;
  else ((float **) dspBufferAtInlet[2])[inlet_index-2] = buffer;
}

void DspObject::setDspBufferAtOutlet(float *buffer, unsigned int outlet_index) {
  if (outlet_index < 2) dspBufferAtOutlet[outlet_index] = buffer;
  else ((float **) dspBufferAtOutlet[2])[outlet_index-2] = buffer;
}


#pragma mark -

void DspObject::clearMessageQueue() {
  while (!messageQueue.empty()) {
    MessageConnection messageConnection = messageQueue.front();
    pd::Message *message = messageConnection.first;
    message->freeMessage();
    messageQueue.pop();
  }
}

void DspObject::receive_message(int inlet_index, pd::Message *message) {
  // Queue the message to be processed during the DSP round only if the graph is switched on.
  // Otherwise messages would begin to pile up because the graph is not processed.
  if (graph->isSwitchedOn()) {
    // Copy the message to the heap so that it is available to process later.
    // The message is released once it is consumed in processDsp().
    messageQueue.push(Connection::new(message->clone_on_heap(), inlet_index));
    
    // only process the message if the process function is set to the default no-message function.
    // If it is set to anything else, then it is assumed that messages should not be processed.
    if (processFunction == processFunctionNoMessage) processFunction = &processFunctionMessage;
  }
}


#pragma mark - processDsp

void DspObject::processFunctionDefaultNoMessage(DspObject *dspObject, int fromIndex, int toIndex) {
  dspObject->processDspWithIndex(fromIndex, toIndex);
}

void DspObject::processFunctionMessage(DspObject *dspObject, int fromIndex, int toIndex) {
  double blockIndexOfLastMessage = 0.0; // reset the block index of the last received message
  do { // there is at least one message
    MessageConnection messageConnection = dspObject->messageQueue.front();
    pd::Message *message = messageConnection.first;
    unsigned int inlet_index = messageConnection.second;
    
    double blockIndexOfCurrentMessage = dspObject->graph->getBlockIndex(message);
    dspObject->processFunctionNoMessage(dspObject,
        ceil(blockIndexOfLastMessage), ceil(blockIndexOfCurrentMessage));
    dspObject->process_message(inlet_index, message);
    message->freeMessage(); // free the message from the head, the message has been consumed.
    dspObject->messageQueue.pop();
    
    blockIndexOfLastMessage = blockIndexOfCurrentMessage;
  } while (!dspObject->messageQueue.empty());
  dspObject->processFunctionNoMessage(dspObject, ceil(blockIndexOfLastMessage), toIndex);
  
  // because messages are received much less often than on a per-block basis, once messages are
  // processed in this block, return to the default process function which assumes that no messages
  // are present. This improves performance because the messageQueue must not be checked for
  // any pending messages. It is assumed that there aren't any.
  dspObject->processFunction = dspObject->processFunctionNoMessage;
}

void DspObject::processDspWithIndex(double fromIndex, double toIndex) {
  // by default, this function just calls the integer version with adjusted block indicies
  processDspWithIndex((int) ceil(fromIndex), (int) ceil(toIndex));
}

void DspObject::processDspWithIndex(int fromIndex, int toIndex) {
  // by default, this function just calls the float version
  processDspWithIndex((float) fromIndex, (float) toIndex);
}


#pragma mark - Process Order

bool DspObject::is_leaf_node() {
  if (!message::Object::is_leaf_node()) return false;
  else {
    for (int i = 0; i < outgoingDspConnections.size(); i++) {
      if (!outgoingDspConnections[i].empty()) return false;
    }
    return true;
  }
}

list<DspObject *> DspObject::get_process_order() {
  if (is_ordered) {
    // if this object has already been ordered, then move on
    return list<DspObject *>();
  } else {
    is_ordered = true;
    list<DspObject *> processList;
    for (int i = 0; i < incoming_connections.size(); i++) {
      for (list<Connection>::iterator it = incoming_connections[i].begin();
          it != incoming_connections[i].end(); ++it) {
        Connection obj_let_pair = *it;
        list<DspObject *> parentProcessList = obj_let_pair.first->get_process_order();
        // DspObjects "connected" through message connections must be processed, but buffers
        // not otherwise calculated
        processList.splice(processList.end(), parentProcessList);
      }
    }
    
    BufferPool *bufferPool = graph->getBufferPool();
    pd::Message *dspAddInitMessage = PD_MESSAGE_ON_STACK(1);
    dspAddInitMessage->from_timestamp_and_float(0, 0.0f);
    for (int i = 0; i < incomingDspConnections.size(); i++) {
      switch (incomingDspConnections[i].size()) {
        case 0: {
          setDspBufferAtInlet(bufferPool->getZeroBuffer(), i);
          break;
        }
        case 1: {
          Connection obj_let_pair = incomingDspConnections[i].front();
          list<DspObject *> parentProcessList = obj_let_pair.first->get_process_order();
          
          // configure the input buffers
          DspObject *dspObject = reinterpret_cast<DspObject *>(obj_let_pair.first);
          float *buffer = dspObject->getDspBufferAtOutlet(obj_let_pair.second);
          setDspBufferAtInlet(buffer, i);
          // NOTE(mhroth): inlet buffer is released once all inlet buffers have been resolved
          // This is so that a buffer at an earlier inlet is not used when resolving buffers
          // while in the get_process_order() function of a following inlet.
          
          // combine the process lists
          processList.splice(processList.end(), parentProcessList);
          break;
        }
        default: { // > 1
          /*
           * [obj~] [obj~] [obj~]...
           *   \     /      /
           *    \   /      /
           *    [+~~]     /
           *     \       /
           *      \     /
           *       \   /
           *       [+~~]
           *        \
           *         etc...
           */
          list<Connection>::iterator it = incomingDspConnections[i].begin();
          Connection leftOlPair = *it++;
          list<DspObject *> parentProcessList = leftOlPair.first->get_process_order();
          processList.splice(processList.end(), parentProcessList);
          
          while (it != incomingDspConnections[i].end()) {
            Connection rightOlPair = *it++;
            list<DspObject *> parentProcessList = rightOlPair.first->get_process_order();
            processList.splice(processList.end(), parentProcessList);
            
            DspImplicitAdd *dspAdd = new DspImplicitAdd(dspAddInitMessage, get_graph());
            float *buffer = reinterpret_cast<DspObject *>(leftOlPair.first)->getDspBufferAtOutlet(leftOlPair.second);
            dspAdd->setDspBufferAtInlet(buffer, 0);
            bufferPool->releaseBuffer(buffer);
            
            buffer = reinterpret_cast<DspObject *>(rightOlPair.first)->getDspBufferAtOutlet(rightOlPair.second);
            dspAdd->setDspBufferAtInlet(buffer, 1);
            bufferPool->releaseBuffer(buffer);
            
            // assign the output buffer of the +~~
            dspAdd->setDspBufferAtOutlet(bufferPool->getBuffer(1), 0);
            
            processList.push_back(dspAdd);
            leftOlPair = Connection::new(dspAdd, 0);
          }
          
          float *buffer = reinterpret_cast<DspObject *>(leftOlPair.first)->getDspBufferAtOutlet(leftOlPair.second);
          setDspBufferAtInlet(buffer, i);
          // inlet buffer is released once all inlet buffers have been resolved
          break;
        }
      }
    }
    
    // release the inlet buffers only after everything has been set up
    for (int i = 0; i < getNumDspInlets(); i++) {
      float *buffer = getDspBufferAtInlet(i);
      bufferPool->releaseBuffer(buffer);
    }
    
    // set the outlet buffers
    for (int i = 0; i < getNumDspOutlets(); i++) {
      if (canSetBufferAtOutlet(i)) {
        float *buffer = bufferPool->getBuffer(outgoingDspConnections[i].size());
        setDspBufferAtOutlet(buffer, i);
      }
    }
    
    // NOTE(mhroth): even if an object does not process audio, its buffer still needs to be connected.
    // They may be passed on to other objects, such as s~/r~ pairs
    if (doesProcessAudio()) processList.push_back(this);
    return processList;
  }
}
