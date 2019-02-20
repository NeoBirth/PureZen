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

#if __APPLE__
#include <Accelerate/Accelerate.h>
#endif
#include <string.h>
#include "MessageTable.h"
#include "PdAbstractionDataBase.h"
#include "pd::Context.h"
#include "PdFileParser.h"
#include "PdGraph.h"
#include "ZenGarden.h"

/*
void zg_remove_graph(pd::Context *context, PdGraph *graph) {
  context->removeGraph(graph);
}
*/

ZGObject *zg_graph_add_new_object(PdGraph *graph, const char *objectString, float coordinates.x, float coordinates.y) {
  char *objectStringCopy = utils::copy_string(objectString);
  char *object_label = strtok(objectStringCopy, " ;");
  char *initString = strtok(NULL, ";");
  char resolutionBuffer[256];
  pd::Message *init_message = PD_MESSAGE_ON_STACK(32);
  init_message->from_string_and_args(32, initString, graph->getArguments(), resolutionBuffer, 256);
  message::Object *message_obj = graph->getContext()->new_object(object_label, init_message, graph);
  free(objectStringCopy);

  if (message_obj != NULL) {
    graph->addObject(coordinates.x, coordinates.y, message_obj);
  }

  return message_obj;
}

void zg_graph_delete(ZGGraph *graph) {
  if (graph != NULL) {
    /*
    if (graph->isAttachedToContext()) {
      context->removeGraph(graph);
    }
    */
    delete graph;
  }
}


#pragma mark - Object

void zg_object_remove(message::Object *object) {
  object->get_graph()->removeObject(object);
}

ZGconnection::Type zg_object_get_connection_type(ZGObject *object, unsigned int outlet_index) {
  switch (object->get_connection_type(outlet_index)) {
    default:
    case MESSAGE: return ZG_CONNECTION_MESSAGE;
    case DSP: return ZG_CONNECTION_DSP;
  }
}

unsigned int zg_object_get_num_inlets(ZGObject *object) {
  return (object != NULL) ? object->get_num_inlets() : 0;
}

unsigned int zg_object_get_num_outlets(ZGObject *object) {
  return (object != NULL) ? object->get_num_outlets() : 0;
}

ZGConnectionPair *zg_object_get_connections_at_inlet(ZGObject *object, unsigned int inlet_index, unsigned int *n) {
  list<Connection> connections = object->get_incoming_connections(inlet_index);
  *n = connections.size();
  int i = 0;
  Connection *conns = (Connection *) malloc(connections.size() * sizeof(Connection));
  for (list<Connection>::iterator it = connections.begin(); it != connections.end(); it++, i++) {
    conns[i] = *it;
  }
  return (ZGConnectionPair *) conns;
}

ZGConnectionPair *zg_object_get_connections_at_outlet(ZGObject *object, unsigned int outlet_index, unsigned int *n) {
  list<Connection> connections = object->get_outgoing_connections(outlet_index);
  *n = connections.size();
  int i = 0;
  Connection *conns = (Connection *) malloc(connections.size() * sizeof(Connection));
  for (list<Connection>::iterator it = connections.begin(); it != connections.end(); it++, i++) {
    conns[i] = *it;
  }
  return (ZGConnectionPair *) conns;
}

const char *zg_object_get_label(ZGObject *object) {
  return object->get_object_label();
}

void zg_object_send_message(message::Object *object, unsigned int inlet_index, ZGMessage *message) {
  object->get_graph()->lockContextIfAttached();
  object->receive_message(inlet_index, message);
  object->get_graph()->unlockContextIfAttached();
}

Coordinates zg_object_get_coordinates(ZGObject *object) {
  object->get_coordinates()
}

void zg_object_set_coordinates(ZGObject *object, Coordinates coordinates) {
  object->set_coordinates(coordinates);
}

char *zg_object_to_string(ZGObject *object) {
  string str = object->toString();
  const char *strptr = str.c_str();
  char *ret = (char *) malloc((strlen(strptr)+1)*sizeof(char));
  strcpy(ret, strptr);
  return ret;
}


#pragma mark - Context

ZGContext *zg_context_new(int num_input_channels, int num_output_channels, int block_size, float sample_rate,
      void *(*callback_function)(ZGCallbackFunction, void *, void *), void *userData) {
  return new pd::Context(num_input_channels, num_output_channels, block_size, sample_rate,
      callback_function, userData);
}

void zg_context_delete(ZGContext *context) {
  delete context;
}

