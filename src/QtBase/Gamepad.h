/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_GAMEPAD_H
#define __QTBASE_GAMEPAD_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/Vector.h>
#include "QtBaseConfig.h"
#include <QObject>


// forward declaration
class QGamepad;

class QTBASE_API Gamepad : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		Gamepad(uint32 id, QObject* parent=NULL);
		~Gamepad();

		uint32 GetId() const								{ return mId; }

		bool IsConnected() const;
		Core::String GetNameString() const;

		// left joystick
		Core::Vector2 GetLeftAxis() const					{ return mLeftAxis; }
		double GetLeftAxisX() const							{ return mLeftAxis.x; }
		double GetLeftAxisY() const							{ return mLeftAxis.y; }

		// right joystick
		Core::Vector2 GetRightAxis() const					{ return mRightAxis; }
		double GetRightAxisX() const						{ return mRightAxis.x; }
		double GetRightAxisY() const						{ return mRightAxis.y; }

		// abxy buttons
		bool GetButtonA() const								{ return mButtonA; }
		bool GetButtonB() const								{ return mButtonB; }
		bool GetButtonX() const								{ return mButtonX; }
		bool GetButtonY() const								{ return mButtonY; }

		// left & right fire buttons
		bool GetButtonL1() const							{ return mButtonL1; }
		bool GetButtonL2() const							{ return mButtonL2; }
		bool GetButtonL3() const							{ return mButtonL3; }
		bool GetButtonR1() const							{ return mButtonR1; }
		bool GetButtonR2() const							{ return mButtonR2; }
		bool GetButtonR3() const							{ return mButtonR3; }

		// misc buttons
		bool GetButtonSelect() const						{ return mButtonSelect; }
		bool GetButtonStart() const							{ return mButtonStart; }
		bool GetButtonGuide() const							{ return mButtonGuide; }

		// joystick buttons
		bool GetButtonUp() const							{ return mButtonUp; }
		bool GetButtonDown() const							{ return mButtonDown; }
		bool GetButtonLeft() const							{ return mButtonLeft; }
		bool GetButtonRight() const							{ return mButtonRight; }
		bool GetButtonCenter() const						{ return mButtonCenter; }

	public slots:
		void Update();

	private:
		void Init();

		QGamepad*			mGamepad;
		uint32				mId;

		Core::Vector2		mLeftAxis;
		Core::Vector2		mRightAxis;

		bool				mButtonA;
		bool				mButtonB;
		bool				mButtonX;
		bool				mButtonY;

		bool				mButtonL1;
		bool				mButtonL2;
		bool				mButtonL3;
		bool				mButtonR1;
		bool				mButtonR2;
		bool				mButtonR3;

		bool				mButtonSelect;
		bool				mButtonStart;
		bool				mButtonGuide;

		bool				mButtonUp;
		bool				mButtonDown;
		bool				mButtonLeft;
		bool				mButtonRight;
		bool				mButtonCenter;
};


#endif