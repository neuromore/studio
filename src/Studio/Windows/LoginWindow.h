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

#ifndef __NEUROMORE_LOGINWINDOW_H
#define __NEUROMORE_LOGINWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "../Config.h"
#include <LinkWidget.h>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>


/*
 * Usage:
 * 	LoginWindow* loginWindow = new LoginWindow(this);
 *	loginWindow->exec();
 *	Core::LogInfo( "User Name: %s", FromQtString(loginWindow->GetUserId().AsChar() );
 *	Core::LogInfo( "Password:  %s", FromQtString(loginWindow->GetPassword().AsChar() );
 */
class LoginWindow : public QDialog
{
	Q_OBJECT
	public:
		LoginWindow(QWidget* parent, const char* title, const char* firstLineEdit, const char* firstLineEditText, const char* secondLineEdit, const char* secondLineEditText, const char* buttonText, bool passwordHidden=true);
		virtual ~LoginWindow();

		QString GetUserId()							{ return mUserId->text().trimmed(); }
		QString GetPassword()						{ return mPassword->text().trimmed(); }
		bool GetRememberMe()						{ return mRememberMe->isChecked(); }

		bool RememberMe() const						{ if (mRememberMe == NULL) return false; return mRememberMe->isChecked(); }

	signals:
		void Login();

	private slots:
		void OnUserIdEdited(const QString& text);
		void OnPasswordEdited(const QString& text);
		void OnLogin();

	protected:
		QWidget*		mGroupingWidget;
		QLineEdit*		mUserId;
		QLineEdit*		mPassword;
		LinkWidget*		mForgotPassword;
		QPushButton*	mLogin;
		QCheckBox*		mRememberMe;
};


#endif