ZGGraph *zg_context_new_empty_graph(pd::Context *context) {
  pd::Message *init_message = PD_MESSAGE_ON_STACK(0); // create an empty message to use for initialisation
  init_message->from_timestamp(0.0, 0);
  // the new graph has no parent graph and is created in the given context with a unique id
  PdGraph *graph = new PdGraph(init_message, NULL, context, context->get_next_graph_id(), "zg_free");
  return graph;
}

ZGGraph *zg_context_new_graph_from_file(pd::Context *context, const char *directory, const char *filename) {
  PdFileParser *parser = new PdFileParser(string(directory), string(filename));
  PdGraph *graph = parser->execute(context);
  graph->addDeclarePath(directory); // ensure that the root director is added to the declared path set
  delete parser;
  return graph;
}

ZGGraph *zg_context_new_graph_from_string(pd::Context *context, const char *netlist) {
  PdFileParser *parser = new PdFileParser(string(netlist));
  PdGraph *graph = parser->execute(context);
  delete parser;
  return graph;
}

void zg_context_process(pd::Context *context, float *input_buffers, float *output_buffers) {
  context->process(input_buffers, output_buffers);
}

void zg_context_process_s(ZGContext *context, short *input_buffers, short *output_buffers) {
  const int num_input_channels = context->get_num_input_channels();
  const int num_output_channels = context->get_num_output_channels();
  const int block_size = context->get_block_size();
  const int input_bufferLength = num_input_channels*block_size;
  const int output_bufferLength = num_output_channels*block_size;
  float finput_buffers[input_bufferLength];
  float foutput_buffers[output_bufferLength];

  #if __APPLE__
  // convert short to float, and uninterleave the samples into the float buffer
  // allow fallthrough in all cases
  switch (num_input_channels) {
    default: { // input channels > 2
      for (int i = 2; i < num_input_channels; ++i) {
        vDSP_vflt16(input_buffers+i, num_input_channels, finput_buffers+i*block_size, 1, block_size);
      } // allow fallthrough
    }
    case 2: vDSP_vflt16(input_buffers+1, num_input_channels, finput_buffers+block_size, 1, block_size);
    case 1: vDSP_vflt16(input_buffers, num_input_channels, finput_buffers, 1, block_size);
    case 0: break;
  }

  // convert samples to range of [-1,+1]
  float a = 0.000030517578125f; // == 2^-15
  vDSP_vsmul(finput_buffers, 1, &a, finput_buffers, 1, input_bufferLength);

  // process the samples
  context->process(finput_buffers, foutput_buffers);

  // clip the output to [-1,+1]
  float min = -1.0f;
  float max = 1.0f;
  vDSP_vclip(foutput_buffers, 1, &min, &max, foutput_buffers, 1, output_bufferLength);

  // scale the floating-point samples to short range
  a = 32767.0f;
  vDSP_vsmul(foutput_buffers, 1, &a, foutput_buffers, 1, output_bufferLength);

  // convert float to short and interleave into short buffer
  // allow fallthrough in all cases
  switch (num_output_channels) {
    default: { // output channels > 2
      for (int i = 2; i < num_output_channels; ++i) {
        vDSP_vfix16(foutput_buffers+i*block_size, num_output_channels, output_buffers+i, 1, block_size);
      } // allow fallthrough
    }
    case 2: vDSP_vfix16(foutput_buffers+block_size, 1, output_buffers+1, num_output_channels, block_size);
    case 1: vDSP_vfix16(foutput_buffers, 1, output_buffers, num_output_channels, block_size);
    case 0: break;
  }
  #else
  // uninterleave and short->float the samples in input_buffers to finput_buffers
  switch (num_input_channels) {
    default: {
      for (int k = 2; k < num_input_channels; k++) {
        for (int i = k, j = k*block_size; i < input_bufferLength; i+=num_input_channels, j++) {
          finput_buffers[j] = ((float) input_buffers[i]) / 32768.0f;
        }
      } // allow fallthrough
    }
    case 2: {
      for (int i = 1, j = block_size; i < input_bufferLength; i+=num_input_channels, j++) {
        finput_buffers[j] = ((float) input_buffers[i]) / 32768.0f;
      }  // allow fallthrough
    }
    case 1: {
      for (int i = 0, j = 0; i < input_bufferLength; i+=num_input_channels, j++) {
        finput_buffers[j] = ((float) input_buffers[i]) / 32768.0f;
      } // allow fallthrough
    }
    case 0: break;
  }

  // process the context
  context->process(finput_buffers, foutput_buffers);

  // clip the output to [-1,1]
  for (int i = 0; i < output_bufferLength; i++) {
    float f = foutput_buffers[i];
    foutput_buffers[i] = (f < -1.0) ? -1.0f : (f > 1.0f) ? 1.0f : f;
  }

  // interleave and float->short the samples in finput_buffers to cinput_buffers
  switch (num_output_channels) {
    default: {
      for (int k = 2; k < num_output_channels; k++) {
        for (int i = k, j = k*block_size; i < output_bufferLength; i+=num_output_channels, j++) {
          output_buffers[i] = (short) (foutput_buffers[j] * 32767.0f);
        }
      } // allow fallthrough
    }
    case 2: {
      for (int i = 1, j = block_size; i < output_bufferLength; i+=num_output_channels, j++) {
        output_buffers[i] = (short) (foutput_buffers[j] * 32767.0f);
      } // allow fallthrough
    }
    case 1: {
      for (int i = 0, j = 0; i < output_bufferLength; i+=num_output_channels, j++) {
        output_buffers[i] = (short) (foutput_buffers[j] * 32767.0f);
      } // allow fallthrough
    }
    case 0: break;
  }
  #endif
}

