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

#include "DeclareList.h"
#include "DspImplicitAdd.h"
#include "DspInlet.h"
#include "DspOutlet.h"
#include "DspTablePlay.h"
#include "DspTableRead.h"
#include "DspTableRead4.h"
#include "MessageInlet.h"
#include "MessageOutlet.h"
#include "MessageTableRead.h"
#include "MessageTableWrite.h"
#include "PdContext.h"
#include "PdGraph.h"
#include "utils.h"


#pragma mark - Constructor/Deconstructor

// a PdGraph begins with zero inlets and zero outlets. These will be added as inlet/~ and outlet/~
// objects are added to the graph
PdGraph::PdGraph(pd::Message *init_message, PdGraph *parentGraph, pd::Context *context, int graphId, const char *graphName) :
    DspObject(0, 0, 0, 0, (parentGraph == NULL) ? context->get_block_size() : parentGraph->get_block_size(), parentGraph) {
  this->parentGraph = parentGraph; // == NULL if this is a root graph
  this->context = context;
  inletList = vector<message::Object *>();
  outletList = vector<message::Object *>();
  nodeList = list<message::Object *>();
  dspNodeList = list<DspObject *>();
  declareList = new DeclareList();
  // all graphs start out unattached to any context, though they exist in a context
  isAttachedToContext = false;
  switched = true; // graphs are switched on by default
  process_function = &processGraph;

  // initialise the graph arguments
  this->graphId = graphId;
  int numInitElements = init_message->get_num_elements();
  graphArguments = PD_MESSAGE_ON_STACK(numInitElements+1);
  graphArguments->from_timestamp(0.0, numInitElements+1);
  graphArguments->set_float(0, (float) graphId); // $0
  memcpy(graphArguments->get_element(1), init_message->get_element(0), numInitElements * sizeof(pd::message::Atom));
  graphArguments = graphArguments->clone_on_heap();
  name = graphName;
}

PdGraph::~PdGraph() {
  graphArguments->free_message();
  delete declareList;

  // remove all implicit +~~ objects
  for (list<DspObject *>::iterator it = dspNodeList.begin(); it != dspNodeList.end(); ++it) {
    DspObject *dspObject = *it;

    if (dspObject->get_graph() != this) break;
    if (!strcmp(dspObject->toString().c_str(), DspImplicitAdd::get_object_label())) {
      delete dspObject;
    }
  }

  // delete all constituent nodes
  for (list<message::Object *>::iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
    delete *it;
  }
}


#pragma mark - Lock/Unlock Context

void PdGraph::lockContextIfAttached() {
  if (isAttachedToContext) {
    context->lock();
  }
}

void PdGraph::unlockContextIfAttached() {
  if (isAttachedToContext) {
    context->unlock();
  }
}


#pragma mark - Add/Remove Objects

void PdGraph::addObject(float canvas_x, float canvas_y, message::Object *message_obj) {
  lockContextIfAttached();

  nodeList.push_back(message_obj); // all nodes are added to the node list regardless

  message_obj->set_coordinates(Coordinates::new(canvas_x, canvas_y));

  switch (message_obj->get_object_type()) {
    case MESSAGE_INLET:
    case DSP_INLET: {
      addLetObjectToLetList(message_obj, coordinates.x, &inletList);
      break;
    }
    case MESSAGE_OUTLET:
    case DSP_OUTLET: {
      addLetObjectToLetList(message_obj, coordinates.x, &outletList);
      break;
    }
    default: {
      // only register objects with the context if the graph is attached
      if (isAttachedToContext) {
        registerObject(message_obj);
      }
      break;
    }
  }

  unlockContextIfAttached();
}

