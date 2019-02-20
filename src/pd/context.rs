//
// Copyright © 2009-2019 NeoBirth Developers
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

//! Pure Data context: main API entrypoint and store of all state
//!
//! ## Translation Notes
//!
//! This file was translated from the following ZenGarden C++ sources:
//!
//! - `PdContext.cpp`
//! - `PdContext.h`
//!
//! Original C++ classes/types were renamed/refactored as follows:
//!
//! - `PdContext` => `pd::Context`

use super::{graph, Graph};
use crate::{
    allocator::{Allocator, FixedSizeAllocator},
    error::Error,
    message::{
        self,
        object::{self, connection},
        Timestamp,
    },
    pd,
};
use core::{mem::size_of, str::FromStr, time::Duration};
use heapless::{self, consts::*, ArrayLength};

/// Pure Data context object: main API entrypoint and owner of all state
/// (objects, graphs, etc).
///
/// In an Entity-Component-System (ECS) model, which this library is gradually
/// adopting, this type is the `System` or `ECS` object.
pub struct Context<'pd> {
    /// Number of input channels
    num_input_channels: usize,

    /// Number of output channels
    num_output_channels: usize,

    /// Size of a block to be rendered
    block_size: usize,

    /// Sample rate audio is being rendered at
    sample_rate: f32,

    /// Number of bytes in input buffers
    #[allow(dead_code)]
    num_bytes_in_input_buffers: usize,

    /// Number of bytes in output buffers
    #[allow(dead_code)]
    num_bytes_in_output_buffers: usize,

    /// Global DSP input buffers
    // TODO(tarcieri): real lifetimes or ownership
    global_dsp_input_buffers: &'static mut [f32],

    /// Global DSP output buffers
    // TODO(tarcieri): real lifetimes or ownership
    global_dsp_output_buffers: &'static mut [f32],

    /// Start of the current block in milliseconds
    block_start_timestamp: Timestamp,

    /// Duration of one block
    block_duration: Duration,

    /// Message queue keeping track of all scheduled messages.
    // TODO(tarcieri): queue size chosen arbitrarily. Tune this size.
    message_callback_queue: message::OrderedQueue<'pd, U64>,

    /// Global send controller
    send_controller: message::send::Controller,

    /// The `pd::Context` object owns all `message::Object` values
    // TODO(tarcieri): chosen arbitrarily. Tune this size.
    #[allow(dead_code)]
    object_allocator: FixedSizeAllocator<message::Object, U32>,

    /// The `pd::Context` object owns all `pd::Graph` values
    // TODO(tarcieri): support for more than one `pd::Graph`
    #[allow(dead_code)]
    graph_allocator: FixedSizeAllocator<Graph, U1>,

    /// Current global graph id
    #[allow(dead_code)]
    global_graph_id: pd::graph::Id,

    // Lock around objects that require thread safety
    // TODO(tarcieri): multi-thread support on `std` targets
    // context_lock: Mutex<_>

    // Global list of all `send~` objects
    // TODO(tarcieri): `DspSend`
    // dsp_send_list: List<DspSend>,

    //  Global list of all `receive~` objects
    // TODO(tarcieri): `DspReceive`
    // dsp_receive_map: heapless::IndexMap<heapless::String, DspReceive>,

    // Global list of all `delwrite~` objects
    // TODO(tarcieri): `DspDelayWrite`
    // delay_line_list: List<DspDelayWrite>,

    // Global list of all `delread~` and `vd~` objects
    // TODO(tarcieri): `DelayReceiver`
    // delay_receiver_list: List<DelayReceiver>,

    // Global list of all `throw~` objects
    // TODO(tarcieri): `DspThrow`
    // throw_list: List<DspThrow>,

    // Global list of all `catch~` objects
    // TODO(tarcieri): `DspCatch`
    // catch_list: List<DspCatch>,

    // Global list of all `table` objects
    // TODO(tarcieri): `MessageTable`
    // table_list: List<MessageTable>,

    // Global list of all table receivers (e.g., `tabread4~` and `tabplay~`)
    // TODO(tarcieri): `TableReceiverInterface`
    // table_receiver_list: List<TableReceiverInterface>,

    // Object factory map
    // TODO(tarcieri): `ObjectFactoryMap` (or replace with a type registry?)
    // object_factory_map: ObjectFactoryMap,

    // Buffer pool
    // TODO(tarcieri): replace this with `Allocator`
    // buffer_pool: BufferPool

    // Abstraction database
    // TODO(tarcieri): `PdAbstractionDatabase`
    // abstraction_database: PdAbstractionDatabase

    // Callback function
    // TODO(tarcieri): replace this with a trait?
    // callback_function: Option<Fn(ZGCallbackFunction, c_void *, c_void *)>,

    // User-supplied data to pass to callback function
    // TODO(tarcieri): replace this with a trait?
    // callback_user_data: T

    // TODO(tarcieri): extract this into its own type?
    /// Global map for storing Value objects
    value_map: heapless::FnvIndexMap<heapless::String<U32>, f32, U32>,
}

