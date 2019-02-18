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

#include "BufferPool.h"
#include "MessageSendController.h"
#include "ObjectFactoryMap.h"
#include "PdAbstractionDataBase.h"
#include "PdContext.h"
#include "PdFileParser.h"

#include "DelayReceiver.h"
#include "DspCatch.h"
#include "DspDelayWrite.h"
#include "DspReceive.h"
#include "DspSend.h"
#include "DspThrow.h"
#include "MessageMessageBox.h"
#include "MessageFloat.h"
#include "MessageSymbol.h"
#include "MessageTable.h"
#include "TableReceiverInterface.h"

#pragma mark Constructor/Deconstructor

pd::Context::pd::Context(int num_input_channels, int num_output_channels, int block_size, float sample_rate,
    void *(*function)(ZGCallbackFunction, void *, void *), void *userData) {
  this->num_input_channels = num_input_channels;
  this->num_output_channels = num_output_channels;
  this->block_size = block_size;
  this->sample_rate = sample_rate;
  callbackFunction = function;
  callbackUserData = userData;
  blockStartTimestamp = 0.0;
  blockDurationMs = ((double) block_size / (double) sample_rate) * 1000.0;
  messageCallbackQueue = new OrderedMessageQueue();
  objectFactoryMap = new ObjectFactoryMap();
  global_graph_id = 0;
  bufferPool = new BufferPool(block_size);

  numBytesInInputBuffers = block_size * num_input_channels * sizeof(float);
  numBytesInOutputBuffers = block_size * num_output_channels * sizeof(float);
  globalDspInputBuffers = (numBytesInInputBuffers > 0) ? ALLOC_ALIGNED_BUFFER(numBytesInInputBuffers) : NULL;
  memset(globalDspInputBuffers, 0, numBytesInInputBuffers);
  globalDspOutputBuffers = (numBytesInOutputBuffers > 0) ? ALLOC_ALIGNED_BUFFER(numBytesInOutputBuffers) : NULL;
  memset(globalDspOutputBuffers, 0, numBytesInOutputBuffers);

  sendController = new MessageSendController(this);

  abstractionDatabase = new PdAbstractionDataBase();

  // configure the context lock, which is recursive
  pthread_mutexattr_t mta;
  pthread_mutexattr_init(&mta);
  pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&context_lock, &mta);
}

pd::Context::~pd::Context() {
  FREE_ALIGNED_BUFFER(globalDspInputBuffers);
  FREE_ALIGNED_BUFFER(globalDspOutputBuffers);

  delete messageCallbackQueue;
  delete sendController;
  delete objectFactoryMap;
  delete bufferPool;

  // delete all of the PdGraphs in the graph list
  for (int i = 0; i < graph_list.size(); i++) {
    delete graph_list[i];
  }

  delete abstractionDatabase;

  pthread_mutex_destroy(&context_lock);
}


#pragma mark - External Object Management

void pd::Context::registerExternalObject(const char *objectLabel,
    message::Object *(*objFactory)(pd::Message *, PdGraph *)) {
  objectFactoryMap->registerExternalObject(objectLabel, objFactory);
}

void pd::Context::unregisterExternalObject(const char *objectLabel) {
  objectFactoryMap->unregisterExternalObject(objectLabel);
}


#pragma mark - Get Context Attributes

int pd::Context::getNumInputChannels() {
  return num_input_channels;
}

int pd::Context::getNumOutputChannels() {
  return num_output_channels;
}

int pd::Context::getBlockSize() {
  return block_size;
}

float pd::Context::getSampleRate() {
  return sample_rate;
}

float *pd::Context::getGlobalDspBufferAtInlet(int inlet_index) {
  return globalDspInputBuffers + (inlet_index * block_size);
}

float *pd::Context::getGlobalDspBufferAtOutlet(int outlet_index) {
  return globalDspOutputBuffers + (outlet_index * block_size);
}

double pd::Context::getBlockStartTimestamp() {
  return blockStartTimestamp;
}

double pd::Context::getBlockDuration() {
  return blockDurationMs;
}

int pd::Context::getNextGraphId() {
  return ++global_graph_id;
}


#pragma mark - process

