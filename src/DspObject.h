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

#ifndef _DSP_OBJECT_H_
#define _DSP_OBJECT_H_

#include <queue>
#include "ArrayArithmetic.h"
#include "MessageObject.h"

#if __SSE__
// allocate memory aligned to 16-bytes memory boundary
#define ALLOC_ALIGNED_BUFFER(_numBytes) (float *) _mm_malloc(_numBytes, 16)
#define FREE_ALIGNED_BUFFER(_buffer) _mm_free(_buffer)
#else
// NOTE(mhroth): valloc seems to work well, but is deprecated!
#define ALLOC_ALIGNED_BUFFER(_numBytes) (float *) valloc(_numBytes)
#define FREE_ALIGNED_BUFFER(_buffer) free(_buffer)
#endif

typedef std::pair<PdMessage *, unsigned int> MessageConnection;

/**
 * A <code>DspObject</code> is the abstract superclass of any object which processes audio.
 * <code>DspObject</code> is a subclass of <code>MessageObject</code>, such that all of the former
 * can implicitly also process <code>PdMessage</code>s.
 */
class DspObject : public MessageObject {

  public:
    /** The nominal constructor. */
    DspObject(int numMessageInlets, int numDspInlets, int numMessageOutlets, int numDspOutlets, PdGraph *graph);

    /**
     * This constructor is used exclusively by <code>PdGraph</code>.
     * <code>DspObject</code> requires the blocksize in order to instantiate, however <code>PdGraph</code>
     * is a subclass of <code>DspObject</code> and thus the fields of the latter are not yet initialised
     * when the fomer fields are filled in.
     */
    DspObject(int numMessageInlets, int numDspInlets, int numMessageOutlets, int numDspOutlets,
        int block_size, PdGraph *graph);

    virtual ~DspObject();

    virtual void receive_message(int inlet_index, PdMessage *message);

    /* Override MessageObject::shouldDistributeMessageToInlets() */
    virtual bool shouldDistributeMessageToInlets() { return false; }

    /** Process audio buffers in this block. */
    void (*processFunction)(DspObject *dspObject, int fromIndex, int toIndex);

    /** Returns the connection type of the given outlet. */
    virtual connection::Type get_connection_type(int outlet_index);

    /** Get and set buffers at inlets and outlets. */
    virtual void set_dsp_buffer_at_inlet(float *buffer, unsigned int inlet_index);
    virtual void setDspBufferAtOutlet(float *buffer, unsigned int outlet_index);
    virtual float *get_dsp_buffer_at_inlet(int inlet_index);
    virtual float *get_dsp_buffer_at_outlet(int outlet_index);


    /** Return true if a buffer from the Buffer Pool should set set at the given outlet. False otherwise. */
    virtual bool canSetBufferAtOutlet(unsigned int outlet_index) { return true; }

    virtual void addConnectionFromObjectToInlet(MessageObject *messageObject, int outlet_index, int inlet_index);
    virtual void addConnectionToObjectFromOutlet(MessageObject *messageObject, int inlet_index, int outlet_index);
    virtual void removeConnectionFromObjectToInlet(MessageObject *messageObject, int outlet_index, int inlet_index);
    virtual void removeConnectionToObjectFromOutlet(MessageObject *messageObject, int inlet_index, int outlet_index);

    virtual bool doesProcessAudio() { return true; }

    virtual bool isLeafNode();

    virtual list<DspObject *> getProcessOrder();

    virtual unsigned int getNumInlets() {
      return max(incoming_messageConnections.size(), incomingDspConnections.size());
    }
    virtual unsigned int getNumOutlets() {
      return max(outgoing_messageConnections.size(), outgoingDspConnections.size());
    }
    virtual unsigned int getNumDspInlets() { return incomingDspConnections.size(); }
    virtual unsigned int getNumDspOutlets() { return outgoingDspConnections.size(); }

    /**
     * Returns <i>all</i> incoming connections to the given inlet. This includes both message and
     * dsp connections.
     */
    virtual list<ObjectConnection> getIncomingConnections(unsigned int inlet_index);

    /** Returns only incoming dsp connections to the given inlet. */
    virtual list<ObjectConnection> getIncomingDspConnections(unsigned int inlet_index);

    /**
     * Returns <i>all</i> outgoing connections from the given outlet. This includes both message and
     * dsp connections.
     */
    virtual list<ObjectConnection> getOutgoingConnections(unsigned int outlet_index);

    /** Returns only outgoing dsp connections from the given outlet. */
    virtual list<ObjectConnection> getOutgoingDspConnections(unsigned int outlet_index);

    static const char *get_object_label() { return "obj~"; }

  protected:
    static void processFunctionDefaultNoMessage(DspObject *dspObject, int fromIndex, int toIndex);
    static void processFunctionMessage(DspObject *dspObject, int fromIndex, int toIndex);

    /* IMPORTANT: one of these two functions MUST be overridden (or processFunction()) */
    virtual void processDspWithIndex(double fromIndex, double toIndex);
    virtual void processDspWithIndex(int fromIndex, int toIndex);

    /**
     * DspObject subclasses are informed that a connection change has happened to an inlet. A
     * message or signal connection has been added or removed. They may which to reconfigure their
     * (optimised) codepath with this new information.
     */
    virtual void onInletConnectionUpdate(unsigned int inlet_index);

    /** Immediately deletes all messages in the message queue without executing them. */
    void clearMessageQueue();

    // both float and int versions of the blocksize are stored as different internal mechanisms
    // require different number formats
    int block_sizeInt;

    /** The local message queue. Messages that are pending for the next block. */
    queue<MessageConnection> messageQueue;

    /* An array of pointers to resolved dsp buffers at each inlet. */
    float *dspBufferAtInlet[3];

    /* An array of pointers to resolved dsp buffers at each outlet. */
    float *dspBufferAtOutlet[3];

    /** List of all dsp objects connecting to this object at each inlet. */
    vector<list<ObjectConnection> > incomingDspConnections;

    /** List of all dsp objects to which this object connects at each outlet. */
    vector<list<ObjectConnection> > outgoingDspConnections;

    /** The process function to use when acting on a message. */
    void (*processFunctionNoMessage)(DspObject *dspObject, int fromIndex, int toIndex);

  private:
    /** This function encapsulates the common code between the two constructors. */
    void init(int numDspInlets, int numDspOutlets, int block_size);
};

#endif // _DSP_OBJECT_H_
