/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as
** appearing in the file neuromore-class-exception.md included in the
** packaging of this file. Please review the following information to
** ensure the neuromore Public License requirements will be met:
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "ToneGeneratorNode.h"

using namespace Core;

ToneGeneratorNode::ToneGeneratorNode(Graph* graph) : SPNode(graph),
   mInstrument(INSTRUMENT_SINEWAVE),
   mSineWave(),
   mClarinet(),
   mFlute(1.0),
   mGuitar(),
   mSitar(),
   mLastToneDelta(1.0),
   mSamplesAhead(0.0),
   mSamplesEnd(&mSamples[AUDIOBUFFERSAMPLES]),
   mBufferWrite(mBuffer),
   mBufferRead(mBuffer),
   mEnabled(true)
{
   stk::Stk::setSampleRate(AUDIOSAMPLERATE);
   assert(AUDIOSAMPLERATE == (int)stk::Stk::sampleRate());
}

ToneGeneratorNode::~ToneGeneratorNode()
{
}

void ToneGeneratorNode::Init()
{
   // init base class first
   SPNode::Init();

   // CONFIG SPNODE
   RequireSyncedInput();

   // SETUP PORTS
   InitInputPorts(5);
   GetInputPort(INPUTPORT_INSTRUMENT).Setup("Instrument", "instrument", AttributeChannels<double>::TYPE_ID, INPUTPORT_INSTRUMENT);
   GetInputPort(INPUTPORT_FREQUENCY).Setup("Frequency", "frequency", AttributeChannels<double>::TYPE_ID, INPUTPORT_FREQUENCY);
   GetInputPort(INPUTPORT_AMPLITUDE).Setup("Amplitude", "amplitude", AttributeChannels<double>::TYPE_ID, INPUTPORT_AMPLITUDE);
   GetInputPort(INPUTPORT_DELAY).Setup("Delay", "delay", AttributeChannels<double>::TYPE_ID, INPUTPORT_DELAY);
   GetInputPort(INPUTPORT_ENABLED).Setup("Enabled", "enabled", AttributeChannels<double>::TYPE_ID, INPUTPORT_ENABLED);
}

void ToneGeneratorNode::ResetTone()
{
   mSineWave.reset();      // reset tone
   mClarinet.clear();      // reset clarinet
   mFlute.clear();         // reset flute
   mGuitar.clear();        // reset guitar
   mSitar.clear();         // reset sitar
   mLastToneDelta = 1.0;   // reset lasttonedelta
   mSamplesAhead = 0.0;    // reset samples ahead counter
   mBufferWrite = mBuffer; // reset write position
   mBufferRead  = mBuffer; // reset read position
}

void ToneGeneratorNode::Start(const Time& elapsed)
{
   SPNode::Start(elapsed);
   ResetTone();
}

void ToneGeneratorNode::ReInit(const Time& elapsed, const Time& delta)
{
   if (BaseReInit(elapsed, delta) == false)
      return;

   SPNode::ReInit(elapsed, delta);
   PostReInit(elapsed, delta);
}