impl<'pd> Context<'pd> {
    /// Create a new `pd::Context`. This is the first thing you'll need to do in
    /// order to use this crate.
    pub fn new(
        num_input_channels: usize,
        num_output_channels: usize,
        block_size: usize,
        sample_rate: f32,
    ) -> Self {
        let block_start_timestamp = Timestamp::default();

        let block_duration =
            Duration::from_millis(((block_size as f64) / (f64::from(sample_rate)) * 1000.0) as u64);

        let message_callback_queue = message::OrderedQueue::new();

        // TODO: object factory map
        // let object_factory_map = ObjectFactoryMap::new();

        // TODO: global graph ID
        // let global_graph_id = pd::graph::Id(0);

        let num_bytes_in_input_buffers = block_size * num_input_channels * size_of::<f32>();

        // TODO: allocate DSP input buffer
        // let global_dsp_input_buffers = (num_bytes_in_input_buffers > 0)? ALLOC_ALIGNED_BUFFER(num_bytes_in_input_buffers): NULL;
        // memset(global_dsp_input_buffers, 0, num_bytes_in_input_buffers);
        let global_dsp_input_buffers = &mut [];

        let num_bytes_in_output_buffers = block_size * num_output_channels * size_of::<f32>();

        // TODO: allocate DSP output buffer
        // let global_dsp_output_buffers = (num_bytes_in_output_buffers > 0)? ALLOC_ALIGNED_BUFFER(num_bytes_in_output_buffers): NULL;
        // memset(global_dsp_output_buffers, 0, num_bytes_in_output_buffers);
        let global_dsp_output_buffers = &mut [];

        let send_controller = message::send::Controller::new();

        let object_allocator = FixedSizeAllocator::new("object");

        let graph_allocator = FixedSizeAllocator::new("graph");
        let global_graph_id = pd::graph::Id::default();

        // TODO: abstraction database
        // let abstraction_database = new PdAbstractionDataBase();

        // TODO: multithread support
        // let context_lock = Mutex::new(...);

        let value_map = heapless::FnvIndexMap::new();

        Self {
            num_input_channels,
            num_output_channels,
            block_size,
            sample_rate,
            num_bytes_in_input_buffers,
            num_bytes_in_output_buffers,
            global_dsp_input_buffers,
            global_dsp_output_buffers,
            block_start_timestamp,
            block_duration,
            message_callback_queue,
            send_controller,
            object_allocator,
            graph_allocator,
            global_graph_id,
            value_map,
        }
    }

    // Register an object label and its associated factory method. */
    // TODO(tarcieri): `ObjectFactoryMap` (or replace with a type registry?)
    // pub fn register_external_object<F, N>(&mut self, object_label: &str, obj_factory: F)
    // where
    //    F: Fn(&pd::Message<'pd, N>, &pd::Graph) -> message::Object,
    //    N: ArrayLength<pd::message::Atom<'pd>>,
    // {
    //    self.object_factory_map.register_external_object(object_label, obj_factory);
    // }

    // Unregister an object label
    // pub fn unregister_external_object(&mut self, object_label: &str) {
    //     self.object_factory_map.unregister_external_object(object_label);
    // }

    /// Get the number of input channels
    pub fn get_num_input_channels(&self) -> usize {
        self.num_input_channels
    }

    /// Get the number of output channels
    pub fn get_num_output_channels(&self) -> usize {
        self.num_output_channels
    }

    /// Get the audio block size we're generating in bytes
    pub fn get_block_size(&self) -> usize {
        self.block_size
    }

    /// Get the current sample rate
    pub fn get_sample_rate(&self) -> f32 {
        self.sample_rate
    }

