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

#ifndef __NEUROMORE_SESSIONINFOWIDGET_H
#define __NEUROMORE_SESSIONINFOWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/EventManager.h>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QGridLayout>

class SessionInfoWidget : public QWidget, public Core::EventHandler
{
	Q_OBJECT
	public:
		SessionInfoWidget(QWidget* parent = NULL);
		virtual ~SessionInfoWidget();

		enum EInfoType
		{
			TYPE_INFO,
			TYPE_WARNING,
			TYPE_ERROR,

			NUM_TYPES
		};


		void ShowInfo(EInfoType type, const char* message, const char* description, double duration = 0);
		void RemoveInfo(const char* message);
		void Clear();
		
		bool HasInfo(const char* message);

	private slots:
		void OnTimerEvent();

	private:
		void UpdateLayout();
		void RemoveAt(uint32 index);

		struct Info
		{
			EInfoType  	 mType;
			Core::String mMessage;
			Core::String mDescription;

			// references to the widgets in the layout
			QLabel*		 mIcon;
			QLabel*	 	 mLabel;
			
			
			bool		 mAutoRemove;
			double		 mTimeRemaining;
			bool		 mIsFading;
			bool		 mTimedOut;
		};

		Core::Array<Info>	mInfos;

		QGridLayout*		mGridLayout;
		QTimer*				mTimer;

		Core::Array<QPixmap> mIcons;	// size=NUM_TYPES, same order as EInfoType

		const QPixmap& GetIcon(EInfoType type)		{ return mIcons[type]; }
		const char* GetHtmlColor(EInfoType type);
		const char* GetName(EInfoType type);
};


#endif
