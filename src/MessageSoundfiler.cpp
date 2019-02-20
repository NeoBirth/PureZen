/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#include "MessageSoundfiler.h"
#include "MessageTable.h"
#include "PdGraph.h"

#include <sndfile.h>

message::Object *MessageSoundfiler::new_object(pd::Message *init_message, PdGraph *graph) {
  return new MessageSoundfiler(init_message, graph);
}

MessageSoundfiler::MessageSoundfiler(pd::Message *init_message, PdGraph *graph) : message::Object(1, 1, graph) {
  // TODO(mhroth)
}

MessageSoundfiler::~MessageSoundfiler() {
  // nothing to do
}

void MessageSoundfiler::process_message(int inlet_index, pd::Message *message)
{
  if (message->is_symbol_str(0, "read"))
  {
    int currentElementIndex;
    bool shouldResizeTable = false;
    char *tabName;
    char *soundfilePath;
    MessageTable *table;

    for (currentElementIndex = 1;
         currentElementIndex < message->get_num_elements();
         ++currentElementIndex)
    {
      if (message->is_symbol_str(currentElementIndex, "-resize"))
        shouldResizeTable = true;
      /* else if other flags... */
      else
        break;
    }
    if (message->get_num_elements() - currentElementIndex - 2 < 0)
    {
      graph->print_err("[soundfiler]: parameters are incorrect");
      return;
    }
    soundfilePath = message->get_symbol(currentElementIndex++);
    tabName = message->get_symbol(currentElementIndex++);
    if ((table = graph->get_table(tabName)) == NULL)
    {
      graph->print_err("[soundfiler]: table '%s' cannot be found", tabName);
      return;
    }
    SF_INFO sfInfo;
    char *fullPath = graph->resolveFullPath(soundfilePath);
    if (fullPath == NULL)
    {
      graph->print_err("[soundfiler]: file '%s' cannot be found.", soundfilePath);
      return;
    }

    SNDFILE *sndFile = sf_open(fullPath, SFM_READ, &sfInfo);
    if (sndFile == NULL)
    {
      graph->print_err("[soundfiler]: file %s cannot be opened.", fullPath);
      free(fullPath);
      return; // there was an error reading the file. Move on with life.
    }
    delete fullPath;

    // It is assumed that the channels are interleaved.
    int samplesPerChannel = static_cast<int>(sfInfo.frames);
    int bufferLength = samplesPerChannel * sfInfo.channels;
    // create a buffer in memory for the file data
    float *buffer = (float *) malloc(bufferLength * sizeof(float));
    sf_read_float(sndFile, buffer, bufferLength); // read the whole file into memory
    sf_close(sndFile); // release the handle to the file

    if (sfInfo.channels > 0) // sanity check
    {
      // get the table's buffer. Resize the buffer if necessary.
      int tableLength = samplesPerChannel;
      float *tableBuffer = shouldResizeTable ? table->resizeBuffer(samplesPerChannel) :
      table->getBuffer(&tableLength);
      if (tableLength > samplesPerChannel)
      {
        // avoid trying to read more into the table buffer than is available
        tableLength = samplesPerChannel;
      }

      // extract the first channel
      for (int i = 0, j = 0; i < bufferLength; i+=sfInfo.channels, j++)
      {
        tableBuffer[j] = buffer[i];
      }

      // extract the second channel (if it exists and if there is a table to write it to)
      if (sfInfo.channels > 1 &&
          (tabName = message->get_symbol(currentElementIndex++)) != NULL &&
          (table = graph->get_table(tabName)) != NULL)
      {
        tableLength = samplesPerChannel;
        tableBuffer = shouldResizeTable ? table->resizeBuffer(samplesPerChannel) :
        table->getBuffer(&tableLength);
        if (tableLength > samplesPerChannel)
        {
          // avoid trying to read more into the table buffer than is available
          tableLength = samplesPerChannel;
        }
        for (int i = 1, j = 0; i < bufferLength; i+=sfInfo.channels, j++)
          tableBuffer[j] = buffer[i];
      }
    }
    delete buffer;

    // send message with sample length when all tables have been filled
    pd::Message *outgoing_message = PD_MESSAGE_ON_STACK(1);
    outgoing_message->from_timestamp_and_float(message->get_timestamp(), samplesPerChannel);
    send_message(0, outgoing_message);
  }
  else if (message->is_symbol_str(0, "write"))
  {
    //Not implemented yet
    graph->print_err("[soundfiler]: The 'write' command is not supported yet.");
  }
}

