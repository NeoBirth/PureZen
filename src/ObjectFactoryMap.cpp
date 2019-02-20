/*
 *  Copyright 2011 Reality Jockey, Ltd.
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

#include "ObjectFactoryMap.h"

// all standard message objects
#include "MessageAbsoluteValue.h"
#include "MessageAdd.h"
#include "MessageArcTangent.h"
#include "MessageArcTangent2.h"
#include "MessageBang.h"
#include "MessageCosine.h"
#include "MessageCputime.h"
#include "MessageChange.h"
#include "MessageClip.h"
#include "MessageCputime.h"
#include "MessageDeclare.h"
#include "MessageDelay.h"
#include "MessageDiv.h"
#include "MessageDivide.h"
#include "MessageDbToPow.h"
#include "MessageDbToRms.h"
#include "MessageEqualsEquals.h"
#include "MessageExp.h"
#include "MessageFloat.h"
#include "MessageFrequencyToMidi.h"
#include "MessageGreaterThan.h"
#include "MessageGreaterThanOrEqualTo.h"
#include "MessageInlet.h"
#include "MessageInteger.h"
#include "MessageLessThan.h"
#include "MessageLessThanOrEqualTo.h"
#include "MessageLine.h"
#include "MessageListAppend.h"
#include "MessageListLength.h"
#include "MessageListPrepend.h"
#include "MessageListSplit.h"
#include "MessageListTrim.h"
#include "MessageLoadbang.h"
#include "MessageLog.h"
#include "MessageLogicalAnd.h"
#include "MessageLogicalOr.h"
#include "MessageMakefilename.h"
#include "MessageMaximum.h"
#include "MessageMessageBox.h"
#include "MessageMetro.h"
#include "MessageMidiToFrequency.h"
#include "MessageMinimum.h"
#include "MessageModulus.h"
#include "MessageMoses.h"
#include "MessageMultiply.h"
#include "MessageNotEquals.h"
#include "MessageNotein.h"
#include "MessageOpenPanel.h"
#include "MessageOutlet.h"
#include "MessagePack.h"
#include "MessagePipe.h"
#include "MessagePow.h"
#include "MessagePowToDb.h"
#include "MessagePrint.h"
#include "MessageRandom.h"
#include "MessageReceive.h"
#include "MessageRemainder.h"
#include "MessageRmsToDb.h"
#include "MessageRoute.h"
#include "MessageSamplerate.h"
#include "MessageSelect.h"
#include "message::Send.h"
#include "MessageSine.h"
#include "MessageSoundfiler.h"
#include "MessageSpigot.h"
#include "MessageSqrt.h"
#include "MessageStripNote.h"
#include "MessageSubtract.h"
#include "MessageSwitch.h"
#include "MessageSwap.h"
#include "MessageSymbol.h"
#include "MessageTable.h"
#include "MessageTableRead.h"
#include "MessageTableWrite.h"
#include "MessageTangent.h"
#include "MessageText.h"
#include "MessageTimer.h"
#include "MessageToggle.h"
#include "MessageTrigger.h"
#include "MessageUntil.h"
#include "MessageUnpack.h"
#include "MessageValue.h"
#include "MessageWrap.h"

// all standard dsp objects
#include "DspAdc.h"
#include "DspAdd.h"
#include "DspBandpassFilter.h"
#include "DspBang.h"
#include "DspCatch.h"
#include "DspClip.h"
#include "DspCosine.h"
#include "DspDac.h"
#include "DspDelayRead.h"
#include "DspDelayWrite.h"
#include "DspDivide.h"
#include "DspEnvelope.h"
#include "DspHighpassFilter.h"
#include "DspInlet.h"
#include "DspLine.h"
#include "DspLog.h"
#include "DspLowpassFilter.h"
#include "DspMinimum.h"
#include "DspMultiply.h"
#include "DspNoise.h"
#include "DspOsc.h"
#include "DspOutlet.h"
#include "DspPhasor.h"
#include "DspPrint.h"
#include "DspReceive.h"
#include "DspReciprocalSqrt.h"
#include "DspRfft.h"
#include "DspRifft.h"
#include "DspSend.h"
#include "DspSignal.h"
#include "DspSqrt.h"
#include "DspSnapshot.h"
#include "DspSubtract.h"
#include "DspTablePlay.h"
#include "DspTableRead.h"
#include "DspTableRead4.h"
#include "DspThrow.h"
#include "DspVariableDelay.h"
#include "DspVariableLine.h"
#include "DspVCF.h"
#include "DspWrap.h"

ObjectFactoryMap::ObjectFactoryMap() {
  // these objects represent the core set of supported objects
  
  // message objects
  object_factory_map[string(MessageAbsoluteValue::get_object_label())] = &MessageAbsoluteValue::new_object;
  object_factory_map[string(MessageAdd::get_object_label())] = &MessageAdd::new_object;
  object_factory_map[string(MessageArcTangent::get_object_label())] = &MessageArcTangent::new_object;
  object_factory_map[string(MessageArcTangent2::get_object_label())] = &MessageArcTangent2::new_object;
  object_factory_map[string(MessageBang::get_object_label())] = &MessageBang::new_object;
  object_factory_map[string("bng")] = &MessageBang::new_object;
  object_factory_map[string("b")] = &MessageBang::new_object;
  object_factory_map[string(MessageChange::get_object_label())] = &MessageChange::new_object;
  object_factory_map[string(MessageClip::get_object_label())] = &MessageClip::new_object;
  object_factory_map[string(MessageCosine::get_object_label())] = &MessageCosine::new_object;
  object_factory_map[string(MessageCputime::get_object_label())] = &MessageCputime::new_object;
  object_factory_map[string(MessageDbToPow::get_object_label())] = &MessageDbToPow::new_object;
  object_factory_map[string(MessageDbToRms::get_object_label())] = &MessageDbToRms::new_object;
  object_factory_map[string(MessageDeclare::get_object_label())] = &MessageDeclare::new_object;
  object_factory_map[string(MessageDelay::get_object_label())] = &MessageDelay::new_object;
  object_factory_map[string("del")] = &MessageDelay::new_object;
  object_factory_map[string(MessageDiv::get_object_label())] = &MessageDiv::new_object;
  object_factory_map[string(MessageDivide::get_object_label())] = &MessageDivide::new_object;
  object_factory_map[string(MessageEqualsEquals::get_object_label())] = &MessageEqualsEquals::new_object;
  object_factory_map[string(MessageExp::get_object_label())] = &MessageExp::new_object;
  object_factory_map[string(MessageFloat::get_object_label())] = &MessageFloat::new_object;
  object_factory_map[string("f")] = &MessageFloat::new_object;
  object_factory_map[string("nbx")] = &MessageFloat::new_object; // number boxes are represented as float objects
  object_factory_map[string("hsl")] = &MessageFloat::new_object; // horizontal and vertical sliders are
  object_factory_map[string("vsl")] = &MessageFloat::new_object; // represened as float boxes
  object_factory_map[string(MessageFrequencyToMidi::get_object_label())] = &MessageFrequencyToMidi::new_object;
  object_factory_map[string(MessageGreaterThan::get_object_label())] = &MessageGreaterThan::new_object;
  object_factory_map[string(MessageGreaterThanOrEqualTo::get_object_label())] = &MessageGreaterThanOrEqualTo::new_object;
  object_factory_map[string(MessageInlet::get_object_label())] = &MessageInlet::new_object;
  object_factory_map[string(MessageInteger::get_object_label())] = &MessageInteger::new_object;
  object_factory_map[string("i")] = &MessageInteger::new_object;
  object_factory_map[string(MessageLessThan::get_object_label())] = &MessageLessThan::new_object;
  object_factory_map[string(MessageLessThanOrEqualTo::get_object_label())] = &MessageLessThanOrEqualTo::new_object;
  object_factory_map[string(MessageLine::get_object_label())] = &MessageLine::new_object;
  object_factory_map[string("list")] = &MessageListAppend::new_object; // MessageListAppend factory creates any kind of list object
  object_factory_map[string(MessageLoadbang::get_object_label())] = &MessageLoadbang::new_object;
  object_factory_map[string(MessageLog::get_object_label())] = &MessageLog::new_object;
  object_factory_map[string(MessageLogicalAnd::get_object_label())] = &MessageLogicalAnd::new_object;
  object_factory_map[string(MessageLogicalOr::get_object_label())] = &MessageLogicalOr::new_object;
  object_factory_map[string(MessageMakefilename::get_object_label())] = &MessageMakefilename::new_object;
  object_factory_map[string(MessageMaximum::get_object_label())] = &MessageMaximum::new_object;
  object_factory_map[string(MessageMessageBox::get_object_label())] = &MessageMessageBox::new_object;
  object_factory_map[string(MessageMetro::get_object_label())] = &MessageMetro::new_object;
  object_factory_map[string(MessageMidiToFrequency::get_object_label())] = &MessageMidiToFrequency::new_object;
  object_factory_map[string(MessageMinimum::get_object_label())] = &MessageMinimum::new_object;
  object_factory_map[string(MessageModulus::get_object_label())] = &MessageModulus::new_object;
  object_factory_map[string(MessageMoses::get_object_label())] = &MessageMoses::new_object;
  object_factory_map[string(MessageMultiply::get_object_label())] = &MessageMultiply::new_object;
  object_factory_map[string(MessageNotein::get_object_label())] = &MessageNotein::new_object;
  object_factory_map[string(MessageNotEquals::get_object_label())] = &MessageNotEquals::new_object;
  object_factory_map[string(MessageOpenPanel::get_object_label())] = &MessageOpenPanel::new_object;
  object_factory_map[string(MessageOutlet::get_object_label())] = &MessageOutlet::new_object;
  object_factory_map[string(MessagePack::get_object_label())] = &MessagePack::new_object;
  object_factory_map[string(MessagePipe::get_object_label())] = &MessagePipe::new_object;
  object_factory_map[string(MessagePow::get_object_label())] = &MessagePow::new_object;
  object_factory_map[string(MessagePowToDb::get_object_label())] = &MessagePowToDb::new_object;
  object_factory_map[string(MessagePrint::get_object_label())] = &MessagePrint::new_object;
  object_factory_map[string(MessageRandom::get_object_label())] = &MessageRandom::new_object;
  object_factory_map[string(MessageReceive::get_object_label())] = &MessageReceive::new_object;
  object_factory_map[string("r")] = &MessageReceive::new_object;
  object_factory_map[string(MessageRemainder::get_object_label())] = &MessageRemainder::new_object;
  object_factory_map[string(MessageRmsToDb::get_object_label())] = &MessageRmsToDb::new_object;
  object_factory_map[string(MessageRoute::get_object_label())] = &MessageRoute::new_object;
  object_factory_map[string(MessageSamplerate::get_object_label())] = &MessageSamplerate::new_object;
  object_factory_map[string(MessageSelect::get_object_label())] = &MessageSelect::new_object;
  object_factory_map[string("sel")] = &MessageSelect::new_object;
  object_factory_map[string(message::Send::get_object_label())] = &message::Send::new_object;
  object_factory_map[string("s")] = &message::Send::new_object;
  object_factory_map[string(MessageSine::get_object_label())] = &MessageSine::new_object;
  object_factory_map[string(MessageSoundfiler::get_object_label())] = &MessageSoundfiler::new_object;
  object_factory_map[string(MessageSpigot::get_object_label())] = &MessageSpigot::new_object;
  object_factory_map[string(MessageSqrt::get_object_label())] = &MessageSqrt::new_object;
  object_factory_map[string(MessageStripNote::get_object_label())] = &MessageStripNote::new_object;
  object_factory_map[string(MessageSubtract::get_object_label())] = &MessageSubtract::new_object;
  object_factory_map[string(MessageSwap::get_object_label())] = &MessageSwap::new_object;
  object_factory_map[string(MessageSwitch::get_object_label())] = &MessageSwitch::new_object;
  object_factory_map[string(MessageSymbol::get_object_label())] = &MessageSymbol::new_object;
  object_factory_map[string(MessageTable::get_object_label())] = &MessageTable::new_object;
  object_factory_map[string(MessageTableRead::get_object_label())] = &MessageTableRead::new_object;
  object_factory_map[string(MessageTableWrite::get_object_label())] = &MessageTableWrite::new_object;
  object_factory_map[string(MessageTangent::get_object_label())] = &MessageTangent::new_object;
  object_factory_map[string(MessageText::get_object_label())] = &MessageText::new_object;
  object_factory_map[string(MessageTimer::get_object_label())] = &MessageTimer::new_object;
  object_factory_map[string(MessageToggle::get_object_label())] = &MessageToggle::new_object;
  object_factory_map[string("tgl")] = &MessageToggle::new_object;
  object_factory_map[string(MessageTrigger::get_object_label())] = &MessageTrigger::new_object;
  object_factory_map[string("t")] = &MessageTrigger::new_object;
  object_factory_map[string(MessageUnpack::get_object_label())] = &MessageUnpack::new_object;
  object_factory_map[string(MessageUntil::get_object_label())] = &MessageUntil::new_object;
  object_factory_map[string(MessageValue::get_object_label())] = &MessageValue::new_object;
  object_factory_map[string("v")] = &MessageValue::new_object;
  object_factory_map[string(MessageWrap::get_object_label())] = &MessageWrap::new_object;
  
  // dsp objects
  object_factory_map[string(DspAdc::get_object_label())] = &DspAdc::new_object;
  object_factory_map[string(DspAdd::get_object_label())] = &DspAdd::new_object;
  object_factory_map[string(DspBandpassFilter::get_object_label())] = &DspBandpassFilter::new_object;
  object_factory_map[string(DspBang::get_object_label())] = &DspBang::new_object;
  object_factory_map[string(DspCatch::get_object_label())] = &DspCatch::new_object;
  object_factory_map[string(DspClip::get_object_label())] = &DspClip::new_object;
  object_factory_map[string(DspCosine::get_object_label())] = &DspCosine::new_object;
  object_factory_map[string(DspDac::get_object_label())] = &DspDac::new_object;
  object_factory_map[string(DspDelayRead::get_object_label())] = &DspDelayRead::new_object;
  object_factory_map[string(DspDelayWrite::get_object_label())] = &DspDelayWrite::new_object;
  object_factory_map[string(DspDivide::get_object_label())] = &DspDivide::new_object;
  object_factory_map[string(DspEnvelope::get_object_label())] = &DspEnvelope::new_object;
  object_factory_map[string(DspHighpassFilter::get_object_label())] = &DspHighpassFilter::new_object;
  object_factory_map[string(DspInlet::get_object_label())] = &DspInlet::new_object;
  object_factory_map[string(DspLine::get_object_label())] = &DspLine::new_object;
  object_factory_map[string(DspLog::get_object_label())] = &DspLog::new_object;
  object_factory_map[string(DspLowpassFilter::get_object_label())] = &DspLowpassFilter::new_object;
  object_factory_map[string(DspMinimum::get_object_label())] = &DspMinimum::new_object;
  object_factory_map[string(DspMultiply::get_object_label())] = &DspMultiply::new_object;
  object_factory_map[string(DspNoise::get_object_label())] = &DspNoise::new_object;
  object_factory_map[string(DspOsc::get_object_label())] = &DspOsc::new_object;
  object_factory_map[string(DspOutlet::get_object_label())] = &DspOutlet::new_object;
  object_factory_map[string(DspPhasor::get_object_label())] = &DspPhasor::new_object;
  object_factory_map[string(DspPrint::get_object_label())] = &DspPrint::new_object;
  object_factory_map[string(DspReceive::get_object_label())] = &DspReceive::new_object;
  object_factory_map[string("r~")] = &DspReceive::new_object;
  object_factory_map[string(DspReciprocalSqrt::get_object_label())] = &DspReciprocalSqrt::new_object;
  object_factory_map[string("q8_rsqrt~")] = &DspReciprocalSqrt::new_object;
  object_factory_map[string(DspRfft::get_object_label())] = &DspRfft::new_object;
  object_factory_map[string(DspRifft::get_object_label())] = &DspRifft::new_object;
  object_factory_map[string(DspSend::get_object_label())] = &DspSend::new_object;
  object_factory_map[string("s~")] = &DspSend::new_object;
  object_factory_map[string(DspSignal::get_object_label())] = &DspSignal::new_object;
  object_factory_map[string(DspSnapshot::get_object_label())] = &DspSnapshot::new_object;
  object_factory_map[string(DspSqrt::get_object_label())] = &DspSqrt::new_object;
  object_factory_map[string("q8_sqrt~")] = &DspSqrt::new_object;
  object_factory_map[string(DspSubtract::get_object_label())] = &DspSubtract::new_object;
  object_factory_map[string(DspTablePlay::get_object_label())] = &DspTablePlay::new_object;
  object_factory_map[string(DspTableRead::get_object_label())] = &DspTableRead::new_object;
  object_factory_map[string(DspTableRead4::get_object_label())] = &DspTableRead4::new_object;
  object_factory_map[string(DspThrow::get_object_label())] = &DspThrow::new_object;
  object_factory_map[string(DspVariableDelay::get_object_label())] = &DspVariableDelay::new_object;
  object_factory_map[string(DspVariableLine::get_object_label())] = &DspVariableLine::new_object;
  object_factory_map[string(DspWrap::get_object_label())] = &DspWrap::new_object;
}

ObjectFactoryMap::~ObjectFactoryMap() {
  // nothing to do
}

void ObjectFactoryMap::register_external_object(const char *object_label, message::Object *(*new_object)(pd::Message *, PdGraph *)) {
  object_factory_map[string(object_label)] = new_object;
}

void ObjectFactoryMap::unregister_external_object(const char *object_label) {
  object_factory_map.erase(string(object_label));
}

message::Object *ObjectFactoryMap::new_object(const char *object_label, pd::Message *init_message, PdGraph *graph) {
  message::Object *(*new_object)(pd::Message *, PdGraph *) = object_factory_map[string(object_label)];
  return (new_object != NULL) ? new_object(init_message, graph) : NULL;
}