void pd::Context::process(float *inputBuffers, float *outputBuffers) {
  lock(); // lock the context

  // set up adc~ buffers
  memcpy(globalDspInputBuffers, inputBuffers, numBytesInInputBuffers);

  // clear the global output audio buffers so that dac~ nodes can write to it
  memset(globalDspOutputBuffers, 0, numBytesInOutputBuffers);

  // Send all messages for this block
  ObjectMessageConnection omlPair;
  double nextBlockStartTimestamp = blockStartTimestamp + blockDurationMs;
  while (!messageCallbackQueue->empty() &&
      (omlPair = messageCallbackQueue->peek()).second.first->get_timestamp() < nextBlockStartTimestamp) {

    messageCallbackQueue->pop(); // remove the message from the queue

    message::Object *object = omlPair.first;
    pd::Message *message = omlPair.second.first;
    unsigned int outlet_index = omlPair.second.second;
    if (message->get_timestamp() < blockStartTimestamp) {
      // messages injected into the system with a timestamp behind the current block are automatically
      // rescheduled for the beginning of the current block. This is done in order to normalise
      // the treament of messages, but also to avoid difficulties in cases when messages are scheduled
      // in subgraphs with different block sizes.
      message->set_timestamp(blockStartTimestamp);
    }

    object->send_message(outlet_index, message);
    message->freeMessage(); // free the message now that it has been sent and processed
  }

  switch (graph_list.size()) {
    case 0: break;
    case 1: graph_list.front()->processFunction(graph_list.front(), 0, 0); break;
    default: {
      int numGraphs = graph_list.size();
      PdGraph **graph = &graph_list.front();
      for (int i = 0; i < numGraphs; ++i) {
        graph[i]->processFunction(graph[i], 0, 0);
      }
    }
  }

  blockStartTimestamp = nextBlockStartTimestamp;

  // copy the output audio to the given buffer
  memcpy(outputBuffers, globalDspOutputBuffers, numBytesInOutputBuffers);

  unlock(); // unlock the context
}


#pragma mark - Un/Attach Graph

void pd::Context::attachGraph(PdGraph *graph) {
  lock();
  graph_list.push_back(graph);
  graph->attachToContext(true);
  graph->computeDeepLocalDspProcessOrder();
  unlock();
}

void pd::Context::unattachGraph(PdGraph *graph) {
  lock();
  graph_list.erase(std::remove(graph_list.begin(), graph_list.end(), graph),
    graph_list.end());
  graph->attachToContext(false);
  unlock();
}


#pragma mark - New Object

message::Object *pd::Context::new_object(const char *objectLabel, pd::Message *init_message, PdGraph *graph) {
  message::Object *message_obj = objectFactoryMap->new_object(objectLabel, init_message, graph);
  if (message_obj != NULL) {
    return message_obj;
  } else if(utils::is_numeric(objectLabel)) {
    // special case for constructing a float object from a number
    pd::Message *initMsg = PD_MESSAGE_ON_STACK(1);
    initMsg->from_timestamp_and_float(0.0, atof(objectLabel));
    return objectFactoryMap->new_object("float", initMsg, graph);
  } else {
    return NULL; // unknown object
  }
}


#pragma mark - PrintStd/PrintErr

void pd::Context::printErr(char *msg) {
  if (callbackFunction != NULL) {
    callbackFunction(ZG_PRINT_ERR, callbackUserData, msg);
  }
}

void pd::Context::printErr(const char *msg, ...) {
  char stringBuffer[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, sizeof(stringBuffer), msg, ap);
  va_end(ap);

  printErr(stringBuffer);
}

void pd::Context::printStd(char *msg) {
  if (callbackFunction != NULL) {
    callbackFunction(ZG_PRINT_STD, callbackUserData, msg);
  }
}

void pd::Context::printStd(const char *msg, ...) {
  char stringBuffer[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, sizeof(stringBuffer), msg, ap);
  va_end(ap);

  printStd(stringBuffer);
}


#pragma mark - Register/Unregister DspSend/Receive

void pd::Context::registerDspReceive(DspReceive *dspReceive) {
  // NOTE(mhroth): no duplicate check is made for dspReceive
  list<DspReceive *> *receiveList = &(dspReceiveMap[string(dspReceive->getName())]);
  receiveList->push_back(dspReceive);

  // connect receive~ to associated send~
  DspSend *dspSend = getDspSend(dspReceive->getName());
  if (dspSend != NULL) {
    dspReceive->setDspBufferAtInlet(dspSend->getDspBufferAtInlet(0), 0);
  }
}

void pd::Context::unregisterDspReceive(DspReceive *dspReceive) {
  list<DspReceive *> *receiveList = &(dspReceiveMap[string(dspReceive->getName())]);
  receiveList->remove(dspReceive);
  dspReceive->setDspBufferAtInlet(dspReceive->get_graph()->getBufferPool()->getZeroBuffer(), 0);
}

