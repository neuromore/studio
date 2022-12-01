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

#ifndef __NEUROMORE_TONEGENERATORNODE_H
#define __NEUROMORE_TONEGENERATORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"

// STK
#include <stk/SineWave.h>

class ENGINE_API ToneGeneratorNode : public SPNode
{
public:
   enum { TYPE_ID = 0x005A };
   static const char* Uuid() { return "966f12c6-72b0-40aa-8a2b-6ce3cf1359e1"; }

   // sample rate used when generation audio
   static constexpr int AUDIOSAMPLERATE = 8000;
   
   // number of samples that can be stored in the buffer (4000=500ms)
   static constexpr int AUDIOBUFFERSAMPLES = AUDIOSAMPLERATE / 2;

   enum
   {
      INPUTPORT_FREQUENCY = 0,
      INPUTPORT_ENABLED   = 1
   };

   // constructor & destructor
   ToneGeneratorNode(Graph* graph);
   ~ToneGeneratorNode();

   // init, reinit & update
   void Init() override;
   void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
   void Start(const Core::Time& elapsed) override;
   void Update(const Core::Time& elapsed, const Core::Time& delta) override;

   void OnAttributesChanged() override;

   Core::Color GetColor() const override { return Core::RGBA(251, 94, 55); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "Tone Generator"; }
   const char* GetRuleName() const override final { return "NODE_ToneGenerator"; }
   uint32 GetPaletteCategory() const override { return CATEGORY_OUTPUT; }
   GraphObject* Clone(Graph* graph) override { ToneGeneratorNode* clone = new ToneGeneratorNode(graph); return clone; }

private:
   stk::SineWave   mSineWave;
   double          mFramesRemainder;
   union {
      const float* mSamplesEnd;
      const char*  mBufferEnd;
   };
   char*           mBufferRead;
   union {
      float*       mSamplesWrite;
      char*        mBufferWrite;
   };
   union {
      float        mSamples[AUDIOBUFFERSAMPLES];
      char         mBuffer[AUDIOBUFFERSAMPLES*4U];
   };
   bool            mEnabled;

public:
   void ResetTone();
   bool GetChunk(char*& buffer, uint32_t& length);
   void DidChunk(uint32_t length);
};


#endif