void PdGraph::removeObject(message::Object *object) {
  lockContextIfAttached();

  list<message::Object *>::iterator it = nodeList.begin();
  list<message::Object *>::iterator end = nodeList.end();
  while (it != end) {
    // find the object in the nodeList
    if (*it == object) {

      // remove all incoming connections
      for (int i = 0; i < object->get_num_inlets(); i++) {
        list<Connection> incoming_connections = object->get_incoming_connections(i);
        list<Connection>::iterator lit = incoming_connections.begin();
        list<Connection>::iterator lend = incoming_connections.end();
        while (lit != lend) {
          removeConnection((*lit).first, (*lit).second, object, i);
          lit++;
        }
      }

      // remove all outgoing connections
      for (int i = 0; i < object->get_num_outlets(); i++) {
        list<Connection> outgoing_connections = object->get_outgoing_connections(i);
        list<Connection>::iterator lit = outgoing_connections.begin();
        list<Connection>::iterator lend = outgoing_connections.end();
        while (lit != lend) {
          removeConnection(object, i, (*lit).first, (*lit).second);
          lit++;
        }
      }

      // remove the object from the nodeList
      nodeList.erase(it);

      // remove the object from the dspNodeList if the object processes audio
      if (object->doesProcessAudio()) {
        dspNodeList.remove((DspObject *) object);
      }

      // remove the object from any special lists if it is in any of them (e.g., receive, throw~, etc.)
      unregisterObject(object);

      // delete the object
      delete object;

      break;
    } else {
      it++;
    }
  }

  unlockContextIfAttached();
}

void PdGraph::addLetObjectToLetList(message::Object *inletObject, float newPosition, vector<message::Object *> *letList) {
  vector<message::Object *>::iterator it = letList->begin();
  vector<message::Object *>::iterator end = letList->end();
  while (it != end) {
    message::Object *object = *it;
    float position = 0.0f;
    switch (object->get_object_type()) {
      case MESSAGE_INLET:
      case DSP_INLET:
      case MESSAGE_OUTLET:
      case DSP_OUTLET: {
        position = object->get_coordinates().x;
        break;
      }
      default: break;
    }
    if (newPosition < position) {
      letList->insert(it, inletObject);
      return;
    } else {
      it++;
    }
  }
  letList->push_back(inletObject);
}


#pragma mark - Register/Unregister Objects

void PdGraph::registerObject(message::Object *message_obj) {
  switch (message_obj->get_object_type()) {
    case MESSAGE_RECEIVE:
    case MESSAGE_NOTEIN: {
      context->register_remote_message_receiver(reinterpret_cast<RemoteMessageReceiver *>(message_obj));
      break;
    }
    case MESSAGE_TABLE: {
      // tables must be registered globally as a table, but can also receive remote messages
      context->register_remote_message_receiver(reinterpret_cast<RemoteMessageReceiver *>(message_obj));
      context->register_table((MessageTable *) message_obj);
      break;
    }
    case MESSAGE_TABLE_READ: {
      context->register_table_receiver(reinterpret_cast<MessageTableRead *>(message_obj));
      break;
    }
    case MESSAGE_TABLE_WRITE: {
      context->register_table_receiver(reinterpret_cast<MessageTableWrite *>(message_obj));
      break;
    }
    case DSP_CATCH: {
      context->register_dsp_catch((DspCatch *) message_obj);
      break;
    }
    case DSP_DELAY_READ:
    case DSP_VARIABLE_DELAY: {
      context->register_delay_receiver((DelayReceiver *) message_obj);
      break;
    }
    case DSP_DELAY_WRITE: {
      context->register_delay_line((DspDelayWrite *) message_obj);
      break;
    }
    case DSP_SEND: {
      context->register_dsp_send((DspSend *) message_obj);
      break;
    }
    case DSP_RECEIVE: {
      context->register_dsp_receive((DspReceive *) message_obj);
      break;
    }
    case DSP_TABLE_PLAY: {
      context->register_table_receiver((DspTablePlay *) message_obj);
      break;
    }
    case DSP_TABLE_READ4: {
      context->register_table_receiver((DspTableRead4 *) message_obj);
      break;
    }
    case DSP_TABLE_READ: {
      context->register_table_receiver((DspTableRead *) message_obj);
      break;
    }
    case DSP_THROW: {
      context->register_dsp_throw((DspThrow *) message_obj);
      break;
    }
    default: {
      break; // nothing to do
    }
  }
}