void pd::Context::registerDspSend(DspSend *dspSend) {
  DspSend *sendObject = getDspSend(dspSend->getName());
  if (sendObject != NULL) {
    printErr("Duplicate send~ object found with name \"%s\".", dspSend->getName());
    return;
  }
  dspSendList.push_back(dspSend);

  // connect associated receive~s to send~.
  updateDspReceiveForSendWitBuffer(dspSend->getName(), dspSend->getDspBufferAtOutlet(0));
}

void pd::Context::unregisterDspSend(DspSend *dspSend) {
  dspSendList.remove(dspSend);

  // inform all previously connected receive~s that the send~ buffer does not exist anymore.
  updateDspReceiveForSendWitBuffer(dspSend->getName(), dspSend->get_graph()->getBufferPool()->getZeroBuffer());
}

DspSend *pd::Context::getDspSend(const char *name) {
  for (list<DspSend *>::iterator it = dspSendList.begin(); it != dspSendList.end(); ++it) {
    if (!strcmp((*it)->getName(), name)) return *it;
  }
  return NULL;
}

void pd::Context::updateDspReceiveForSendWitBuffer(const char *name, float *buffer) {
  list<DspReceive *> receiveList = dspReceiveMap[string(name)];
  for (list<DspReceive *>::iterator it = receiveList.begin(); it != receiveList.end(); ++it) {
    DspReceive *dspReceive = *it;
    dspReceive->setDspBufferAtInlet(buffer, 0);
  }
}


#pragma mark - Register/Unregister Objects

void pd::Context::registerRemoteMessageReceiver(RemoteMessageReceiver *receiver) {
  sendController->addReceiver(receiver);
}

void pd::Context::unregisterRemoteMessageReceiver(RemoteMessageReceiver *receiver) {
  sendController->removeReceiver(receiver);
}

void pd::Context::registerDelayline(DspDelayWrite *delayline) {
  if (getDelayline(delayline->getName()) != NULL) {
    printErr("delwrite~ with duplicate name \"%s\" registered.", delayline->getName());
    return;
  }
  delaylineList.push_back(delayline);

  // connect this delayline to all same-named delay receivers
  for (list<DelayReceiver *>::iterator it = delayReceiverList.begin(); it != delayReceiverList.end(); it++) {
    if (!strcmp((*it)->getName(), delayline->getName())) (*it)->setDelayline(delayline);
  }
}

void pd::Context::registerDelayReceiver(DelayReceiver *delayReceiver) {
  delayReceiverList.push_back(delayReceiver);

  // connect the delay receiver to the named delayline
  DspDelayWrite *delayline = getDelayline(delayReceiver->getName());
  delayReceiver->setDelayline(delayline);
}

DspDelayWrite *pd::Context::getDelayline(const char *name) {
  for (list<DspDelayWrite *>::iterator it = delaylineList.begin(); it != delaylineList.end(); ++it) {
    if (!strcmp((*it)->getName(), name)) return *it;
  }
  return NULL;
}

void pd::Context::registerDspThrow(DspThrow *dspThrow) {
  // NOTE(mhroth): no duplicate testing for the same object more than once
  throwList.push_back(dspThrow);

  DspCatch *dspCatch = getDspCatch(dspThrow->getName());
  if (dspCatch != NULL) {
    dspCatch->addThrow(dspThrow);
  }
}

void pd::Context::registerDspCatch(DspCatch *dspCatch) {
  DspCatch *catchObject = getDspCatch(dspCatch->getName());
  if (catchObject != NULL) {
    printErr("catch~ with duplicate name \"%s\" already exists.", dspCatch->getName());
    return;
  }
  catchList.push_back(dspCatch);

  // connect catch~ to all associated throw~s
  for (list<DspThrow *>::iterator it = throwList.begin(); it != throwList.end(); it++) {
    if (!strcmp((*it)->getName(), dspCatch->getName())) dspCatch->addThrow((*it));
  }
}

DspCatch *pd::Context::getDspCatch(const char *name) {
  for (list<DspCatch *>::iterator it = catchList.begin(); it != catchList.end(); it++) {
    if (!strcmp((*it)->getName(), name)) return (*it);
  }
  return NULL;
}

void pd::Context::registerTable(MessageTable *table) {
  if (getTable(table->getName()) != NULL) {
    printErr("Table with name \"%s\" already exists.", table->getName());
    return;
  }
  tableList.push_back(table);

  for (list<TableReceiverInterface *>::iterator it = tableReceiverList.begin();
      it != tableReceiverList.end(); it++) {
    // in case the table receiver doesn't have the table name yet
    if ((*it)->getName() == NULL)
      continue;
    if (!strcmp((*it)->getName(), table->getName())) (*it)->setTable(table);
  }
}

MessageTable *pd::Context::getTable(const char *name) {
  for (list<MessageTable *>::iterator it = tableList.begin(); it != tableList.end(); it++) {
    if (!strcmp((*it)->getName(), name)) return (*it);
  }
  return NULL;
}

