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

#ifndef __NEUROMORE_VISUALIZATIONSELECTWINDOW_H
#define __NEUROMORE_VISUALIZATIONSELECTWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Array.h>
#include "../VisualizationManager.h"
#include "../Config.h"
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


class VisualizationSelectWidget : public QWidget
{
	Q_OBJECT
	public:
		VisualizationSelectWidget(QGridLayout* gridLayout, Visualization* visualization, uint32 thumbnailWidth, QWidget* parent);
		virtual ~VisualizationSelectWidget();

		void UpdateInterface();
		bool IsHovered() const						{ return mIsHovered; }

	signals:
		void HoverStateChanged();
		void VisualizationSelected();

	private slots:
		void OnHoverStateChanged()					{ emit HoverStateChanged(); }
		void OnSelectVisualization();

	private:
		Visualization*							mVisualization;

		void paintEvent(QPaintEvent* event);

		QPixmap									mThumbnailPixmap;
		QPixmap									mThumbnailPixmapDisabled;
		QLabel*									mThumbnailLabel;
		QLabel*									mNameLabel;
		QLabel*									mDescriptionLabel;
		QPushButton*							mSelectButton;

		// mouse events
		void enterEvent(QEvent* event);
		void leaveEvent(QEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);
		
		bool									mIsHovered;
};


class VisualizationSelectWindow : public QDialog
{
	Q_OBJECT
	public:
		VisualizationSelectWindow(QWidget* parent);
		virtual ~VisualizationSelectWindow();

		void UpdateInterface();

	protected:
		Core::Array<VisualizationSelectWidget*>	mVizWidgets;
};


#endif
