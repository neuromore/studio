/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_SEARCHWIDGET_H
#define __NEUROMORE_SEARCHWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <QWidget>
#include <QLineEdit>

// forward declarations
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)


class QTBASE_API SearchBoxEdit : public QLineEdit
{
	Q_OBJECT
	public:
		SearchBoxEdit(QWidget* parent = 0) : QLineEdit(parent) 
		{
			setObjectName("SearchEdit");
			mDefaultText = "Search"; 
			Reset();
		}
	
		void Reset()	
		{ 
			mIsSearching = false;
			setText(mDefaultText);
		}

	private:
		void focusInEvent(QFocusEvent* event) override
		{
			mIsSearching = true;
			clear();
			QLineEdit::focusInEvent(event);
		}	
		
		QString mDefaultText;
		bool mIsSearching;
};


class QTBASE_API SearchBoxWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		SearchBoxWidget(QWidget* parent=NULL);
		virtual ~SearchBoxWidget();

		const QString& GetText();

	signals:
		void TextChanged(const QString & text);
		void TextCleared();

	public slots:
		void ClearSearch();

	private slots:
		void OnTextChanged(const QString & text);

	private:
		QHBoxLayout*	mLayout;
		SearchBoxEdit*	mSearchEdit;

		QString			mText;	// local copy for getter
};


#endif