void PdGraph::unregisterObject(message::Object *message_obj) {
  // TODO(mhroth)
  switch (message_obj->get_object_type()) {
    case MESSAGE_RECEIVE:
    case MESSAGE_NOTEIN: {
      context->unregister_remote_message_receiver((RemoteMessageReceiver *) message_obj);
      break;
    }
    case MESSAGE_TABLE_READ: {
      context->unregister_table_receiver((MessageTableRead *) message_obj);
      break;
    }
    case MESSAGE_TABLE_WRITE: {
      context->unregister_table_receiver((MessageTableWrite *) message_obj);
      break;
    }
    case DSP_SEND: {
      context->unregister_dsp_send((DspSend *) message_obj);
      break;
    }
    case DSP_RECEIVE: {
      context->unregister_dsp_receive((DspReceive *) message_obj);
      break;
    }
    case DSP_TABLE_PLAY: {
      context->unregister_table_receiver((DspTablePlay *) message_obj);
      break;
    }
    case DSP_TABLE_READ4: {
      context->unregister_table_receiver((DspTableRead4 *) message_obj);
      break;
    }
    case DSP_TABLE_READ: {
      context->unregister_table_receiver((DspTableRead *) message_obj);
      break;
    }
    default: {
      break;
    }
  }
}


#pragma mark - Attach to Context

void PdGraph::attach_to_context(bool isAttached) {
  // ensure that this function is only run on attachement change
  if (isAttachedToContext != isAttached) {
    isAttachedToContext = isAttached;
    // ensure that all subgraphs know if they are attached or not
    for (list<message::Object *>::iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
      message::Object *message_obj = *it;
      if (isAttachedToContext) {
        registerObject(message_obj);
      } else {
        unregisterObject(message_obj);
      }
      if (message_obj->get_object_type() == object::Type::PURE_DATA) {
        PdGraph *pdGraph = (PdGraph *) message_obj;
        pdGraph->attach_to_context(isAttached);
      }
    }
  }
}


#pragma mark - Path Listing

char *PdGraph::resolveFullPath(const char *filename) {
  if (DeclareList::isFullPath(filename)) {
    return utils::file_exists(filename) ? utils::copy_string(filename) : NULL;
  } else {
    string directory = findFilePath(filename);
    return (!directory.empty()) ? utils::concat_strings(directory.c_str(), filename) : NULL;
  }
}

string PdGraph::findFilePath(const char *filename) {
  for (list<string>::iterator it = declareList->getIterator(); it != declareList->getEnd(); ++it) {
    string directory = *it;
    string fullPath = directory + string(filename);
    if (utils::file_exists(fullPath.c_str())) {
      return directory;
    }
  }
  return isRootGraph() ? "" : parentGraph->findFilePath(filename);
}

void PdGraph::addDeclarePath(const char *path) {
  if (isRootGraph()) {
    declareList->addPath(path);
  } else {
    if (graphId == parentGraph->get_graphId()) {
      // this graph is a subgraph (not an abstraction) of the parent graph
      // so the parent should handle the declared path
      parentGraph->addDeclarePath(path);
    } else {
      declareList->addPath(path);
    }
  }
}


#pragma mark - Manage Messages

pd::Message *PdGraph::schedule_message(message::Object *message_obj, int outlet_index, pd::Message *message) {
  return context->schedule_message(message_obj, outlet_index, message);
}

void PdGraph::cancel_message(message::Object *message_obj, int outlet_index, pd::Message *message) {
  context->cancel_message(message_obj, outlet_index, message);
}

void PdGraph::send_message_to_named_receivers(char *name, pd::Message *message) {
  context->send_message_to_named_receivers(name, message);
}


#pragma mark - Message/DspObject Functions