    /// Returns the global DSP buffer at the given inlet
    ///
    /// Exclusively used by `DspAdc`
    // TODO(tarcieri): extract a `dsp::Buffer` type?
    pub fn get_global_dsp_buffer_at_inlet(&self, inlet_index: usize) -> &[f32] {
        &self.global_dsp_input_buffers[(inlet_index * self.block_size)..]
    }

    /// Returns the global DSP buffer at the given outlet
    ///
    /// Exclusively used by `DspDac`
    // TODO(tarcieri): extract a `dsp::Buffer` type?
    pub fn get_global_dsp_buffer_at_outlet(&self, outlet_index: usize) -> &[f32] {
        &self.global_dsp_output_buffers[(outlet_index * self.block_size)..]
    }

    /// Returns the timestamp of the beginning of the current block
    pub fn get_block_start_timestamp(&self) -> Timestamp {
        self.block_start_timestamp
    }

    /// Returns the duration of one block
    pub fn get_block_duration(&self) -> Duration {
        self.block_duration
    }

    /// Get an object with a given object ID
    pub fn get_object_mut(&mut self, object_id: object::Id) -> Option<&mut message::Object> {
        self.object_allocator.get_mut(object_id)
    }

    /// Get the graph with a given graph ID
    pub fn get_graph_mut(&mut self, graph_id: graph::Id) -> Option<&mut Graph> {
        self.graph_allocator.get_mut(graph_id)
    }

    // TODO(tarcieri): `pd::Graph` support, and use an `Allocator` for this?
    // Returns the next globally unique graph id
    // pub fn get_next_graph_id(&mut self) -> pd::graph::Id {
    //    self.global_graph_id = self.global_graph_id.next();
    //    self.global_graph_id
    //}

    /// Run the Pure Data engine, processing the given input and output buffers
    pub fn process(&mut self, input_buffers: &[f32], output_buffers: &mut [f32]) {
        // TODO(tarcieri): multithread support
        // self.lock(); // lock the context

        // Set up `adc~` buffers
        self.global_dsp_input_buffers.copy_from_slice(input_buffers);

        // Clear the global output audio buffers so that `dac~` nodes can write to it
        for value in self.global_dsp_output_buffers.iter_mut() {
            *value = 0.0
        }

        // Send all messages for this block
        let next_block_start_timestamp = self.block_start_timestamp + self.block_duration;

        while !self.message_callback_queue.is_empty() {
            // Stop processing messages if their timestamps are outside the current block
            if self
                .message_callback_queue
                .peek()
                .unwrap()
                .second
                .first
                .get_timestamp()
                >= next_block_start_timestamp
            {
                break;
            }

            // remove the message from the queue
            let mut message_entry = self.message_callback_queue.pop().unwrap();

            let _object_id = message_entry.first;
            let _outlet_index = message_entry.second.second;
            let message = &mut message_entry.second.first;

            if message.get_timestamp() < self.block_start_timestamp {
                // messages injected into the system with a timestamp behind
                // the current block are automatically rescheduled for the
                // beginning of the current block.
                //
                // This is done in order to normalize the treament of messages,
                // but also to avoid difficulties in cases when messages are
                // scheduled in subgraphs with different block sizes.
                message.set_timestamp(self.block_start_timestamp);
            }

            // TODO(tarcieri): figure out lifetime solution for double borrow here
            // self.get_object(object_id)
            //    .unwrap()
            //    .send_message(self, outlet_index, message);

            // TODO(tarcieri): figure out buffer management strategy for `pd::Message` values
            // free the message now that it has been sent and processed
            // message.free_message();

            // TODO(tarcieri): figure out lifetime solution for double borrow here
            //for graph in self.graph_allocator.iter_mut() {
            //    graph.process_function(graph, 0, 0);
            //}
        }

        self.block_start_timestamp = next_block_start_timestamp;

        // copy the output audio to the given buffer
        output_buffers.copy_from_slice(self.global_dsp_output_buffers);

        // TODO(tarcieri): multithread support
        // self.unlock(); // unlock the context
    }

    // TODO(tarcieri): pd::Graph
    //// Attach the given `graph` to this `context`, also registering all
    //// necessary objects, and recomputing the DSP object order if necessary.
    //// pub fn attach_graph(&mut self, graph: &pd::Graph) {
    //    // TODO(tarcieri): multithread support
    //    // self.lock();
    //
    //    self.graph_allocator.push_back(graph);
    //    graph.attach_to_context(true);
    //    graph.compute_deep_local_process_order();
    //
    //    // TODO(tarcieri): multithread support
    //    // self.unlock();
    // }

