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

#ifndef __NEUROMORE_NOTIFICATIONS_H
#define __NEUROMORE_NOTIFICATIONS_H

// Note: somewhat missleading name (not sure in what direction we'll develop this). Its only used as a central place for the ids right now.
class Notifications
{
  public:

	// note: we can't use the first bit (assume signed int32)
	// second bit: type
	enum EType
	{
		MASK_TYPE			= 1 << 30,

		TYPE_EVENT			= 0,			// not clearable (happens only once)
		TYPE_STATE			= 1 << 30,		// clearable (will be cleared by caller once resolved); only for warnings and errors
	};

	static EType GetType (uint32 id)		{ if (id & EType::MASK_TYPE) return TYPE_STATE; else return TYPE_EVENT; }	


	// third to fourth bit: class
	enum EClass
	{
		MASK_CLASS			= 0b11 << 28,

		CLASS_INFO			= 1 << 28,
		CLASS_WARNING		= 2 << 28,
		CLASS_ERROR			= 3 << 28,

		// class/state combination
		CLASS_WARNINGEVENT	= TYPE_EVENT | CLASS_WARNING,
		CLASS_WARNINGSTATE  = TYPE_STATE | CLASS_WARNING,
		CLASS_ERROREVENT	= TYPE_EVENT | CLASS_ERROR,
		CLASS_ERRORSTATE	= TYPE_STATE | CLASS_ERROR,
	};

	static EClass GetClass (uint32 id)		{ return (EClass)(id & EClass::MASK_CLASS); }	
	

	// second nibble
	enum ESource
	{
		MASK_SOURCE			= 0b1111 << (4 * 4),

		SOURCE_DEVICE		= 1 << (4 * 4),				// device errors like battery low or sensor drift
		SOURCE_GRAPHOBJECT	= 2 << (4 * 4),				// graph object errors from certain nodes, states etc
		SOURCE_DESIGN		= 3 << (4 * 4),				// design (experience) errors (e.g. 'nodes in graph have errors')
		SOURCE_SESSION		= 4 << (4 * 4),				// session abort due to device loss or statemachine exit state
	};

	static ESource GetSource (uint32 id)	{ return (ESource)(id & ESource::MASK_SOURCE); }	


	// error codes (lower 3 bytes)
	enum EError
	{
		// devices
		ERROR_DEVICE_BADSIGNAL			= CLASS_ERROREVENT | SOURCE_DEVICE | 0x001,		// signal reception is too bad (= lost packets and drift correction)
		ERROR_DEVICE_DISCONNECTED		= CLASS_ERROREVENT | SOURCE_DEVICE | 0x002,		// device was lost during a session

		// graphobjects
		// TODO move all graph errors here
		//ERROR_GRAPHOBJECT_DEVICEINPUTNODE = CLASS_ERROR | SOURCE_GRAPHOBJECT | 

		// design
		ERROR_DESIGN_GRAPHOBJECT		= CLASS_ERRORSTATE | SOURCE_DESIGN | 0x001,		// one of the nodes is in an error state
		ERROR_DESIGN_LOADGRAPHOBJECT	= CLASS_ERROREVENT | SOURCE_DESIGN | 0x002,		// error while loading a graph object
		ERROR_DESIGN_PERMISSION			= CLASS_ERROREVENT | SOURCE_DESIGN | 0x003,		// insufficient permissions while loading design

		// session
		ERROR_SESSION_ABORTED			= CLASS_ERROREVENT | SOURCE_SESSION | 0x001,	// session was aborted for some reason (engine or by statemachine)
		ERROR_SESSION_INIT				= CLASS_ERROREVENT | SOURCE_SESSION | 0x002,	// something went wrong during initialization of the session
		ERROR_SESSION_UPLOAD			= CLASS_ERROREVENT | SOURCE_SESSION | 0x003,	// something went wrong during the channel data or parameter upload
	};


	// warning codes
	enum EWarning
	{
		// devices
		WARNING_DEVICE_POWERSTATECRITICAL = CLASS_WARNINGSTATE | SOURCE_DEVICE | 0x001,	// power reached critical state
		WARNING_DEVICE_CONNECTIONISSUE	  = CLASS_WARNINGSTATE | SOURCE_DEVICE | 0x002,	// device connection is bordering the unacceptable state (much drift, some lost packets)
		
		// graphobjects
		// TODO move all graph object warnings here
		//WARNING_GRAPHOBJECT_AUDIOOUTPUTNODE_NOINPUT = CLASS_WARNING | SOURCE_GRAPHOBJECT | 0x001,
							
		// design
		WARNING_DESIGN_GRAPHOBJECT		= CLASS_WARNINGSTATE | SOURCE_DESIGN | 0x001,		// one of the nodes is in a warning state
		WARNING_DESIGN_DEPRECATEDOBJECT	= CLASS_WARNINGEVENT | SOURCE_DESIGN | 0x002,		// graph contains a deprecated object
	};

	
	// info codes
	enum EInfo
	{
		// device
		INFO_DEVICE_CONNECTED			= CLASS_INFO | SOURCE_DEVICE | 0x001,
		INFO_DEVICE_DISCONNECTED		= CLASS_INFO | SOURCE_DEVICE | 0x002,
		
		// session
		INFO_SESSION_ENDED				= CLASS_INFO | SOURCE_SESSION | 0x001,				// session ended successfully
		INFO_SESSION_SUCCESSFUL			= CLASS_INFO | SOURCE_SESSION | 0x002,
	};






};
#endif