void *zg_context_get_userinfo(pd::Context *context) {
  return context->callback_user_data;
}

ZGGraph *zg_context_get_graphs(ZGContext *context, unsigned int *n) {
  // TODO(mhroth): implement this
  *n = 0;
  return NULL;
}

void zg_context_register_external_object(ZGContext *context, const char *object_label,
    ZGObject *(*factory)(ZGMessage *message, ZGGraph *graph)) {
  context->register_external_object(object_label, factory);
}

void zg_context_unregister_external_object(ZGContext *context, const char *object_label) {
  context->unregister_external_object(object_label);
}


#pragma mark - Objects from Context

ZGObject *zg_context_get_table_for_name(ZGObject *table, const char *name) {
  return NULL; // TODO(mhroth): implement this
}


#pragma mark - Context Un/Register External Receivers

void zg_context_register_receiver(ZGContext *context, const char *receiver_name) {
  context->register_external_receiver(receiver_name);
}

void zg_context_unregister_receiver(ZGContext *context, const char *receiver_name) {
  context->unregister_external_receiver(receiver_name);
}


#pragma mark - Context Send Message

/** Send a message to the named receiver. */
void zg_context_send_message(ZGContext *context, const char *receiver_name, ZGMessage *message) {
  context->schedule_external_message(receiver_name, message);
}

void zg_context_send_message_from_string(ZGContext *context, const char *receiver_name,
    double timestamp, const char *initString) {
  context->schedule_external_message(receiver_name, timestamp, initString);
}

void zg_context_send_messageV(pd::Context *context, const char *receiver_name, double timestamp,
    const char *messageFormat, ...) {
  va_list ap;
  va_start(ap, messageFormat);
  context->schedule_external_message_v(receiver_name, 0.0, messageFormat, ap);
  va_end(ap); // release the va_list
}

void zg_context_send_message_at_blockindex(pd::Context *context, const char *receiver_name, double blockIndex,
    const char *messageFormat, ...) {
  va_list ap;
  va_start(ap, messageFormat);
  double timestamp = context->get_block_start_timestamp();
  if (blockIndex >= 0.0 && blockIndex <= (double) (context->get_block_size()-1)) {
    timestamp += blockIndex / context->get_sample_rate();
  }
  context->schedule_external_message_v(receiver_name, timestamp, messageFormat, ap);
  va_end(ap);
}

void zg_context_send_midinote(pd::Context *context, int channel, int noteNumber, int velocity, double blockIndex) {
  char receiver_name[snprintf(NULL, 0, "zg_notein_%i", channel)+1];
  snprintf(receiver_name, sizeof(receiver_name), "zg_notein_%i", channel);

  zg_context_send_message_at_blockindex(context, receiver_name, blockIndex, "fff",
      (float) noteNumber, (float) velocity, (float) channel);

  // all message are also sent to the omni listener
  zg_context_send_message_at_blockindex(context, "zg_notein_omni", blockIndex, "fff",
      (float) noteNumber, (float) velocity, (float) channel);
}


#pragma mark - Graph

void zg_graph_attach(ZGGraph *graph) {
  graph->getContext()->attach_graph(graph);
}

void zg_graph_unattach(ZGGraph *graph) {
  graph->getContext()->unattach_graph(graph);
}

void zg_graph_add_connection(ZGGraph *graph, ZGObject *fromObject, int outlet_index, ZGObject *toObject, int inlet_index) {
  graph->addConnection(fromObject, outlet_index, toObject, inlet_index);
}