    // TODO(tarcieri): pd::Graph
    //// Detach the given `graph` from this `context`, also unregistering objects
    //// pub fn unattach_graph(&mut self, graph: &pd::Graph) {
    //    // TODO(tarcieri): multithread support
    //    // self.lock();
    //
    //    graph_allocator.remove(graph);
    //    graph.attach_to_context(false);
    //
    //    // TODO(tarcieri): multithread support
    //    // self.unlock();
    // }

    // TODO(tarcieri): pd::Graph
    //// Create a new object in a graph
    // pub fn new_object<N>(
    //     &mut self,
    //     object_label: &str,
    //     init_message: &pd::Message<'pd, N>,
    //     graph: &pd::Graph,
    // ) -> &mut message::Object
    // where
    //     N: ArrayLength<pd::message::Atom<'pd>>,
    // {
    //     if let Some(message_obj) =
    //         self.object_factory_map
    //             .new_object(object_label, init_message, graph)
    //     {
    //         message_obj
    //     } else if let Some(float_value) = object_label.parse::<f32>() {
    //         // special case for constructing a float object from a number
    //         self.object_factory_map.new_object(
    //             "float",
    //             pd::Message::from_timestamp_and_float(Timestamp::default(), float_value),
    //             graph,
    //         )
    //     } else {
    //         panic!("couldn't construct object: {}", object_label);
    //     }
    // }

    /// Prints the given message to error output
    // TODO: figure out `no_std` solution for logging messages
    #[cfg(feature = "std")]
    pub fn print_err(&mut self, msg: &str) {
        eprintln!("{}", msg);
    }

    /// Prints the given message to standard output
    // TODO: figure out `no_std` solution for logging messages
    #[cfg(feature = "std")]
    pub fn print_std(&mut self, msg: &str) {
        println!("{}", msg);
    }

    // TODO(tarcieri): DspReceive
    // /// Globally register a `receive~` object.
    // ///
    // /// Connects to registered `send~` objects with the same name
    // pub fn register_dsp_receive(&mut self, dsp_receive: &DspReceive) {
    //     // NOTE(mhroth): no duplicate check is made for dsp_receive
    //     self.dsp_receive_map[dsp_receive.get_name()].push_back(dsp_receive);
    //
    //     // connect `receive~` to associated `send~`
    //     if let Some(dsp_send) = self.get_dsp_send(dsp_receive.get_name()) {
    //         dsp_receive.set_dsp_buffer_at_inlet(dsp_send.get_dsp_buffer_at_inlet(0), 0);
    //     }
    // }

    // TODO(tarcieri): DspReceive
    // /// Globally unregister a `receive~` object.
    // pub fn unregister_dsp_receive(&mut self, dsp_receive: &DspReceive) {
    //     let mut receive_list = self.dsp_receive_map[dsp_receive.get_name()];
    //     receive_list.remove(dsp_receive);
    //     dsp_receive.set_dsp_buffer_at_inlet(
    //         dsp_receive.get_graph().get_buffer_pool().get_zero_buffer(),
    //         0,
    //     );
    // }

    // TODO(tarcieri): DspSend
    // /// Globally register a `send~` object.
    // ///
    // /// Connect to registered `receive~` objects with the same name.
    // pub fn register_dsp_send(&mut self, dsp_send: &DspSend) {
    //     if let Some(send_object) = self.get_dsp_send(dsp_send.get_name()) {
    //         self.dsp_send_list.push_back(dsp_send);
    //
    //         // connect associated `receive~` objects to `send~`
    //         self.update_dsp_receive_for_send_with_buffer(
    //             dsp_send.get_name(),
    //             dsp_send.get_dsp_buffer_at_outlet(0),
    //         );
    //     } else {
    //         panic!(
    //             "duplicate send~ object found with name '{}'",
    //             dsp_send.get_name()
    //         );
    //     }
    // }

    // TODO(tarcieri): DspSend
    // /// Globally unregister a `send~` object.
    // pub fn unregister_dsp_send(&mut self, dsp_send: &DspSend) {
    //     self.dsp_send_list.remove(dsp_send);
    //
    //     // inform all previously connected receive~ objects that the send~
    //     // buffer does not exist anymore.
    //     self.update_dsp_receive_for_send_with_buffer(
    //         dsp_send.get_name(),
    //         dsp_send.get_graph().get_buffer_pool().get_zero_buffer(),
    //     );
    // }
    //
    // /// Get the named global `DspSend` object.
    // pub fn get_dsp_send(&mut self, name: &str) -> Option<&mut DspSend> {
    //     self.dsp_send_list
    //         .find(|dsp_send| dsp_send.get_name() == name)
    // }

