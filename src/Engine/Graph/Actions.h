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

#ifndef __NEUROMORE_ACTIONS_H
#define __NEUROMORE_ACTIONS_H

// include the required headers
#include "../Config.h"
#include "Action.h"

// TODO merge all Audio/Video/Image actions into one, don't use a 1:1 mapping to the callbacks. Provide combobox attribute for selecting what should be done (play, stop, pause etc)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// play audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API PlayAudioAction : public Action
{
	public:
		enum { TYPE_ID = 0x00700 };
		static const char* Uuid ()																{ return "8c4470fa-596a-11e5-885d-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_VOLUME,
			ATTRIBUTE_ISLOOPING,
			ATTRIBUTE_NUMLOOPS,
			ATTRIBUTE_ALLOWSTREAM
		};

		// constructor & destructor
		PlayAudioAction(Graph* graph) : Action(graph)											{}
		~PlayAudioAction()																		{}

		// FIXME not implemented?
		void Looped();

		void Init() override;
		void OnAttributesChanged() override;
		void Execute()  override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// assets
		void GetAsset(Core::String* outLocation, AssetType* outType, bool* outAllowStreaming) const override final		{ *outLocation = GetStringAttribute(ATTRIBUTE_URL); *outType = Action::ASSET_AUDIO; *outAllowStreaming=GetBoolAttribute(ATTRIBUTE_ALLOWSTREAM); }

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Play Audio Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ PlayAudioAction* clone = new PlayAudioAction(graph); return clone; }

	private:
		uint32 mLoops;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stop audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API StopAudioAction : public Action
{
	public:
		enum { TYPE_ID = 0x00701 };
		static const char* Uuid ()																{ return "25812e20-5baf-11e5-885d-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_URL = 0
		};

		// constructor & destructor
		StopAudioAction(Graph* graph) : Action(graph)											{}
		~StopAudioAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Stop Audio Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ StopAudioAction* clone = new StopAudioAction(graph); return clone; }
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pause audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API PauseAudioAction : public Action
{
	public:
		enum { TYPE_ID = 0x00712 };
		static const char* Uuid ()																{ return "d342524a-f5b6-11e5-9ce9-5e5517507c66"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_UNPAUSE,
		};

		// constructor & destructor
		PauseAudioAction(Graph* graph) : Action(graph)											{}
		~PauseAudioAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Pause Audio Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ PauseAudioAction* clone = new PauseAudioAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// seek audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API SeekAudioAction : public Action
{
	public:
		enum { TYPE_ID = 0x00715 };
		static const char* Uuid ()																{ return "31e94a0e-e781-11e5-9730-9a79f06e9478"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_POSITION,
		};

		// constructor & destructor
		SeekAudioAction(Graph* graph) : Action(graph)											{}
		~SeekAudioAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Seek Audio Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ SeekAudioAction* clone = new SeekAudioAction(graph); return clone; }
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// show image action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API ShowImageAction : public Action
{
	public:
		enum { TYPE_ID = 0x00702 };
		static const char* Uuid ()																{ return "a5cd340a-5bb1-11e5-885d-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_URL = 0
		};

		// constructor & destructor
		ShowImageAction(Graph* graph) : Action(graph)											{}
		~ShowImageAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// assets
		void GetAsset(Core::String* outLocation, AssetType* outType, bool* outAllowStreaming) const override final		{ *outLocation = GetStringAttribute(ATTRIBUTE_URL); *outType = Action::ASSET_IMAGE; *outAllowStreaming=false; }

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Show Image Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ ShowImageAction* clone = new ShowImageAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hide image action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API HideImageAction : public Action
{
	public:
		enum { TYPE_ID = 0x00703 };
		static const char* Uuid ()																{ return "741921b0-5bb3-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		HideImageAction(Graph* graph) : Action(graph)											{}
		~HideImageAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Hide Image Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ HideImageAction* clone = new HideImageAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// show text action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API ShowTextAction : public Action
{
	public:
		enum { TYPE_ID = 0x00704 };
		static const char* Uuid ()																{ return "b5b3b450-5bb3-11e5-885d-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_TEXT = 0,
			ATTRIBUTE_COLOR,
			ATTRIBUTE_DURATION
		};

		// constructor & destructor
		ShowTextAction(Graph* graph) : Action(graph)											{}
		~ShowTextAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Show Text Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ ShowTextAction* clone = new ShowTextAction(graph); return clone; }

	private:
		Core::String mText;
		Core::String mTempPlaceholder;
		Core::String mTempValue;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hide text action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API HideTextAction : public Action
{
	public:
		enum { TYPE_ID = 0x00705 };
		static const char* Uuid ()																{ return "b8f6beaa-5bb3-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		HideTextAction(Graph* graph) : Action(graph)											{}
		~HideTextAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Hide Text Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ HideTextAction* clone = new HideTextAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// set background color action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API SetBackgroundColorAction : public Action
{
public:
	enum { TYPE_ID = 0x00711 };
	static const char* Uuid()																{ return "dfaf04a4-b7c9-11e5-9912-ba0be0483c18"; }

	enum
	{
		ATTRIBUTE_COLOR = 0,
	};

	// constructor & destructor
	SetBackgroundColorAction(Graph* graph) : Action(graph)									{}
	~SetBackgroundColorAction()																{}

	void Init() override;
	void Execute() override;
	void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

	// overloaded
	virtual const char* GetReadableType() const override final								{ return "Set Background Color Action"; }
	uint32 GetType() const override															{ return TYPE_ID; }
	const char* GetTypeUuid() const override final											{ return Uuid(); }
	GraphObject* Clone(Graph* graph) override												{ SetBackgroundColorAction* clone = new SetBackgroundColorAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API CommandAction : public Action
{
	public:
		enum
		{
			ATTRIBUTE_COMMAND = 0
		};

		enum { TYPE_ID = 0x00706 };
		static const char* Uuid ()																{ return "6d1d10e6-7d8d-11e5-8bcf-feff819cdc9f"; }

		// constructor & destructor
		CommandAction(Graph* graph) : Action(graph)												{}
		~CommandAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Command Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ CommandAction* clone = new CommandAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// play video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API PlayVideoAction : public Action
{
	public:
		enum { TYPE_ID = 0x00707 };
		static const char* Uuid ()																{ return "71c620d4-8138-11e5-8bcf-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_VOLUME,
			ATTRIBUTE_ISLOOPING,
			ATTRIBUTE_NUMLOOPS,
			ATTRIBUTE_ALLOWSTREAM
		};

		// constructor & destructor
		PlayVideoAction(Graph* graph) : Action(graph)											{}
		~PlayVideoAction()																		{}

		// FIXME not implemented?
		void Looped();

		void Init() override;
		void OnAttributesChanged() override;
		void Execute()  override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// assets
		void GetAsset(Core::String* outLocation, AssetType* outType, bool* outAllowStreaming) const override final		{ *outLocation = GetStringAttribute(ATTRIBUTE_URL); *outType = Action::ASSET_VIDEO; *outAllowStreaming=GetBoolAttribute(ATTRIBUTE_ALLOWSTREAM); }

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Play Video Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ PlayVideoAction* clone = new PlayVideoAction(graph); return clone; }

	private:
		uint32 mLoops;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stop video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API StopVideoAction : public Action
{
	public:
		enum { TYPE_ID = 0x00708 };
		static const char* Uuid ()																{ return "aa85eeea-8138-11e5-8bcf-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_URL = 0
		};

		// constructor & destructor
		StopVideoAction(Graph* graph) : Action(graph)											{}
		~StopVideoAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Stop Video Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ StopVideoAction* clone = new StopVideoAction(graph); return clone; }
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pause Video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API PauseVideoAction : public Action
{
	public:
		enum { TYPE_ID = 0x00713 };
		static const char* Uuid ()																{ return "d342559c-f5b6-11e5-9ce9-5e5517507c66"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_UNPAUSE,
		};

		// constructor & destructor
		PauseVideoAction(Graph* graph) : Action(graph)											{}
		~PauseVideoAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Pause Video Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ PauseVideoAction* clone = new PauseVideoAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// seek Video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API SeekVideoAction : public Action
{
	public:
		enum { TYPE_ID = 0x00714 };
		static const char* Uuid ()																{ return "d34254a2-f5b6-11e5-9ce9-5e5517507c66"; }

		enum
		{
			ATTRIBUTE_URL = 0,
			ATTRIBUTE_POSITION,
		};

		// constructor & destructor
		SeekVideoAction(Graph* graph) : Action(graph)											{}
		~SeekVideoAction()																		{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Seek Video Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ SeekVideoAction* clone = new SeekVideoAction(graph); return clone; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// parameter action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API ParameterAction : public Action
{
	public:
		enum { TYPE_ID = 0x00709 };
		static const char* Uuid ()																{ return "491cf928-8200-11e5-8bcf-feff819cdc9f"; }

		enum
		{
			ATTRIBUTE_PARAMETERNAME = 0,
			ATTRIBUTE_PARAMETERCHANNELINDEX,
			ATTRIBUTE_FUNCTION,
			ATTRIBUTE_VALUE,
			ATTRIBUTE_FEEDBACKNAME,
			ATTRIBUTE_FEEDBACKCHANNELINDEX,
			ATTRIBUTE_MINVALUE,
			ATTRIBUTE_MAXVALUE
		};

		// constructor & destructor
		ParameterAction(Graph* graph) : Action(graph)											{}
		~ParameterAction()																		{}

		void Init() override;
		void OnAttributesChanged() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Parameter Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ ParameterAction* clone = new ParameterAction(graph); return clone; }

		// available functions
		enum EFunction
		{
			FUNCTION_SET = 0,
			FUNCTION_INCREASE,
			FUNCTION_DECREASE,
			FUNCTION_SCALE,
			FUNCTION_RANDOM,
			FUNCTION_FEEDBACK,
			FUNCTION_MULTIFEEDBACK,
			FUNCTION_NUMFUNCTIONS
		};

		const char* GetFunctionString(EFunction function);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clear buttons action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINE_API ClearButtonsAction : public Action
{
	public:
		enum { TYPE_ID = 0x00710 };
		static const char* Uuid ()																{ return "f637e908-83b1-11e5-8bcf-feff819cdc9f"; }

		// constructor & destructor
		ClearButtonsAction(Graph* graph) : Action(graph)										{}
		~ClearButtonsAction()																	{}

		void Init() override;
		void Execute() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}

		// overloaded
		virtual const char* GetReadableType() const override final								{ return "Clear Buttons Action"; }
		uint32 GetType() const override															{ return TYPE_ID; }
		const char* GetTypeUuid() const override final											{ return Uuid(); }
		GraphObject* Clone(Graph* graph) override												{ ClearButtonsAction* clone = new ClearButtonsAction(graph); return clone; }
};


#endif