void PdGraph::receive_message(int inlet_index, pd::Message *message) {
  MessageInlet *inlet = (MessageInlet *) inletList.at(inlet_index);
  inlet->receive_message(0, message);
}

void PdGraph::processGraph(DspObject *dspObject, int fromIndex, int toIndex) {
  PdGraph *d = reinterpret_cast<PdGraph *>(dspObject);

  if (d->switched) {
    // when inlets are processed, they will resolve their buffers and everything will proceed as normal

    // process all dsp objects
    // DSP processing elements are only executed if the graph is switched on

    // TODO(mhroth): iterate depending on local blocksize relative to parent
    // execute all nodes which process audio
    for (list<DspObject *>::iterator it = d->dspNodeList.begin(); it != d->dspNodeList.end(); ++it) {
      DspObject *dspObject = *it;
      dspObject->process_function(dspObject, 0, d->block_sizeInt);
    }
  }
}


#pragma mark - Add/Remove Connections (High Level)

void PdGraph::addConnection(message::Object *fromObject, int outlet_index, message::Object *toObject, int inlet_index) {
  // check to make sure that this connection can even work. Otherwise don't bother.
  if (outlet_index >= fromObject->get_num_outlets() || inlet_index >= toObject->get_num_inlets()) {
    print_err("mismatched connnection. Attempt to make a connection from "
        "%s(%p):%i/%i to %s(%p):%i/%i. Connection ignored.",
        fromObject->toString().c_str(), fromObject, outlet_index, fromObject->get_num_outlets(),
        toObject->toString().c_str(), toObject, inlet_index, toObject->get_num_inlets());
    return;
  }

  lockContextIfAttached();
  toObject->add_connection_from_object_to_inlet(fromObject, outlet_index, inlet_index);
  fromObject->add_connection_to_object_from_outlet(toObject, inlet_index, outlet_index);

  // NOTE(mhroth): very heavy handed approach. Always recompute the process order when adding connections.
  // In theory this function should check to see if a reordering is even necessary and then only make
  // the appropriate changes. Usually a complete reevaluation shouldn't be necessary, and otherwise
  // the use of a linked list to store the node list should make the reordering fast.
  //  compute_deep_local_process_order();

  unlockContextIfAttached();
}

void PdGraph::addConnection(int fromObjectIndex, int outlet_index, int toObjectIndex, int inlet_index) {
  list<message::Object *>::iterator fromIt = nodeList.begin();
  for (int i = 0; i < fromObjectIndex; i++) fromIt++;
  list<message::Object *>::iterator toIt = nodeList.begin();
  for (int i = 0; i < toObjectIndex; i++) toIt++;

  message::Object *fromObject = *fromIt;
  message::Object *toObject = *toIt;
  addConnection(fromObject, outlet_index, toObject, inlet_index);
}

/*
 * removeConnection does not force a reordering of the dspNodeList, as lost connections do not create
 * any new constraints on the dsp object ordering that weren't there already. addConnection does
 * force a reevaluation, as the new connection may force a new constrained how how objects are ordered.
 */
void PdGraph::removeConnection(message::Object *fromObject, int outlet_index, message::Object *toObject, int inlet_index) {
  lockContextIfAttached();
  toObject->remove_connection_from_object_to_inlet(fromObject, outlet_index, inlet_index);
  fromObject->remove_connection_to_object_from_outlet(toObject, inlet_index, outlet_index);
  unlockContextIfAttached();
}

list<Connection> PdGraph::get_incoming_connections(unsigned int inlet_index) {
  if (inletList.empty()) {
    return list<Connection>();
  } else {
    message::Object *inletObject = inletList[inlet_index];
    switch (inletObject->get_object_type()) {
      case MESSAGE_INLET: {
        MessageInlet *messageInlet = (MessageInlet *) inletObject;
        return messageInlet->get_incoming_connections(0);
      }
      case DSP_INLET: {
        DspInlet *dspInlet = (DspInlet *) inletObject;
        return dspInlet->get_incoming_connections(0);
      }
      default: {
        return list<Connection>();
      }
    }
  }
}