    // TODO(tarcieri): DspReceive and dsp_receive_map
    // /// Updates the input buffers all attached objects to the `DspReceive`s
    // /// associated with the given `DspSend`.
    // pub fn update_dsp_receive_for_send_with_buffer(&mut self, name: &str, buffer: &[f32]) {
    //     for mut dsp_receive in self.dsp_receive_map[name] {
    //         dsp_receive.set_dsp_buffer_at_inlet(buffer, 0);
    //     }
    // }

    // TODO(tarcieri): RemoteMessageReceiver
    // /// Globally register a remote message receiver (e.g. `send` or `notein`)
    // pub fn register_remote_message_receiver(&mut self, receiver: &RemoteMessageReceiver) {
    //     self.send_controller.add_receiver(receiver);
    // }

    // TODO(tarcieri): RemoteMessageReceiver
    // /// Globally unregister a remote message receiver (e.g. `send` or `notein`)
    // pub fn unregister_remote_message_receiver(&mut self, receiver: &RemoteMessageReceiver) {
    //     self.send_controller.remove_receiver(receiver);
    // }

    // TODO(tarcieri): DspDelayWrite
    // /// Globally register a `delwrite~` object.
    // ///
    // /// Registration is necessary such that they can be connected to `delread~`
    // /// and `vd~` objects as are they are added to the graph.
    // pub fn register_delay_line(&mut self, delay_line: &DspDelayWrite) {
    //     if self.get_delay_line(delay_line.get_name()).is_some() {
    //         panic!(
    //             "delwrite~ with duplicate name '{}' registered.",
    //             delay_line.get_name()
    //         );
    //     }
    //
    //     self.delay_line_list.push_back(delay_line);
    //
    //     // connect this delay line to all same-named delay receivers
    //     for mut delay_receiver in self.delay_receiver_list {
    //         if delay_receiver.get_name() == delay_line.get_name() {
    //             delay_receiver.set_delay_line(delayline);
    //         }
    //     }
    // }

    // TODO(tarcieri): DelayReceiver
    // /// Globally register a `delread~` or `vd~` object.
    // ///
    // /// Registration is necessary such that they can be connected to
    // /// `delwrite~` objects as are they are added to the graph.
    // pub fn register_delay_receiver(&mut self, delay_receiver: &DelayReceiver) {
    //     self.delay_receiver_list.push_back(delay_receiver);
    //
    //     // connect the delay receiver to the named delayline
    //     self.get_delay_line(delay_receiver.get_name())
    //         .set_delay_line(delayline);
    // }

    // TODO(tarcieri): DspDelayWrite
    // /// Returns the named global `DspDelayWrite` object.
    // pub fn get_delay_line(&mut self, name: &str) -> Option<&mut DspDelayWrite> {
    //     self.delay_line_list
    //         .find(|delay_line| delay_line.get_name() == name)
    // }

    // TODO(tarcieri): DspThrow
    // /// Globally register a `DspThrow` object
    // pub fn register_dsp_throw(&mut self, dsp_throw: &DspThrow) {
    //     // NOTE(mhroth): no duplicate testing for the same object more than once
    //     self.throw_list.push_back(dsp_throw);
    //
    //     if let Some(dsp_catch) = self.get_dsp_catch(dsp_throw.get_name()) {
    //         dsp_catch.add_throw(dsp_throw);
    //     }
    // }

    // TODO(tarcieri): DspCatch
    // /// Globally register a `DspCatch` object
    // pub fn register_dsp_catch(&mut self, dsp_catch: &mut DspCatch) {
    //     if let Some(catch_object) = self.get_dsp_catch(dsp_catch.get_name()) {
    //         panic!(
    //             "catch~ with duplicate name '{}' already exists",
    //             dsp_catch.get_name()
    //         );
    //     }
    //
    //     self.catch_list.push_back(dsp_catch);
    //
    //     // connect `catch~` to all associated `throw~` objects
    //     for mut throw_object in self.throw_list() {
    //         if throw_object.get_name() == dsp_catch.get_name() {
    //             dsp_catch.add_throw()
    //         }
    //     }
    // }