void zg_graph_remove_connection(ZGGraph *graph, ZGObject *fromObject, int outlet_index, ZGObject *toObject, int inlet_index) {
  graph->removeConnection(fromObject, outlet_index, toObject, inlet_index);
}

unsigned int zg_graph_get_dollar_zero(ZGGraph *graph) {
  return (graph != NULL) ? (unsigned int) graph->getArguments()->get_float(0) : 0;
}

ZGObject **zg_graph_get_objects(ZGGraph *graph, unsigned int *n) {
  list<message::Object *> nodeList = graph->getNodeList();
  list<message::Object *>::iterator it = nodeList.begin();
  list<message::Object *>::iterator end = nodeList.end();
  *n = (unsigned int) nodeList.size();
  ZGObject **nodeArray = (ZGObject **) malloc(nodeList.size() * sizeof(ZGObject *));
  for (unsigned int i = 0; i < *n; i++, it++) {
    nodeArray[i] = *it;
  }
  return nodeArray;
}


#pragma mark - Table

float *zg_table_get_buffer(message::Object *table, unsigned int *n) {
  if (table != NULL && table->get_object_type() == MESSAGE_TABLE) {
    MessageTable *messageTable = reinterpret_cast<MessageTable *>(table);
    int x = 0;
    float *buffer = messageTable->getBuffer(&x);
    *n = x;
    return buffer;
  }
  *n = 0;
  return NULL;
}

void zg_table_set_buffer(message::Object *table, float *buffer, unsigned int n) {
  if (table != NULL && table->get_object_type() == MESSAGE_TABLE)  {
    MessageTable *messageTable = reinterpret_cast<MessageTable *>(table);
    messageTable->get_graph()->lockContextIfAttached();
    float *tableBuffer = messageTable->resizeBuffer(n); // resize the buffer to the new size (if necessary)
    memcpy(tableBuffer, buffer, n*sizeof(float)); // copy the contents of the buffer to the table
    messageTable->get_graph()->unlockContextIfAttached();
  }
}


#pragma mark - Message

ZGMessage *zg_message_new(double timetamp, unsigned int numElements) {
  pd::Message *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(timetamp, numElements);
  return message->clone_on_heap();
}

ZGMessage *zg_message_new_from_string(double timetamp, const char *initString) {
  unsigned int maxElements = (strlen(initString)/2)+1;
  pd::Message *message = PD_MESSAGE_ON_STACK(maxElements);
  // make a local copy of the initString so that strtok in from_string won't break it
  char str[strlen(initString)+1]; strcpy(str, initString);
  // numElements set to correct number after string is parsed
  message->from_string(timetamp, maxElements, str);
  return message->clone_on_heap();
}

void zg_message_delete(pd::Message *message) {
  message->free_message(); // also frees any symbols on the heap
}

void zg_message_set_float(pd::Message *message, unsigned int index, float f) {
  message->set_float(index, f);
}

void zg_message_set_symbol(pd::Message *message, unsigned int index, const char *s) {
  char *symbol = message->get_symbol(index);
  free(symbol); // free it if it is not already NULL
  message->set_symbol(index, utils::copy_string((char *) s));
}

void zg_message_set_bang(pd::Message *message, unsigned int index) {
  message->set_bang(index);
}

unsigned int zg_message_get_num_elements(pd::Message *message) {
  return message->get_num_elements();
}

double zg_message_get_timestamp(pd::Message *message) {
  return message->get_timestamp();
}

ZGMessageElementType zg_message_get_element_type(pd::Message *message, unsigned int index) {
  switch (message->get_type(index)) {
    case FLOAT: return ZG_MESSAGE_ELEMENT_FLOAT;
    case SYMBOL: return ZG_MESSAGE_ELEMENT_SYMBOL;
    default: return ZG_MESSAGE_ELEMENT_BANG;
  }
}

float zg_message_get_float(pd::Message *message, unsigned int index) {
  return message->get_float(index);
}

const char *zg_message_get_symbol(pd::Message *message, unsigned int index) {
  return message->get_symbol(index);
}

char *zg_message_to_string(ZGMessage *message) {
  return message->toString();
}

void zg_context_register_memorymapped_abstraction(ZGContext *context, const char *object_label, const char *abstraction) {
  context->get_abstraction_database()->addAbstraction(object_label, abstraction);
}

void zg_context_unregister_memorymapped_abstraction(ZGContext *context, const char *object_label) {
  context->get_abstraction_database()->removeAbstraction(object_label);
}