list<Connection> PdGraph::get_outgoing_connections(unsigned int outlet_index) {
  if (outletList.empty()) {
    return list<Connection>();
  } else {
    message::Object *outletObject = outletList[outlet_index];
    switch (outletObject->get_object_type()) {
      case MESSAGE_OUTLET: {
        MessageOutlet *messageOutlet = (MessageOutlet *) outletObject;
        return messageOutlet->get_outgoing_connections(0);
      }
      case DSP_OUTLET: {
        DspOutlet *dspOutlet = (DspOutlet *) outletObject;
        return dspOutlet->get_outgoing_connections(0);
      }
      default: {
        return list<Connection>();
      }
    }
  }
}


#pragma mark - Add/Remove Connections (Low Level)

void PdGraph::add_connection_from_object_to_inlet(message::Object *message_obj, int outlet_index, int inlet_index) {
  switch (message_obj->get_connection_type(outlet_index)) {
    case MESSAGE: {
      message::Object *inletObject = inletList.at(inlet_index);
      if (inletObject->get_object_type() == MESSAGE_INLET) {
        MessageInlet *messageInlet = reinterpret_cast<MessageInlet *>(inletObject);
        messageInlet->add_connection_from_object_to_inlet(message_obj, outlet_index, 0);
      } else {
        print_err("Connection [%s]:%i is of type DSP and cannot be connected to inlet.",
            message_obj->get_object_label(), outlet_index);
      }
      break;
    }
    case DSP: {
      message::Object *inletObject = inletList.at(inlet_index);
      if (inletObject->get_object_type() == DSP_INLET) {
        DspInlet *dspInlet = reinterpret_cast<DspInlet *>(inletObject);
        dspInlet->add_connection_from_object_to_inlet(message_obj, outlet_index, 0);
      } else {
        print_err("Connection [%s]:%i is of type MESSAGE and cannot be connected to inlet~.",
            message_obj->get_object_label(), outlet_index);
      }
      break;
    }
    default: break;
  }
}

void PdGraph::add_connection_to_object_from_outlet(message::Object *message_obj, int inlet_index, int outlet_index) {
  switch (get_connection_type(outlet_index)) {
    case MESSAGE: {
      MessageOutlet *messageOutlet = reinterpret_cast<MessageOutlet *>(outletList.at(outlet_index));
      messageOutlet->add_connection_to_object_from_outlet(message_obj, inlet_index, 0);
      break;
    }
    case DSP: {
      DspOutlet *dspOutlet = reinterpret_cast<DspOutlet *>(outletList.at(outlet_index));
      dspOutlet->add_connection_to_object_from_outlet(message_obj, inlet_index, 0);
      break;
    }
    default: break;
  }
}

void PdGraph::remove_connection_from_object_to_inlet(message::Object *message_obj, int outlet_index, int inlet_index) {
  // NOTE(mhroth): double check! this should work even if inletObject is a DspInlet
  message::Object *inletObject = inletList.at(inlet_index);
  inletObject->remove_connection_from_object_to_inlet(message_obj, outlet_index, 0);
}

void PdGraph::remove_connection_to_object_from_outlet(message::Object *message_obj, int inlet_index, int outlet_index) {
  message::Object *outletObject = outletList.at(outlet_index);
  outletObject->remove_connection_to_object_from_outlet(message_obj, inlet_index, 0);
}


#pragma mark - Get/Set Inlet/Outlet Buffers

void PdGraph::set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index) {
  message::Object *inletObject = inletList[inlet_index];
  if (inletObject->get_object_type() == DSP_INLET) {
    DspObject *dspInlet = reinterpret_cast<DspInlet *>(inletObject);
    dspInlet->set_dsp_buffer_at_inlet(buffer, 0);
  }
}

void PdGraph::setDspBufferAtOutlet(float *buffer, unsigned int outlet_index) {
  // nothing to do because DspOutlet objects do not allow setting outlet buffers
}