void pd::Context::registerTableReceiver(TableReceiverInterface *tableReceiver) {
  tableReceiverList.push_back(tableReceiver); // add the new receiver

  // in case the tableread doesnt have the name of the table yet
  if (tableReceiver->getName()) {
    MessageTable *table = getTable(tableReceiver->getName());
    tableReceiver->setTable(table); // set table whether it is NULL or not
  }
}

void pd::Context::unregisterTableReceiver(TableReceiverInterface *tableReceiver) {
  tableReceiverList.remove(tableReceiver); // remove the receiver
  tableReceiver->setTable(NULL);
}

void pd::Context::setValueForName(const char *name, float constant) {
  valueMap[string(name)] = constant;
}

float pd::Context::getValueForName(const char *name) {
  return valueMap[string(name)];
}

void pd::Context::registerExternalReceiver(const char *receiverName) {
  lock(); // don't update the external receiver registry while processing it, of course!
  sendController->registerExternalReceiver(receiverName);
  unlock();
}

void pd::Context::unregisterExternalReceiver(const char *receiverName) {
  lock();
  sendController->unregisterExternalReceiver(receiverName);
  unlock();
}


#pragma mark - Manage Messages

void pd::Context::send_messageToNamedReceivers(char *name, pd::Message *message) {
  sendController->receive_message(name, message);
}

void pd::Context::scheduleExternalMessageV(const char *receiverName, double timestamp,
    const char *messageFormat, va_list ap) {
  int numElements = strlen(messageFormat);
  pd::Message *message = PD_MESSAGE_ON_STACK(numElements);
  message->from_timestamp(timestamp, numElements);
  for (int i = 0; i < numElements; i++) { // format message
    switch (messageFormat[i]) {
      case 'f': message->set_float(i, (float) va_arg(ap, double)); break;
      case 's': message->set_symbol(i, (char *) va_arg(ap, char *)); break;
      case 'b': message->set_bang(i); break;
      default: break;
    }
  }

  scheduleExternalMessage(receiverName, message);
}

void pd::Context::scheduleExternalMessage(const char *receiverName, pd::Message *message) {
  lock();
  int receiverNameIndex = sendController->getNameIndex(receiverName);
  if (receiverNameIndex >= 0) { // if the receiver exists
    schedule_message(sendController, receiverNameIndex, message);
  }
  unlock();
}

void pd::Context::scheduleExternalMessage(const char *receiverName, double timestamp, const char *initString) {
  // do the heavy lifting of string parsing before the lock (minimise the critical section)
  int maxElements = (strlen(initString)/2)+1;
  pd::Message *message = PD_MESSAGE_ON_STACK(maxElements);
  char str[strlen(initString)+1]; strcpy(str, initString);
  message->from_string(timestamp, maxElements, str);

  lock(); // lock and load
  int receiverNameIndex = sendController->getNameIndex(receiverName);
  if (receiverNameIndex >= 0) { // if the receiver exists
    schedule_message(sendController, receiverNameIndex, message);
  }
  unlock();
}

pd::Message *pd::Context::schedule_message(message::Object *message_obj, unsigned int outlet_index, pd::Message *message) {
  // basic argument checking. It may happen that the message is NULL in case a cancel message
  // is sent multiple times to a particular object, when no message is pending
  if (message != NULL && message_obj != NULL) {
    message = message->clone_on_heap();
    messageCallbackQueue->insertMessage(message_obj, outlet_index, message);
    return message;
  }
  return NULL;
}

void pd::Context::cancelMessage(message::Object *message_obj, int outlet_index, pd::Message *message) {
  if (message != NULL && outlet_index >= 0 && message_obj != NULL) {
    messageCallbackQueue->removeMessage(message_obj, outlet_index, message);
    message->freeMessage();
  }
}

void pd::Context::receiveSystemMessage(pd::Message *message) {
  // TODO(mhroth): What are all of the possible system messages?
  if (message->is_symbol_str(0, "obj")) {
    // TODO(mhroth): dynamic patching
  } else if (callbackFunction != NULL) {
    if (message->is_symbol_str(0, "dsp") && message->is_float(1)) {
      int result = (message->get_float(1) != 0.0f) ? 1 : 0;
      callbackFunction(ZG_PD_DSP, callbackUserData, &result);
    }
  } else {
    char *messageString = message->toString();
    printErr("Unrecognised system command: %s", messageString);
    free(messageString);
  }
}

PdAbstractionDataBase *pd::Context::getAbstractionDataBase() {
  return abstractionDatabase;
}
