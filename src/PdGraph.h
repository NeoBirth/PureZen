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

#ifndef _PD_GRAPH_H_
#define _PD_GRAPH_H_

#include "DspObject.h"
#include "message::OrderedQueue.h"

class BufferPool;
class DeclareList;
class DelayReceiver;
class DspCatch;
class DspDelayWrite;
class DspReceive;
class DspSend;
class DspThrow;
class LetInterface;
class MessageObject;
class MessageReceive;
class message::Send;
class MessageTable;
class pd::Context;

class PdGraph : public DspObject {

  public:
    /** Instantiate an empty graph. */
    PdGraph(PdMessage *init_message, PdGraph *graph, pd::Context *context, int graphId, const char *graphName);
    ~PdGraph();

    /**
     * Schedules a <code>PdMessage</code> to be sent by the <code>MessageObject</code> from the
     * <code>outlet_index</code> at the specified <code>time</code>.
     */
    PdMessage *schedule_message(MessageObject *messageObject, int outlet_index, PdMessage *message);

    /** Cancel a scheduled <code>PdMessage</code> according to its id. */
    void cancel_message(MessageObject *messageObject, int outlet_index, PdMessage *message);

    /*
     * Messages arriving at <code>PdGraph</code>s are processed immediately (passed on to inlet
     * objects, unlike with super-<code>DspObject</code> objects.
     */
    void receive_message(int inlet_index, PdMessage *message);


#pragma mark - Add/Remove Connection

    /** Connect the given <code>MessageObject</code>s from the given outlet to the given inlet. */
    void addConnection(int fromObjectIndex, int outlet_index, int toObjectIndex, int inlet_index);
    void addConnection(MessageObject *fromObject, int outlet_index, MessageObject *toObject, int inlet_index);
    void removeConnection(MessageObject *fromObject, int outlet_index, MessageObject *toObject, int inlet_index);


#pragma mark - Add/Remove Connection to/from Object

    void addConnectionFromObjectToInlet(MessageObject *messageObject, int outlet_index, int inlet_index);
    void addConnectionToObjectFromOutlet(MessageObject *messageObject, int inlet_index, int outlet_index);
    void removeConnectionFromObjectToInlet(MessageObject *messageObject, int outlet_index, int inlet_index);
    void removeConnectionToObjectFromOutlet(MessageObject *messageObject, int inlet_index, int outlet_index);

#pragma mark - Get/Set Buffers

    bool canSetBufferAtOutlet(unsigned int outlet_index) { return false; }
    void set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index);
    void setDspBufferAtOutlet(float *buffer, unsigned int outlet_index);
    float *get_dsp_buffer_at_inlet(int inlet_index);
    float *get_dsp_buffer_at_outlet(int outlet_index);