float *PdGraph::get_dsp_buffer_at_inlet(int inlet_index) {
  message::Object *inletObject = inletList[inlet_index];
  if (inletObject->get_object_type() == DSP_INLET) {
    DspObject *dspInlet = reinterpret_cast<DspInlet *>(inletObject);
    return dspInlet->get_dsp_buffer_at_inlet(0);
  }
  return NULL; // if you've gotten this far, something's gone wrong
}

float *PdGraph::get_dsp_buffer_at_outlet(int outlet_index) {
  message::Object *outletObject = outletList[outlet_index];
  if (outletObject->get_object_type() == DSP_OUTLET) {
    DspObject *dspOutlet = reinterpret_cast<DspInlet *>(outletObject);
    return dspOutlet->get_dsp_buffer_at_outlet(0);
  }
  return NULL; // if you've gotten this far, something's gone wrong
}


#pragma mark - Process Order

list<DspObject *> PdGraph::get_process_order() {
  if (is_ordered) {
    return list<DspObject *>();
  } else {
    is_ordered = true;
    list<DspObject *> processOrder;
    for (vector<message::Object *>::iterator it = inletList.begin(); it != inletList.end(); ++it) {
      message::Object *message_obj = *it;
      // NOTE(mhroth): try to use some "GraphInlet" interface here
      switch (message_obj->get_object_type()) {
        case MESSAGE_INLET: {
          MessageInlet *messgeInlet = reinterpret_cast<MessageInlet *>(message_obj);
          list<DspObject *> inletProcessOrder = messgeInlet->get_process_orderFromInlet();
          processOrder.splice(processOrder.end(), inletProcessOrder);
          break;
        }
        case DSP_INLET: {
          DspInlet *dspInlet = reinterpret_cast<DspInlet *>(message_obj);
          list<DspObject *> inletProcessOrder = dspInlet->get_process_orderFromInlet();
          processOrder.splice(processOrder.end(), inletProcessOrder);
          break;
        }
        default: break;
      }
    }
    compute_deep_local_process_order();
    if (doesProcessAudio()) processOrder.push_back(this);
    return processOrder;
  }
}

bool PdGraph::is_leaf_node() {
  vector<message::Object *>::iterator it = outletList.begin();
  vector<message::Object *>::iterator end = outletList.end();
  while (it != end) {
    if ((*it++)->is_leaf_node()) {
      return false;
    }
  }
  return true;
}

void PdGraph::compute_deep_local_process_order() {
  lockContextIfAttached();

  /* clear/reset dspNodeList
   * Find all leaf nodes in nodeList. this includes PdGraphs as they are objects as well.
   * For each leaf node, generate an ordering for all of the nodes in the current graph.
   * the basic idea is to compute the full process order in each subgraph.
   * Finally, all non-dsp nodes must be removed from this list in order to derive the dsp process order.
   */

  // generate the leafnode list
  list<message::Object *> leafNodeList;
  for (list<message::Object *>::iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
    message::Object *object = *it;

    object->reset_ordered_flag(); // reset the ordered flag on all local objects
    if (object->is_leaf_node()) { // is_leaf_node() takes into account send/~ and throw~ objects
      leafNodeList.push_back(object);
    }
  }

  // remove all +~~ objects
  for (list<DspObject *>::iterator it = dspNodeList.begin(); it != dspNodeList.end(); ++it) {
    DspObject *dspObject = *it;
    if (!strcmp(dspObject->toString().c_str(), DspImplicitAdd::get_object_label())) {
      delete dspObject;
    }
  }

  dspNodeList.clear();

  // for all leaf nodes, order the tree
  for (list<message::Object *>::iterator it = leafNodeList.begin(); it != leafNodeList.end(); ++it) {
    message::Object *object = *it;
    list<DspObject *> processSubList = object->get_process_order();
    dspNodeList.splice(dspNodeList.end(), processSubList);
  }

  /* print out process order of local dsp objects (for debugging) */
  /*
  if (!dspNodeList.empty()) {
    // print dsp evaluation order for debugging, but only if there are any nodes to list
    print_std("  - ordered evaluation list ---");
    list<DspObject *>::iterator it = dspNodeList.begin();
    list<DspObject *>::iterator end = dspNodeList.end();
    while (it != end) {
      print_std((*it++)->toString().c_str());
    }
    print_std("\n");
  }
  */

  unlockContextIfAttached();
}

