/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "Gamepad.h"
#include <QtGamepad/QGamepad>
#include <Core/LogManager.h>


using namespace Core;

// constructor
Gamepad::Gamepad(uint32 id, QObject* parent) : QObject(parent)
{
	mGamepad	= NULL;
	mId			= id;

	// initialize
	Init();
}


// destructor
Gamepad::~Gamepad()
{
}


// initialize gamepad
void Gamepad::Init()
{
	// construct gamepad
	mGamepad = new QGamepad( mId, this );

	connect( mGamepad, &QGamepad::axisLeftXChanged,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::axisLeftYChanged,		this, &Gamepad::Update );

	connect( mGamepad, &QGamepad::axisRightXChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::axisRightYChanged,	this, &Gamepad::Update );

	connect( mGamepad, &QGamepad::buttonAChanged,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonBChanged,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonXChanged,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonYChanged,		this, &Gamepad::Update );

	connect( mGamepad, &QGamepad::buttonL1Changed,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonL2Changed,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonL3Changed,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonR1Changed,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonR2Changed,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonR3Changed,		this, &Gamepad::Update );

	connect( mGamepad, &QGamepad::buttonSelectChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonStartChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonGuideChanged,	this, &Gamepad::Update );

	connect( mGamepad, &QGamepad::buttonUpChanged,		this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonDownChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonLeftChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonRightChanged,	this, &Gamepad::Update );
	connect( mGamepad, &QGamepad::buttonCenterChanged,	this, &Gamepad::Update );
}


// sync gamepad states
void Gamepad::Update()
{
	// make sure our gamepad is valid
	if (mGamepad == NULL)
	{
		LogError("Gamepad::Update(): Gamepad not valid.");
		return;
	}

	// is the gamepad still connected?
	if (IsConnected() == false)
	{
		LogWarning("Gamepad::Update(): Gamepad is not connected.");
		return;
	}

	// axis left
	mLeftAxis.x		= mGamepad->axisLeftX();
	mLeftAxis.y		= mGamepad->axisLeftY();

	// axis right
	mRightAxis.x	= mGamepad->axisRightX();
	mRightAxis.y	= mGamepad->axisRightY();

	// abxy buttons
	mButtonA		= mGamepad->buttonA();
	mButtonB		= mGamepad->buttonB();
	mButtonX		= mGamepad->buttonX();
	mButtonY		= mGamepad->buttonY();
	
	// left and right fire buttons
	mButtonL1		= mGamepad->buttonL1();
	mButtonL2		= mGamepad->buttonL2();
	mButtonL3		= mGamepad->buttonL3();
	mButtonR1		= mGamepad->buttonR1();
	mButtonR2		= mGamepad->buttonR2();
	mButtonR3		= mGamepad->buttonR3();
	
	// misc buttons
	mButtonSelect	= mGamepad->buttonSelect();
	mButtonStart	= mGamepad->buttonStart();
	mButtonGuide	= mGamepad->buttonGuide();
	
	// joystick buttons
	mButtonUp		= mGamepad->buttonUp();
	mButtonDown		= mGamepad->buttonDown();
	mButtonLeft		= mGamepad->buttonLeft();
	mButtonRight	= mGamepad->buttonRight();
	mButtonCenter	= mGamepad->buttonCenter();
}


// is the gamepad connected?
bool Gamepad::IsConnected() const
{
	// make sure our gamepad is valid
	if (mGamepad == NULL)
	{
		LogError("Gamepad::IsConnected(): Gamepad not valid.");
		return false;
	}

	return mGamepad->isConnected();
}


// get the name of the gamepad
Core::String Gamepad::GetNameString() const
{
	// make sure our gamepad is valid
	if (mGamepad == NULL)
	{
		LogError("Gamepad::GetNameString(): Gamepad not valid.");
		return "";
	}

	return mGamepad->name().toUtf8().data();
}