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

#ifndef _PD_CONTEXT_H_
#define _PD_CONTEXT_H_

#include <map>
#include <pthread.h>
#include "OrderedMessageQueue.h"
#include "PdGraph.h"
#include "ZGCallbackFunction.h"

class BufferPool;
class DspCatch;
class DelayReceiver;
class DspDelayWrite;
class DspReceive;
class DspSend;
class DspThrow;
class MessageSendController;
class MessageTable;
class PdFileParser;
class RemoteMessageReceiver;
class TableReceiverInterface;
class PdMessage;
class ObjectFactoryMap;
class PdAbstractionDataBase;

/**
 * The <code>pd::Context</code> is a container for a set of <code>PdGraph</code>s operating in
 * a common environment. The <code>pd::Context</code> tracks all relevent global variables.
 */
class pd::Context {
  
  public:
    pd::Context(int num_input_channels, int num_output_channels, int block_size, float sample_rate,
        void *(*function)(ZGCallbackFunction, void *, void *), void *userData);
    ~pd::Context();
  
    int getNumInputChannels();
    int getNumOutputChannels();
    int getBlockSize();
    float getSampleRate();
  
    /**
     * Attach the given <code>graph</code> to this <code>context</code>, also registering all
     * necessary objects, and computing the dsp object compute order if necessary.
     */
    void attachGraph(PdGraph *graph);
    void unattachGraph(PdGraph *graph);
    
    void process(float *inputBuffers, float *outputBuffers);
  
    void lock() { pthread_mutex_lock(&context_lock); }
    void unlock() { pthread_mutex_unlock(&context_lock); }
  
    /** Globally register a remote message receiver (e.g. [send] or [notein]). */
    void registerRemoteMessageReceiver(RemoteMessageReceiver *receiver);
    void unregisterRemoteMessageReceiver(RemoteMessageReceiver *receiver);
  
    /** Globally register a [receive~] object. Connect to registered [send~] objects with the same name. */
    void registerDspReceive(DspReceive *dspReceive);
    void unregisterDspReceive(DspReceive *dspReceive);
    
    /** Globally register a [send~] object. Connect to registered [receive~] objects with the same name. */
    void registerDspSend(DspSend *dspSend);
    void unregisterDspSend(DspSend *dspSend);
  
    /** Returns the named global <code>DspSend</code> object. */
    DspSend *getDspSend(const char *name);
  
    /** Updates the input buffers all attached objects to the DspReceives associated with the given DspSend. */
    void updateDspReceiveForSendWitBuffer(const char *name, float *buffer);
    
    /**
     * Globally register a [delwrite~] object. Registration is necessary such that they can
     * be connected to [delread~] and [vd~] objects as are they are added to the graph.
     */
    void registerDelayline(DspDelayWrite *delayline);
    
    /** Returns the named global <code>DspDelayWrite</code> object. */
    DspDelayWrite *getDelayline(const char *name);
    
    /**
     * Globally register a [delread~] or [vd~] object. Registration is necessary such that they can
     * be connected to [delwrite~] objects are they are added to the graph.
     */
    void registerDelayReceiver(DelayReceiver *delayReceiver);
    
    void registerDspThrow(DspThrow *dspThrow);
    
    void registerDspCatch(DspCatch *dspCatch);
    
    void registerTable(MessageTable *table);
    
    void registerTableReceiver(TableReceiverInterface *tableReceiver);
    void unregisterTableReceiver(TableReceiverInterface *tableReceiver);
    
    MessageTable *getTable(const char *name);
    
    /** Returns the named global <code>DspCatch</code> object. */
    DspCatch *getDspCatch(const char *name);
    
    /**
     * Sends the given message to all [receive] objects with the given <code>name</code>.
     * This function is used by message boxes to send messages described be the syntax:
     * ;
     * name message;
     */
    void sendMessageToNamedReceivers(char *name, PdMessage *message);
  
    /**
     * Schedules a message to be sent to all receivers at the start of the next block.
     * @returns The <code>PdMessage</code> which will be sent. It is intended that the programmer
     * will set the values of the message with a call to <code>setMessage()</code>.
     */
    void scheduleExternalMessageV(const char *receiverName, double timestamp,
        const char *messageFormat, va_list ap);
  
    /** Schedules a message to be sent to all receivers at the start of the next block. */
    void scheduleExternalMessage(const char *receiverName, PdMessage *message);
  
    /**
     * Schedules a message described by the given string to be sent to named receivers at the
     * given timestamp.
     */
    void scheduleExternalMessage(const char *receiverName, double timestamp,
        const char *initString);
  