    // TODO(tarcieri): DspCatch
    // /// Returns the named global `DspCatch` object
    // pub fn get_dsp_catch(&mut self, name: &str) -> Option<&mut DspCatch> {
    //     self.catch_list
    //         .find(|catch_object| catch_object.get_name() == name)
    // }

    // TODO(tarcieri): MessageTable
    // /// Register a `MessageTable`
    // pub fn register_table(&mut self, table: &MessageTable) {
    //     if self.get_table(table.get_name()).is_some() {
    //         panic!("table with name '{}' already exists.", table.get_name());
    //     }
    //
    //     self.table_list.push_back(table);
    //
    //     for mut table_receiver in self.table_receiver_list {
    //         // in case the table receiver doesn't have the table name yet
    //         if table_receiver.get_name() == table.get_name() {
    //             table_receiver.set_table(table);
    //         }
    //     }
    // }

    // TODO(tarcieri): MessageTable
    // /// Get a `MessageTable`
    // pub fn get_table(&mut self, name: &str) -> Option<&mut MessageTable> {
    //     self.table_list.find(|table| table.get_name() == name)
    // }

    // TODO(tarcieri): TableReceiverInterface
    // /// Register a `TableReceiverInterface`
    // pub fn register_table_receiver(&mut self, table_receiver: &mut dyn TableReceiverInterface) {
    //     // add the new receiver
    //     self.table_receiver_list.push_back(table_receiver);
    //
    //     // in case the tableread doesnt have the name of the table yet
    //     if let Some(name) = table_receiver.get_name() {
    //         // set table whether it is Some or not
    //         table_receiver.set_table(self.get_table(name));
    //     }
    // }

    // TODO(tarcieri): TableReceiverInterface
    // /// Unregister a `TableReceiverInterface`
    // pub fn unregister_table_receiver(&mut self, table_receiver: &mut dyn TableReceiverInterface) {
    //     // remove the receiver
    //     self.table_receiver_list.remove(table_receiver);
    //     table_receiver.set_table(None);
    // }

    /// Set global floating point constant value.
    ///
    /// Used with `MessageValue` for keeping track of global variables.
    pub fn set_value_for_name(&mut self, name: &str, constant: f32) {
        self.value_map[&heapless::String::from_str(name).unwrap()] = constant;
    }

    /// Get global floating point constant value.
    ///
    /// Used with `MessageValue` for keeping track of global variables.
    pub fn get_value_for_name(&self, name: &str) -> Option<f32> {
        // TODO(tarcieri): this data structure could probably be better implemented
        self.value_map
            .get(&heapless::String::from_str(name).unwrap())
            .cloned()
    }

    /// Register an external receiver.
    pub fn register_external_receiver(&mut self, receiver_name: &str) {
        // TODO(tarcieri): multithread support
        // don't update the external receiver registry while processing it, of course!
        // self.lock();

        self.send_controller
            .register_external_receiver(receiver_name);

        // TODO(tarcieri): multithread support
        // self.unlock();
    }

    /// Unregister an external receiver
    pub fn unregister_external_receiver(&mut self, receiver_name: &str) {
        // TODO(tarcieri): multithread support
        // don't update the external receiver registry while processing it, of course!
        // self.lock();

        self.send_controller
            .unregister_external_receiver(receiver_name);

        // TODO(tarcieri): multithread support
        // self.unlock();
    }

    /// Sends the given message to all `receive` objects with the given `name`.
    ///
    /// This function is used by message boxes to send messages described be the syntax:
    ///
    /// `name message;`
    ///
    pub fn send_message_to_named_receivers<N>(
        &mut self,
        _name: &str,
        _message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        // TODO(tarcieri): figure out lifetime solution for double borrow here
        // self.send_controller.receive_message(self, name, message);
        panic!("unimplemented!")
    }