void ToneGeneratorNode::Update(const Time& elapsed, const Time& delta)
{
   if (BaseUpdate(elapsed, delta) == false)
      return;

   // update the baseclass
   SPNode::Update(elapsed, delta);

   // get input ports
   InputPort& instrumentInput = GetInputPort(INPUTPORT_INSTRUMENT);
   InputPort& freqInput       = GetInputPort(INPUTPORT_FREQUENCY);
   InputPort& amplitudeInput  = GetInputPort(INPUTPORT_AMPLITUDE);
   InputPort& delayInput      = GetInputPort(INPUTPORT_DELAY);
   InputPort& enabledInput    = GetInputPort(INPUTPORT_ENABLED);

   // whether to reset later or not
   bool doreset = false;

   // check enabled state
   if (enabledInput.HasConnection())
   {
      const double v = enabledInput.GetChannels()
         ->GetChannel(0)
         ->AsType<double>()
         ->GetLastSample();
      if (v < 1.0 && mEnabled) {
         mEnabled = false;
      }
      else if (v >= 1.0 && !mEnabled) {
         mEnabled = true;
         doreset  = true;
      }
   }
   else
      mEnabled = true;

   // must be enabled and freq must be connected
   if (!mEnabled || !freqInput.HasConnection())
      return;

   // get last frequency
   const double freq = freqInput.GetChannels()
      ->GetChannel(0)
      ->AsType<double>()
      ->GetLastSample();

   // get last amplitude (internal volume of instrument, e.g. 'piano' vs. 'forte')
   const double amplitude = amplitudeInput.HasConnection() ? amplitudeInput.GetChannels()
      ->GetChannel(0)
      ->AsType<double>()
      ->GetLastSample() : DEFAULTAMPLITUDE;

   // get last delay
   const double delay = delayInput.HasConnection() ? delayInput.GetChannels()
      ->GetChannel(0)
      ->AsType<double>()
      ->GetLastSample() : DEFAULTDELAY;

   // get last instrument
   const uint32_t instrument = (uint32_t)(instrumentInput.HasConnection() ? instrumentInput.GetChannels()
      ->GetChannel(0)
      ->AsType<double>()
      ->GetLastSample() : 0.0);

   // check for instrument change
   if (instrument != mInstrument)
   {
      mInstrument = instrument;
      doreset = true;
   }

   // do reset if necessary
   if (doreset)
      ResetTone();

   // add delta
   mLastToneDelta += delta.InSeconds();

   // update
   if (mLastToneDelta >= delay)
   {
      switch (mInstrument) {
      case INSTRUMENT_CLARINET: mClarinet.noteOn(freq, ::std::min(amplitude, 1.0)); break;
      case INSTRUMENT_FLUTE:    mFlute.noteOn(freq, ::std::min(amplitude, 1.0));    break;
      case INSTRUMENT_GUITAR:   mGuitar.noteOn(freq, amplitude);   break;
      case INSTRUMENT_SITAR:    mSitar.noteOn(freq, amplitude);    break;
      default:                  mSineWave.setFrequency(freq);      break;
      }
      mLastToneDelta = 0.0;
   }

   // calculate the number of samples to generate for last delta
   double   secs  = delta.InSeconds();
   double   frms  = AUDIOSAMPLERATE * secs;
   uint32_t nfrms = (uint32_t)frms; // number of samples
   double   rfrms = frms - nfrms;   // fraction/remainder

   // remove missed fraction from samples ahead
   mSamplesAhead -= rfrms;

   // samples missing to reach desired ahead state
   const double missing = DESIREDSAMPLESAHEAD - mSamplesAhead;

   // generate additional samples
   if (missing >= 1.0)
   {
      const uint32_t v = (uint32_t)missing;
      nfrms         += v;
      mSamplesAhead += v;
   }

   // position and end
   float*       p = mSamplesWrite;
   const float* e = mSamplesEnd;

   // generate samples
   while (nfrms) {
      if (p >= e) p = mSamples;
      switch (mInstrument) {
      case INSTRUMENT_CLARINET: *p++ = (float)mClarinet.tick(); break;
      case INSTRUMENT_FLUTE:    *p++ = (float)mFlute.tick();    break;
      case INSTRUMENT_GUITAR:   *p++ = (float)mGuitar.tick();   break;
      case INSTRUMENT_SITAR:    *p++ = (float)mSitar.tick();    break;
      default:                  *p++ = (float)mSineWave.tick(); break;
      }
      nfrms--;
   }

   // save back index
   mSamplesWrite = p;
}

void ToneGeneratorNode::OnAttributesChanged()
{
}

bool ToneGeneratorNode::GetChunk(char*& buffer, uint32_t& length)
{
   char* r = mBufferRead;
   char* w = mBufferWrite;
   if (w > r) {
      buffer = r;
      length = (uint32_t)(w-r);
      return true;
   }
   else if (w < r) {
      buffer = r;
      length = (uint32_t)(mBufferEnd-r);
      return true;
   }
   else {
      buffer = 0;
      length = 0;
      return false;
   }
}

void ToneGeneratorNode::DidChunk(uint32_t length)
{
   char*       r = mBufferRead;
   const char* e = mBufferEnd;
   const char* re = r + length;

   // set new read position
   mBufferRead = (re < e) ? (char*)re :
      mBuffer + (length-(e-r));
}