    /**
     * Schedules a <code>PdMessage</code> to be sent by the <code>MessageObject</code> from the
     * <code>outlet_index</code> at the specified <code>time</code>. The message will be copied
     * to the heap and the context will thereafter take over ownership and be responsible for
     * freeing it. The pointer to the heap-message is returned.
     */
    PdMessage *schedule_message(MessageObject *messageObject, unsigned int outlet_index, PdMessage *message);
  
    /**
     * Cancel a scheduled <code>PdMessage</code> according to its id. The message memory will
     * be freed.
     */
    void cancelMessage(MessageObject *messageObject, int outlet_index, PdMessage *message);
  
    /** Receives and processes messages sent to the Pd system by sending to "pd". */
    void receiveSystemMessage(PdMessage *message);
  
    /** Returns the global dsp buffer at the given inlet. Exclusively used by <code>DspAdc</code>. */
    float *getGlobalDspBufferAtInlet(int inlet_index);
    
    /** Returns the global dsp buffer at the given outlet. Exclusively used by <code>DspDac</code>. */
    float *getGlobalDspBufferAtOutlet(int outlet_index);
  
    /** Returns the timestamp of the beginning of the current block. */
    double getBlockStartTimestamp();
    
    /** Returns the duration in milliseconds of one block. */
    double getBlockDuration();
  
    /** Prints the given message to error output. */
    void printErr(char *msg);
    void printErr(const char *msg, ...);
    
    /** Prints the given message to standard output. */
    void printStd(char *msg);
    void printStd(const char *msg, ...);
  
    /** Returns the next globally unique graph id. */
    int getNextGraphId();
  
    /** Used with MessageValue for keeping track of global variables. */
    void setValueForName(const char *name, float constant);
    float getValueForName(const char *name);
  
    /** Create a new object in a graph. */
    MessageObject *new_object(const char *objectLabel, PdMessage *init_message, PdGraph *graph);
  
    void registerExternalReceiver(const char *receiverName);
    void unregisterExternalReceiver(const char *receiverName);
  
    /** User-provided data associated with the callback function. */
    void *callbackUserData;
  
    /** The registered callback function for sending data outside of the graph. */
    void *(*callbackFunction)(ZGCallbackFunction, void *, void *);
  
    /** Register an object label and its associated factory method. */
    void registerExternalObject(const char *objectLabel,
        MessageObject *(*new_object)(PdMessage *, PdGraph *));
  
    /** Unregister an object label. */
    void unregisterExternalObject(const char *objectLabel);
  
    BufferPool *getBufferPool() { return bufferPool; }

    PdAbstractionDataBase *getAbstractionDataBase();
  
  private:
    /** Returns <code>true</code> if the graph was successfully configured. <code>false</code> otherwise. */
    bool configureEmptyGraphWithParser(PdGraph *graph, PdFileParser *fileParser);
  
    void initObjectInitMap();

    int num_input_channels;
    int num_output_channels;
    int block_size;
    float sample_rate;
  
    /** Keeps track of the current global graph id. */
    unsigned int global_graph_id;
  
    /** A list of all top-level graphs in this context. */
    vector<PdGraph *> graph_list;
  
    /** A thread lock used to access critical sections of this context. */
    pthread_mutex_t context_lock;
    
    int numBytesInInputBuffers;
    int numBytesInOutputBuffers;
    
    float *globalDspInputBuffers;
    float *globalDspOutputBuffers;
  
    /** A message queue keeping track of all scheduled messages. */
    OrderedMessageQueue *messageCallbackQueue;
  
    /** The start of the current block in milliseconds. */
    double blockStartTimestamp;
    
    /** The duration of one block in milliseconds. */
    double blockDurationMs;
  
    /** The global send controller. */
    MessageSendController *sendController;
  
    /** A global list of all [send~] objects. */
    list<DspSend *> dspSendList;
    
    /** A global list of all [receive~] objects. */
    map<string, list<DspReceive *> > dspReceiveMap;
    
    /** A global list of all [delwrite~] objects. */
    list<DspDelayWrite *> delaylineList;
    
    /** A global list of all [delread~] and [vd~] objects. */
    list<DelayReceiver *> delayReceiverList;
    
    /** A global list of all [throw~] objects. */
    list<DspThrow *> throwList;
    
    /** A global list of all [catch~] objects. */
    list<DspCatch *> catchList;
    
    /** A global list of all [table] objects. */
    list<MessageTable *> tableList;
    
    /** A global list of all table receivers (e.g., [tabread4~] and [tabplay~]) */
    list<TableReceiverInterface *> tableReceiverList;
  
    ObjectFactoryMap *objectFactoryMap;
  
    BufferPool *bufferPool;
  
    /** A global map storing values for Value objects. */
    map<string,float> valueMap;

    PdAbstractionDataBase *abstractionDatabase;
};

#endif // _PD_CONTEXT_H_
