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
   mSineWave(),
   mFramesRemainder(0.0),
   mSamplesEnd(&mSamples[AUDIOBUFFERSAMPLES]),
   mBufferWrite(mBuffer),
   mBufferRead(mBuffer)
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
   InitInputPorts(1);
   GetInputPort(INPUTPORT_FREQUENCY).Setup("Frequency", "freq", AttributeChannels<double>::TYPE_ID, INPUTPORT_FREQUENCY);
}

void ToneGeneratorNode::Start(const Time& elapsed)
{
   SPNode::Start(elapsed);

   mSineWave.reset();       // reset tone
   mFramesRemainder = 0.0;  // reset frames remainder
   mBufferWrite = mBuffer;  // reset write position
   mBufferRead  = mBuffer;  // reset read position
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

   // get frequency input port
   InputPort& freqInput = GetInputPort(INPUTPORT_FREQUENCY);

   // must be connected
   if (!freqInput.HasConnection())
      return;

   // get last frequency
   const double freq = freqInput.GetChannels()
      ->GetChannel(0)
      ->AsType<double>()
      ->GetLastSample();

   // set input frequency on STK sinewave
   mSineWave.setFrequency(freq);

   // calculate the number of samples to generate for last delta
   double   secs  = delta.InSeconds();
   double   frms  = AUDIOSAMPLERATE * secs;
   uint32_t nfrms = (uint32_t)frms;
   double   rfrms = frms - nfrms;

   // don't forget the fraction
   mFramesRemainder += rfrms;

   // and increment frames if fractions sum up to more than 1
   if (mFramesRemainder > 1.0) {
      mFramesRemainder -= 1.0;
      nfrms++;
   }

   // position and end
   float*       p = mSamplesWrite;
   const float* e = mSamplesEnd;

   // generate samples
   while (nfrms) {
      if (p >= e) p = mSamples;
      *p++ = (float)mSineWave.tick();
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
