/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_ENTERLABELWINDOW_H
#define __NEUROMORE_ENTERLABELWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "../QtBaseConfig.h"
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


class QTBASE_API EnterLabelWindow : public QDialog
{
	Q_OBJECT
	public:
		EnterLabelWindow(const char* windowTitle, const char* defaultLabelText, const Core::String& invalidChars, QWidget* parent=NULL);
		virtual ~EnterLabelWindow();

		Core::String GetLabelText() const						{ return FromQtString( mLineEdit->text() ); }

	private slots:
		void OnTextChanged(const QString& newText);
		void OnOkButtonPressed();
		void OnCancelButtonPressed();

	private:
		QLineEdit*		mLineEdit;
		Core::String	mInvalidChars;
		QPushButton*	mOkButton;
		QPushButton*	mCancelButton;
};


#endif
