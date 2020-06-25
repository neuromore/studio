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

#ifndef __NEUROMORE_SELECTUSERWINDOW_H
#define __NEUROMORE_SELECTUSERWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Timer.h>
#include "../Config.h"
#include <ImageButton.h>
#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QCheckBox>
#include <User.h>
#include <Widgets/SearchBoxWidget.h>


// window to select one of the users' child users
class SelectUserWindow : public QDialog
{
	Q_OBJECT
	public:
		SelectUserWindow(const User& user, QWidget* parent, bool showSelf = false);
		virtual ~SelectUserWindow();

	signals:
		void OnUserSelected(const User& user);

	private slots:
		void OnSelectionChanged();
		void OnCellDoubleClicked(int row, int column);

		void OnCreateProtocolButtonClicked();
		void OnSelectUserButtonClicked();
		void OnCreateButtonClicked()								{ CreateUser(""); }
		void OnInviteButtonClicked()								{ InviteUser(""); }
		void OnCancelButtonClicked();

		void OnRefreshTimer();
		void OnSearchEdited(const QString& text);
		void OnSearchCleared();

		void OnScrolled();

		void OnRefresh();

	private:
		void AddUser(const User& user);
		bool IsUserListed(const char* userId) const;
		void InviteUser(const Core::String& email);
		void CreateUser(const Core::String& email);

		void RequestNextPage(bool force=false);
		void RequestPage(uint32 pageIndex, bool force=false);

		ImageButton*		mRefreshButton;
		QTimer*				mRefreshTimer;
		QTimer*				mSearchTimer;

		Core::Timer			mLastRefreshTimer;
		QLabel*				mRefreshLabel;
		SearchBoxWidget*	mSearchEdit;
		QTableWidget*		mTableWidget;
		QPushButton*		mCreateProtocolButton;
		QPushButton*		mSelectUserButton;
		QPushButton*		mCreateUserButton;
		QPushButton*		mInviteUserButton;

		bool				mShowSelf;
		const User&			mUser;
		Core::Array<User>	mListedUsers;

		// REST
		uint32				mCurrentPage;
		uint32				mNumTotalPages;

		Core::String		mTempString;
};


#endif