#pragma mark -

    static const char *get_object_label() { return "pd"; }
    object::Type get_object_type() { return object::Type::PURE_DATA; }
    string toString() { return name.empty() ? string(get_object_label()) : name; }

    connection::Type get_connection_type(int outlet_index);

    bool doesProcessAudio();

    /** Turn the audio processing of this graph on or off. */
    void setSwitch(bool switched);

    /** Returns <code>true</code> if the audio processing of this graph is turned on. <code>false</code> otherwise. */
    bool isSwitchedOn();

    /** Set the current block size of this subgraph. */
    void setBlockSize(int block_size);

    /** Get the current block size of this subgraph. */
    int get_block_size();

    /** Returns <code>true</code> of this graph has no parents, code>false</code> otherwise. */
    bool isRootGraph();

    /** Returns this graph's parent graph. Returns <code>NULL</code> if this graph is a top-level graph. */
    PdGraph *getParentGraph();

    /** Prints the given message to error output. */
    void printErr(const char *msg, ...);

    /** Prints the given message to standard output. */
    void printStd(const char *msg, ...);

    /** Get the argument list in the form of a <code>PdMessage</code> from the graph. */
    PdMessage *getArguments();

    /** Returns the global sample rate. */
    float get_sample_rate();

    /** Returns the global dsp buffer at the given inlet. Exclusively used by <code>DspAdc</code>. */
    float *get_global_dsp_buffer_at_inlet(int inlet_index);

    /** Returns the global dsp buffer at the given outlet. Exclusively used by <code>DspDac</code>. */
    float *get_global_dsp_buffer_at_outlet(int outlet_index);

    int get_num_input_channels();
    int get_num_output_channels();

    /** A convenience function to determine when in a block a message occurs. */
    double getBlockIndex(PdMessage *message);

    /** Returns the graphId of this graph. */
    int getGraphId();

    /** Computes the local tree and node processing ordering for dsp nodes, including subgraphs. */
    void computeDeepLocalDspProcessOrder();

    /**
     * Get the process order as if this object (i.e. graph) were an atomic object. The internal
     * process order is not changed.
     */
    list<DspObject *> getProcessOrder();
    bool isLeafNode();

    /**
     * Sends the given message to all [receive] objects with the given <code>name</code>.
     * This function is used by message boxes to send messages described be the syntax:
     * ;
     * name message;
     */
    void sendMessageToNamedReceivers(char *name, PdMessage *message);

    /** Gets the named (global) table object. */
    MessageTable *get_table(char *name);

    /** Add an object to the graph, taking care of any special object registration. */
    void addObject(float canvasX, float canvasY, MessageObject *node);

    /**
     * Remove the object from the graph, also removing all of the connections to and from ths object.
     * The object is then deleted. The object reference is invalid after calling this function and
     * it is an error to reuse it.
     */
    void removeObject(MessageObject *object);

    void attachToContext(bool isAttached);

    /**
     * Searches all declared paths to find a file matching the given name. The given filename
     * should be a relative path, NOT a full path.
     * Returns the directory in which the file was found, or NULL if nothing could be found.
     * The returned string SHOULD NOT be free()ed by the caller. It belongs to the DeclareList.
     */
    string findFilePath(const char *filename);

    /**
     * Resolves the full path of the given file. If the file is already fully specified then a copy
     * of the string is returned. Otherwise all declared paths are searched and the full path is
     * returned. The returned path SHOULD be freed by the caller.
     */
    char *resolveFullPath(const char *filename);

    /**
     * Adds a full or partial path to the declare list. If it is a relative path, then it will be
     * resolved relative to the path of the abstraction. If this graph is a subgraph (not an
     * abstraction), then the path will be handed off to the parent graph to be handled.
     */
    void addDeclarePath(const char *path);

    /** Used with MessageValue for keeping track of global variables. */
    // TODO(mhroth): these are not yet fully implemented
    void set_value_for_name(const char *name, float constant);
    float get_value_for_name(const char *name);

    unsigned int getNumInlets();
    unsigned int getNumOutlets();

    /** Returns the context with which this graph is associated. */
    pd::Context *getContext();

    /** Returns this PdGraph's node list. */
    list<MessageObject *> getNodeList();

    list<ObjectConnection> getIncomingConnections(unsigned int inlet_index);
    list<ObjectConnection> getOutgoingConnections(unsigned int outlet_index);

    /** Locks the context if this graph is attached. */
    void lockContextIfAttached();

    /** Unlocks the context if this graph is attached. */
    void unlockContextIfAttached();

    BufferPool *get_buffer_pool();

    /** Set the graph name. */
    void setName(string newName) { name = newName; }

  private:
    static void processGraph(DspObject *dspObject, int fromIndex, int toIndex);

    /** Create a new object based on its initialisation string. */
    MessageObject *new_object(char *objectType, char *object_label, PdMessage *init_message, PdGraph *graph);

    /**
     * Registers the object with all relevant global lists.
     * Does not check if the object is already registered.
     */
    void registerObject(MessageObject *messageObject);
    void unregisterObject(MessageObject *messageObject);

    void addLetObjectToLetList(MessageObject *inletObject, float newPosition, vector<MessageObject *> *letList);

    /** The <code>pd::Context</code> to which this graph belongs. */
    pd::Context *context;

    /**
     * A boolean indicating if this graph is currently attached to a context. It is automatically
     * updated when this graph or a super graph is added or removed from the context.
     */
    bool isAttachedToContext;

    /** The unique id for this subgraph. Defines "$0". */
    int graphId;

    /** The list of arguments to the graph. Stored as a <code>PdMessage</code> for simplicity. */
    PdMessage *graphArguments;

    /** True if the graph is switch on and should process audio. False otherwise. */
    bool switched;

    /** The parent graph. NULL if this graph is the root. */
    PdGraph *parentGraph;

    /** A list of <i>all</i> <code>MessageObject</code>s in this subgraph.  */
    list<MessageObject *> nodeList;

    /**
     * A list of all <code>DspObject</code>s in this graph, in the order in which they should be
     * called in the <code>processFunction()</code> loop.
     */
    list<DspObject *> dspNodeList;

    /** A list of all inlet (message or audio) nodes in this subgraph. */
    vector<MessageObject *> inletList; // in fact contains only MessageInlet and DspInlet objects

    /** A list of all outlet (message or audio) nodes in this subgraph. */
    vector<MessageObject *> outletList; // in fact contains only MessageOutlet and DspOutlet objects

    /** A global list of all declared directories (-path and -stdpath) */
    DeclareList *declareList;

    /** PdGraphs may have an associated name, such as their abstraction name. */
    string name;
};

#endif // _PD_GRAPH_H_