#pragma mark - Print

void PdGraph::print_err(const char *msg, ...) {
  int maxStringLength = 1024;
  char stringBuffer[maxStringLength];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, maxStringLength-1, msg, ap);
  va_end(ap);

  context->print_err(stringBuffer);
}

void PdGraph::print_std(const char *msg, ...) {
  int maxStringLength = 1024;
  char stringBuffer[maxStringLength];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, maxStringLength-1, msg, ap);
  va_end(ap);

  context->print_std(stringBuffer);
}

#pragma mark - Get Attributes

double PdGraph::getBlockIndex(pd::Message *message) {
  // sample_rate is in samples/second, but we need samples/millisecond
  return (message->get_timestamp() - context->get_block_start_timestamp()) * 0.001 * context->get_sample_rate();
}

float PdGraph::get_sample_rate() {
  // there is no such thing as a local sample rate. Return the sample rate of the context.
  return context->get_sample_rate();
}

int PdGraph::get_graphId() {
  return graphId;
}

float *PdGraph::get_global_dsp_buffer_at_inlet(int inlet_index) {
  return context->get_global_dsp_buffer_at_inlet(inlet_index);
}

float *PdGraph::get_global_dsp_buffer_at_outlet(int outlet_index) {
  return context->get_global_dsp_buffer_at_outlet(outlet_index);
}

pd::Message *PdGraph::getArguments() {
  return graphArguments;
}

int PdGraph::get_num_input_channels() {
  return context->get_num_input_channels();
}

int PdGraph::get_num_output_channels() {
  return context->get_num_output_channels();
}

int PdGraph::get_block_size() {
  return block_sizeInt;
}

bool PdGraph::isRootGraph() {
  return (parentGraph == NULL);
}

MessageTable *PdGraph::get_table(char *name) {
  return context->get_table(name);
}

connection::Type PdGraph::get_connection_type(int outlet_index) {
  // return the connection type depending on the type of outlet object
  message::Object *message_obj = (message::Object *) outletList.at(outlet_index);
  return message_obj->get_connection_type(0);
}

bool PdGraph::doesProcessAudio() {
  // This graph processes audio if it contains any nodes which process audio.
  // This works because graph objects are only created after they have been filled with objects.
  return !dspNodeList.empty();
}

void PdGraph::setBlockSize(int block_size) {
  // only update blocksize if it is <= the parent's
  if (block_size <= parentGraph->get_block_size()) {
    // TODO(mhroth)
    block_sizeInt = block_size;
    // update block_duration, etc.
    // notify all dsp objects and get them to resize their buffers accordingly
  }
}

PdGraph *PdGraph::getParentGraph() {
  return parentGraph;
}

void PdGraph::setSwitch(bool switched) {
  this->switched = switched;
}

bool PdGraph::isSwitchedOn() {
  return switched;
}

void PdGraph::set_value_for_name(const char *name, float constant) {
  context->set_value_for_name(name, constant);
}

float PdGraph::get_value_for_name(const char *name) {
  return context->get_value_for_name(name);
}

unsigned int PdGraph::get_num_inlets() {
  return inletList.size();
}

unsigned int PdGraph::get_num_outlets() {
  return outletList.size();
}

pd::Context *PdGraph::getContext() {
  return context;
}

list<message::Object *> PdGraph::getNodeList() {
  return nodeList;
}

BufferPool *PdGraph::get_buffer_pool() {
  return context->get_buffer_pool();
}