    // TODO(tarcieri): write a macro to replace this
    // /// Schedules a message to be sent to all receivers at the start of the
    // /// next block.
    // ///
    // /// Returns the `pd::Message` which will be sent.
    // ///
    // /// It is intended that the programmer will set the values of the message
    // /// with a call to `set_message()`.
    // // TODO(tarcieri): write a macro to handle the variable arguments
    // pub fn schedule_external_message_v(
    //     &mut self,
    //     receiver_name: &str,
    //     timestamp: Timestamp,
    //     message_format: &str,
    //     args: &[&str],
    // ) {
    //     let mut message = pd::Message::from_timestamp(timestamp);
    //
    //     // format message
    //     for (i, ch) in message_format.chars().enumerate() {
    //         match ch {
    //             'f' => message.set_float(i, args[i].parse::<f32>()),
    //             's' => message.set_symbol(i, args[i]),
    //             'b' => message.set_bang(i),
    //             _ => panic!("invalid format character: {}", ch),
    //         }
    //     }
    //
    //     self.schedule_external_message(receiver_name, message);
    // }

    /// Schedules a message to be sent to all receivers at the start of the
    /// next block.
    pub fn schedule_external_message<N>(
        &mut self,
        receiver_name: &str,
        _message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        // TODO(tarcieri): multithread support
        // self.lock();

        // if the receiver exists
        if let Some(_receiver_name_index) = self.send_controller.get_name_index(receiver_name) {
            // TODO(tarcieri): figure out trait or way to make `send_controller` a `message::Object`
            // self.schedule_message(self.send_controller, receiver_name_index, message);
        }

        // TODO(tarcieri): multithread support
        // self.unlock();
    }

    /// Schedules a message described by the given string to be sent to named receivers at the
    /// given timestamp.
    pub fn schedule_external_message_string(
        &mut self,
        receiver_name: &str,
        timestamp: Timestamp,
        init_string: &str,
    ) -> Result<(), Error> {
        // do the heavy lifting of string parsing before the lock (minimise the critical section)
        // let max_elements = (init_string.len() / 2) + 1;
        // TODO(tarcieri): calculate the maximum number of elements somehow
        let _message: pd::Message<U32> = pd::Message::from_string(timestamp, init_string)?;

        // TODO(tarcieri): multithread support
        // self.lock(); // lock and load

        // if the receiver exists
        if let Some(_receiver_name_index) = self.send_controller.get_name_index(receiver_name) {
            // TODO(tarcieri): figure out trait or way to make `send_controller` a `message::Object`
            // self.schedule_message(self.send_controller, receiver_name_index, &message);
        }

        // TODO(tarcieri): multithread support
        // self.unlock();

        Ok(())
    }

    /// Schedules a `pd::Message` to be sent by the `message::Object` from the
    /// `outlet_index` at the specified `time`.
    pub fn schedule_message<N>(
        &mut self,
        _message_obj: message::Object,
        _outlet_index: connection::Index,
        _message: pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        panic!("unimplemented");
        // TODO(tarcieri): message_callback_queue support
        //self.message_callback_queue
        //    .insert_message(message_obj.id(), outlet_index, message);
    }

    /// Cancel a scheduled `pd::Message` according to its id.
    ///
    /// The message memory will be freed.
    pub fn cancel_message<N>(
        &mut self,
        _message_obj: &message::Object,
        _outlet_index: connection::Index,
        _message: &pd::Message<'pd, N>,
    ) where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        panic!("unimplemented");
        // TODO(tarcieri): message_callback_queue support
        //self.message_callback_queue
        //    .remove_message(message_obj.id(), outlet_index, message);

        // TODO(tarcieri): support for freeing messages?
        // message.free();
    }

    // TODO(tarcieri): figure out replacement for callback API
    /// Receives and processes messages sent to the Pd system by sending to `pd`
    pub fn receive_system_message<N>(&mut self, _message: &pd::Message<'pd, N>)
    where
        N: ArrayLength<pd::message::Atom<'pd>>,
    {
        panic!("unimplemented!");

        // // TODO(mhroth): What are all of the possible system messages?
        // if message.is_symbol_str(0, "obj") {
        //     // TODO(mhroth): dynamic patching
        // } else if let Some(callback_function) = self.callback_function {
        //     if message.is_symbol_str(0, "dsp") && message.is_float(1) {
        //         let result = if message.get_float(1) != Some(0.0) {
        //             1
        //         } else {
        //             0
        //         };
        //
        //         callback_function(ZG_PD_DSP, callback_user_data, &result);
        //     }
        // } else {
        //     panic!("unrecognised system command: {:?}", message);
        // }
    }

    // TODO(tarcieri): PdAbstractionDataBase - what is it? Do we need it?
    // /// Get the abstraction database
    // pub fn get_abstraction_database(&self) -> &PdAbstractionDataBase {
    //     &self.abstraction_database
    // }
}
