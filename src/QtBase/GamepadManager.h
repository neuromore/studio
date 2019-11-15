/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_GAMEPADMANAGER_H
#define __QTBASE_GAMEPADMANAGER_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/Array.h>
#include "Gamepad.h"
#include <QObject>
#include <QTimer>


class QTBASE_API GamepadManager : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		GamepadManager(QObject* parent=NULL);
		~GamepadManager();

		uint32 GetNumGamepads() const					{ return mGamepads.Size(); }
		Gamepad* GetGamepad(uint32 index) const			{ return mGamepads[index]; }
		Gamepad* FindGamepadWithId(uint32 id) const;
		uint32 FindGamepadIndexWithId(uint32 id) const;

	signals:
		void GamepadConnected(Gamepad* gamepad);
		void GamepadDisconnected(uint32 index);

	private slots:
		void OnGamepadConnected(int gamepadId);
		void OnGamepadDisconnected(int gamepadId);

	private:
		Core::Array<Gamepad*>	mGamepads;
};


#endif