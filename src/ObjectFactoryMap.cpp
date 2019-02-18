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
#include "MessageSend.h"
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
  objectFactoryMap[string(MessageAbsoluteValue::getObjectLabel())] = &MessageAbsoluteValue::new_object;
  objectFactoryMap[string(MessageAdd::getObjectLabel())] = &MessageAdd::new_object;
  objectFactoryMap[string(MessageArcTangent::getObjectLabel())] = &MessageArcTangent::new_object;
  objectFactoryMap[string(MessageArcTangent2::getObjectLabel())] = &MessageArcTangent2::new_object;
  objectFactoryMap[string(MessageBang::getObjectLabel())] = &MessageBang::new_object;
  objectFactoryMap[string("bng")] = &MessageBang::new_object;
  objectFactoryMap[string("b")] = &MessageBang::new_object;
  objectFactoryMap[string(MessageChange::getObjectLabel())] = &MessageChange::new_object;
  objectFactoryMap[string(MessageClip::getObjectLabel())] = &MessageClip::new_object;
  objectFactoryMap[string(MessageCosine::getObjectLabel())] = &MessageCosine::new_object;
  objectFactoryMap[string(MessageCputime::getObjectLabel())] = &MessageCputime::new_object;
  objectFactoryMap[string(MessageDbToPow::getObjectLabel())] = &MessageDbToPow::new_object;
  objectFactoryMap[string(MessageDbToRms::getObjectLabel())] = &MessageDbToRms::new_object;
  objectFactoryMap[string(MessageDeclare::getObjectLabel())] = &MessageDeclare::new_object;
  objectFactoryMap[string(MessageDelay::getObjectLabel())] = &MessageDelay::new_object;
  objectFactoryMap[string("del")] = &MessageDelay::new_object;
  objectFactoryMap[string(MessageDiv::getObjectLabel())] = &MessageDiv::new_object;
  objectFactoryMap[string(MessageDivide::getObjectLabel())] = &MessageDivide::new_object;
  objectFactoryMap[string(MessageEqualsEquals::getObjectLabel())] = &MessageEqualsEquals::new_object;
  objectFactoryMap[string(MessageExp::getObjectLabel())] = &MessageExp::new_object;
  objectFactoryMap[string(MessageFloat::getObjectLabel())] = &MessageFloat::new_object;
  objectFactoryMap[string("f")] = &MessageFloat::new_object;
  objectFactoryMap[string("nbx")] = &MessageFloat::new_object; // number boxes are represented as float objects
  objectFactoryMap[string("hsl")] = &MessageFloat::new_object; // horizontal and vertical sliders are
  objectFactoryMap[string("vsl")] = &MessageFloat::new_object; // represened as float boxes
  objectFactoryMap[string(MessageFrequencyToMidi::getObjectLabel())] = &MessageFrequencyToMidi::new_object;
  objectFactoryMap[string(MessageGreaterThan::getObjectLabel())] = &MessageGreaterThan::new_object;
  objectFactoryMap[string(MessageGreaterThanOrEqualTo::getObjectLabel())] = &MessageGreaterThanOrEqualTo::new_object;
  objectFactoryMap[string(MessageInlet::getObjectLabel())] = &MessageInlet::new_object;
  objectFactoryMap[string(MessageInteger::getObjectLabel())] = &MessageInteger::new_object;
  objectFactoryMap[string("i")] = &MessageInteger::new_object;
  objectFactoryMap[string(MessageLessThan::getObjectLabel())] = &MessageLessThan::new_object;
  objectFactoryMap[string(MessageLessThanOrEqualTo::getObjectLabel())] = &MessageLessThanOrEqualTo::new_object;
  objectFactoryMap[string(MessageLine::getObjectLabel())] = &MessageLine::new_object;
  objectFactoryMap[string("list")] = &MessageListAppend::new_object; // MessageListAppend factory creates any kind of list object
  objectFactoryMap[string(MessageLoadbang::getObjectLabel())] = &MessageLoadbang::new_object;
  objectFactoryMap[string(MessageLog::getObjectLabel())] = &MessageLog::new_object;
  objectFactoryMap[string(MessageLogicalAnd::getObjectLabel())] = &MessageLogicalAnd::new_object;
  objectFactoryMap[string(MessageLogicalOr::getObjectLabel())] = &MessageLogicalOr::new_object;
  objectFactoryMap[string(MessageMakefilename::getObjectLabel())] = &MessageMakefilename::new_object;
  objectFactoryMap[string(MessageMaximum::getObjectLabel())] = &MessageMaximum::new_object;
  objectFactoryMap[string(MessageMessageBox::getObjectLabel())] = &MessageMessageBox::new_object;
  objectFactoryMap[string(MessageMetro::getObjectLabel())] = &MessageMetro::new_object;
  objectFactoryMap[string(MessageMidiToFrequency::getObjectLabel())] = &MessageMidiToFrequency::new_object;
  objectFactoryMap[string(MessageMinimum::getObjectLabel())] = &MessageMinimum::new_object;
  objectFactoryMap[string(MessageModulus::getObjectLabel())] = &MessageModulus::new_object;
  objectFactoryMap[string(MessageMoses::getObjectLabel())] = &MessageMoses::new_object;
  objectFactoryMap[string(MessageMultiply::getObjectLabel())] = &MessageMultiply::new_object;
  objectFactoryMap[string(MessageNotein::getObjectLabel())] = &MessageNotein::new_object;
  objectFactoryMap[string(MessageNotEquals::getObjectLabel())] = &MessageNotEquals::new_object;
  objectFactoryMap[string(MessageOpenPanel::getObjectLabel())] = &MessageOpenPanel::new_object;
  objectFactoryMap[string(MessageOutlet::getObjectLabel())] = &MessageOutlet::new_object;
  objectFactoryMap[string(MessagePack::getObjectLabel())] = &MessagePack::new_object;
  objectFactoryMap[string(MessagePipe::getObjectLabel())] = &MessagePipe::new_object;
  objectFactoryMap[string(MessagePow::getObjectLabel())] = &MessagePow::new_object;
  objectFactoryMap[string(MessagePowToDb::getObjectLabel())] = &MessagePowToDb::new_object;
  objectFactoryMap[string(MessagePrint::getObjectLabel())] = &MessagePrint::new_object;
  objectFactoryMap[string(MessageRandom::getObjectLabel())] = &MessageRandom::new_object;
  objectFactoryMap[string(MessageReceive::getObjectLabel())] = &MessageReceive::new_object;
  objectFactoryMap[string("r")] = &MessageReceive::new_object;
  objectFactoryMap[string(MessageRemainder::getObjectLabel())] = &MessageRemainder::new_object;
  objectFactoryMap[string(MessageRmsToDb::getObjectLabel())] = &MessageRmsToDb::new_object;
  objectFactoryMap[string(MessageRoute::getObjectLabel())] = &MessageRoute::new_object;
  objectFactoryMap[string(MessageSamplerate::getObjectLabel())] = &MessageSamplerate::new_object;
  objectFactoryMap[string(MessageSelect::getObjectLabel())] = &MessageSelect::new_object;
  objectFactoryMap[string("sel")] = &MessageSelect::new_object;
  objectFactoryMap[string(MessageSend::getObjectLabel())] = &MessageSend::new_object;
  objectFactoryMap[string("s")] = &MessageSend::new_object;
  objectFactoryMap[string(MessageSine::getObjectLabel())] = &MessageSine::new_object;
  objectFactoryMap[string(MessageSoundfiler::getObjectLabel())] = &MessageSoundfiler::new_object;
  objectFactoryMap[string(MessageSpigot::getObjectLabel())] = &MessageSpigot::new_object;
  objectFactoryMap[string(MessageSqrt::getObjectLabel())] = &MessageSqrt::new_object;
  objectFactoryMap[string(MessageStripNote::getObjectLabel())] = &MessageStripNote::new_object;
  objectFactoryMap[string(MessageSubtract::getObjectLabel())] = &MessageSubtract::new_object;
  objectFactoryMap[string(MessageSwap::getObjectLabel())] = &MessageSwap::new_object;
  objectFactoryMap[string(MessageSwitch::getObjectLabel())] = &MessageSwitch::new_object;
  objectFactoryMap[string(MessageSymbol::getObjectLabel())] = &MessageSymbol::new_object;
  objectFactoryMap[string(MessageTable::getObjectLabel())] = &MessageTable::new_object;
  objectFactoryMap[string(MessageTableRead::getObjectLabel())] = &MessageTableRead::new_object;
  objectFactoryMap[string(MessageTableWrite::getObjectLabel())] = &MessageTableWrite::new_object;
  objectFactoryMap[string(MessageTangent::getObjectLabel())] = &MessageTangent::new_object;
  objectFactoryMap[string(MessageText::getObjectLabel())] = &MessageText::new_object;
  objectFactoryMap[string(MessageTimer::getObjectLabel())] = &MessageTimer::new_object;
  objectFactoryMap[string(MessageToggle::getObjectLabel())] = &MessageToggle::new_object;
  objectFactoryMap[string("tgl")] = &MessageToggle::new_object;
  objectFactoryMap[string(MessageTrigger::getObjectLabel())] = &MessageTrigger::new_object;
  objectFactoryMap[string("t")] = &MessageTrigger::new_object;
  objectFactoryMap[string(MessageUnpack::getObjectLabel())] = &MessageUnpack::new_object;
  objectFactoryMap[string(MessageUntil::getObjectLabel())] = &MessageUntil::new_object;
  objectFactoryMap[string(MessageValue::getObjectLabel())] = &MessageValue::new_object;
  objectFactoryMap[string("v")] = &MessageValue::new_object;
  objectFactoryMap[string(MessageWrap::getObjectLabel())] = &MessageWrap::new_object;
  
  // dsp objects
  objectFactoryMap[string(DspAdc::getObjectLabel())] = &DspAdc::new_object;
  objectFactoryMap[string(DspAdd::getObjectLabel())] = &DspAdd::new_object;
  objectFactoryMap[string(DspBandpassFilter::getObjectLabel())] = &DspBandpassFilter::new_object;
  objectFactoryMap[string(DspBang::getObjectLabel())] = &DspBang::new_object;
  objectFactoryMap[string(DspCatch::getObjectLabel())] = &DspCatch::new_object;
  objectFactoryMap[string(DspClip::getObjectLabel())] = &DspClip::new_object;
  objectFactoryMap[string(DspCosine::getObjectLabel())] = &DspCosine::new_object;
  objectFactoryMap[string(DspDac::getObjectLabel())] = &DspDac::new_object;
  objectFactoryMap[string(DspDelayRead::getObjectLabel())] = &DspDelayRead::new_object;
  objectFactoryMap[string(DspDelayWrite::getObjectLabel())] = &DspDelayWrite::new_object;
  objectFactoryMap[string(DspDivide::getObjectLabel())] = &DspDivide::new_object;
  objectFactoryMap[string(DspEnvelope::getObjectLabel())] = &DspEnvelope::new_object;
  objectFactoryMap[string(DspHighpassFilter::getObjectLabel())] = &DspHighpassFilter::new_object;
  objectFactoryMap[string(DspInlet::getObjectLabel())] = &DspInlet::new_object;
  objectFactoryMap[string(DspLine::getObjectLabel())] = &DspLine::new_object;
  objectFactoryMap[string(DspLog::getObjectLabel())] = &DspLog::new_object;
  objectFactoryMap[string(DspLowpassFilter::getObjectLabel())] = &DspLowpassFilter::new_object;
  objectFactoryMap[string(DspMinimum::getObjectLabel())] = &DspMinimum::new_object;
  objectFactoryMap[string(DspMultiply::getObjectLabel())] = &DspMultiply::new_object;
  objectFactoryMap[string(DspNoise::getObjectLabel())] = &DspNoise::new_object;
  objectFactoryMap[string(DspOsc::getObjectLabel())] = &DspOsc::new_object;
  objectFactoryMap[string(DspOutlet::getObjectLabel())] = &DspOutlet::new_object;
  objectFactoryMap[string(DspPhasor::getObjectLabel())] = &DspPhasor::new_object;
  objectFactoryMap[string(DspPrint::getObjectLabel())] = &DspPrint::new_object;
  objectFactoryMap[string(DspReceive::getObjectLabel())] = &DspReceive::new_object;
  objectFactoryMap[string("r~")] = &DspReceive::new_object;
  objectFactoryMap[string(DspReciprocalSqrt::getObjectLabel())] = &DspReciprocalSqrt::new_object;
  objectFactoryMap[string("q8_rsqrt~")] = &DspReciprocalSqrt::new_object;
  objectFactoryMap[string(DspRfft::getObjectLabel())] = &DspRfft::new_object;
  objectFactoryMap[string(DspRifft::getObjectLabel())] = &DspRifft::new_object;
  objectFactoryMap[string(DspSend::getObjectLabel())] = &DspSend::new_object;
  objectFactoryMap[string("s~")] = &DspSend::new_object;
  objectFactoryMap[string(DspSignal::getObjectLabel())] = &DspSignal::new_object;
  objectFactoryMap[string(DspSnapshot::getObjectLabel())] = &DspSnapshot::new_object;
  objectFactoryMap[string(DspSqrt::getObjectLabel())] = &DspSqrt::new_object;
  objectFactoryMap[string("q8_sqrt~")] = &DspSqrt::new_object;
  objectFactoryMap[string(DspSubtract::getObjectLabel())] = &DspSubtract::new_object;
  objectFactoryMap[string(DspTablePlay::getObjectLabel())] = &DspTablePlay::new_object;
  objectFactoryMap[string(DspTableRead::getObjectLabel())] = &DspTableRead::new_object;
  objectFactoryMap[string(DspTableRead4::getObjectLabel())] = &DspTableRead4::new_object;
  objectFactoryMap[string(DspThrow::getObjectLabel())] = &DspThrow::new_object;
  objectFactoryMap[string(DspVariableDelay::getObjectLabel())] = &DspVariableDelay::new_object;
  objectFactoryMap[string(DspVariableLine::getObjectLabel())] = &DspVariableLine::new_object;
  objectFactoryMap[string(DspWrap::getObjectLabel())] = &DspWrap::new_object;
}

ObjectFactoryMap::~ObjectFactoryMap() {
  // nothing to do
}

void ObjectFactoryMap::registerExternalObject(const char *objectLabel, message::Object *(*new_object)(pd::Message *, PdGraph *)) {
  objectFactoryMap[string(objectLabel)] = new_object;
}

void ObjectFactoryMap::unregisterExternalObject(const char *objectLabel) {
  objectFactoryMap.erase(string(objectLabel));
}

message::Object *ObjectFactoryMap::new_object(const char *objectLabel, pd::Message *init_message, PdGraph *graph) {
  message::Object *(*new_object)(pd::Message *, PdGraph *) = objectFactoryMap[string(objectLabel)];
  return (new_object != NULL) ? new_object(init_message, graph) : NULL;
}
