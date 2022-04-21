/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "GamepadManager.h"
#include <Core/LogManager.h>
#include <QtGamepad/QGamepad>


using namespace Core;

// constructor
GamepadManager::GamepadManager(QObject* parent) : QObject(parent)
{
	QGamepadManager* gamepadManager = QGamepadManager::instance();

	connect( gamepadManager, &QGamepadManager::gamepadConnected, this, &GamepadManager::OnGamepadConnected );
	connect( gamepadManager, &QGamepadManager::gamepadConnected, this, &GamepadManager::OnGamepadDisconnected );
}


// destructor
GamepadManager::~GamepadManager()
{
}


// find the gamepad with the given id
Gamepad* GamepadManager::FindGamepadWithId(uint32 id) const
{
	// get the number of gamepads, iterate through them and search for the one with the given id
	const uint32 numGamepads = mGamepads.Size();
	for (uint32 i=0; i<numGamepads; ++i)
	{
		Gamepad* gamepad = mGamepads[i];

		// compare the id and return the gamepad in case they match
		if (gamepad->GetId() == id)
			return gamepad;
	}

	// no gamepad with the given id found
	return NULL;
}


// find the gamepad indexfor the one with the given id
uint32 GamepadManager::FindGamepadIndexWithId(uint32 id) const
{
	// get the number of gamepads, iterate through them and search for the one with the given id
	const uint32 numGamepads = mGamepads.Size();
	for (uint32 i=0; i<numGamepads; ++i)
	{
		Gamepad* gamepad = mGamepads[i];

		// compare the id and return the gamepad in case they match
		if (gamepad->GetId() == id)
			return i;
	}

	// no gamepad with the given id found
	return CORE_INVALIDINDEX32;
}


// called when a new gamepad gets connected
void GamepadManager::OnGamepadConnected(int gamepadId)
{
	Gamepad* gamepad = FindGamepadWithId(gamepadId);
	if (gamepad != NULL)
	{
		CORE_ASSERT( false );
		LogError( "GamepadManager::OnGamepadConnected(): Gamepad with id %i already exists.", gamepadId);
		return;
	}

	// add a new gamepad for the given id
	gamepad = new Gamepad( gamepadId, this );
	mGamepads.Add( gamepad );

	emit GamepadConnected(gamepad);
}


// called when a gamepad got disconnected
void GamepadManager::OnGamepadDisconnected(int gamepadId)
{
	const uint32 gamepadIndex = FindGamepadIndexWithId(gamepadId);
	if (gamepadIndex == CORE_INVALIDINDEX32)
	{
		CORE_ASSERT( false );
		LogError( "GamepadManager::OnGamepadDisconnected(): Gamepad with id %i does not exist.", gamepadId);
		return;
	}

	// remove the gamepad at the given index and destruct the gamepad object
	Gamepad* gamepad = mGamepads[gamepadIndex];
	gamepad->deleteLater();
	mGamepads.Remove( gamepadIndex );

	emit GamepadDisconnected(gamepadIndex);
}