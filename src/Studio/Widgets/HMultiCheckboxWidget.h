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

#ifndef __NEUROMORE_HMULTICHECKBOXWIDGET_H
#define __NEUROMORE_HMULTICHECKBOXWIDGET_H

// include required headers
#include "../Config.h"
#include <Core/Array.h>
#include <Core/Color.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>



class HMultiCheckboxWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		HMultiCheckboxWidget(QWidget* parent=NULL);
		virtual ~HMultiCheckboxWidget();

		// initialize or reinitialize
		void ReInit(const Core::Array<Core::String>& names, const Core::Array<Core::String>& tooltips, const Core::Array<Core::Color>& colors, const char* allCheckboxName);

		// helper functions
		uint32 GetHighlightedIndex() const;
		uint32 CalcNumChecked() const;
		uint32 GetNumCheckboxes() const												{ return mCheckboxes.Size(); }
		QCheckBox* GetCheckbox(uint32 index) const									{ if (index >= mCheckboxes.Size()) return NULL; return mCheckboxes[index]; }
		bool IsChecked(uint32 index) const											{ if (index >= mCheckboxes.Size()) return false; return mCheckboxes[index]->isChecked(); }
		void SetChecked(uint32 index, bool checked);
		void CheckXCheckboxes(uint32 numChecked);

		// called by the HMultiCheckboxWidget::MainWidget
		void mouseMoveEvent(QMouseEvent* event);

	signals:
		void SelectionChanged();
		void HighlightedIndexChanged(int index);

	private slots:
		void OnAllCheckbox(int state);
		void OnCheckbox(int state);

	private:
		class MainWidget : public QWidget
		{
			public:
				MainWidget(HMultiCheckboxWidget* parent) : QWidget()				{ mParent = parent; setMouseTracking(true); }
				void mouseMoveEvent(QMouseEvent* event)								{ mParent->mouseMoveEvent(event); }

				HMultiCheckboxWidget* mParent;
		};

		QVBoxLayout*								mLayout;
		MainWidget*									mMainWidget;
		Core::Array<QCheckBox*>						mCheckboxes;
		QCheckBox*									mCheckboxAll;
		uint32										mHighlightedIndex;
};


#endif
