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

// include precompiled header
#include <Studio/Precompiled.h>

// include the required headers
#include "VisualizationSelectWindow.h"
#include "../AppManager.h"
#include "../MainWindow.h"
#include <Core/LogManager.h>
#include <ImageButton.h>


using namespace Core;

// constructor
VisualizationSelectWindow::VisualizationSelectWindow(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	// set the window title
	setWindowTitle( "Select Visualization" );

	setModal(true);
	setWindowModality( Qt::ApplicationModal );

	// add the main layout
	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);

	// get the visualization manager and the number of available visualizations
	VisualizationManager* vizManager = GetManager()->GetVisualizationManager();
	const uint32 numVisualizations = vizManager->GetNumVisualizations();

	// tumbnail width calculations
	const uint32 mainWindowWidth = GetMainWindow()->width();
	uint32 thumbnailWidth = mainWindowWidth / (numVisualizations+1);

	// iterate through the available visualizations
	for (uint32 i=0; i<numVisualizations; ++i)
	{
		Visualization* visualization = vizManager->GetVisualization(i);

		// create the visualization widget
		VisualizationSelectWidget* vizWidget = new VisualizationSelectWidget( mainLayout, visualization, thumbnailWidth, this );
		connect( vizWidget, &VisualizationSelectWidget::HoverStateChanged, this, &VisualizationSelectWindow::UpdateInterface );
		connect( vizWidget, &VisualizationSelectWidget::VisualizationSelected, this, &VisualizationSelectWindow::accept );

		mainLayout->addWidget( vizWidget, 0, i );
		mVizWidgets.Add( vizWidget );
	}

	if (numVisualizations == 0)
		mainLayout->addWidget( new QLabel("No visualizations available"), 0, 0 );


	// avoid resizing
	setSizeGripEnabled(false);
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	setFixedSize( sizeHint() );

	// position window in the screen center
	GetQtBaseManager()->CenterToScreen(this);

	show();
}


// destructor
VisualizationSelectWindow::~VisualizationSelectWindow()
{
}


// update interface
void VisualizationSelectWindow::UpdateInterface()
{
	const uint32 numVizWidgets = mVizWidgets.Size();
	for (uint32 i=0; i<numVizWidgets; ++i)
	{
		VisualizationSelectWidget* vizWidget = mVizWidgets[i];
		vizWidget->UpdateInterface();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
VisualizationSelectWidget::VisualizationSelectWidget(QGridLayout* gridLayout, Visualization* visualization, uint32 thumbnailWidth, QWidget* parent) : QWidget(parent)
{
	mVisualization = visualization;

	QVBoxLayout* vLayout = new QVBoxLayout();
	setLayout(vLayout);

	// thumbnail
	QPixmap thumbnailPixmap( visualization->GetImageFilename() );
	mThumbnailPixmap = thumbnailPixmap.scaledToWidth(thumbnailWidth);

	mThumbnailPixmapDisabled = mThumbnailPixmap;
	mThumbnailPixmapDisabled.fill(Qt::transparent);

	QPainter painter(&mThumbnailPixmapDisabled);
	painter.setOpacity(0.4);
	painter.drawPixmap(0, 0, mThumbnailPixmap);
	painter.end();

	mThumbnailLabel = new QLabel(this);
	vLayout->addWidget( mThumbnailLabel );


	// name
	mNameLabel = new QLabel( visualization->GetName(), this );
	QFont font = mNameLabel->font();
	font.setPointSize(20);
	mNameLabel->setFont(font);
	vLayout->addWidget( mNameLabel );


	// description
	mDescriptionLabel = new QLabel( visualization->GetDescription(), this );
	mDescriptionLabel->setWordWrap(true);
	vLayout->addWidget( mDescriptionLabel );


	// select button
	mSelectButton = new QPushButton("Select", this);
	connect( mSelectButton, &QPushButton::clicked, this, &VisualizationSelectWidget::OnSelectVisualization );
	vLayout->addWidget( mSelectButton );

	UpdateInterface();
}


// destructor
VisualizationSelectWidget::~VisualizationSelectWidget()
{
}


// on mouse enter
void VisualizationSelectWidget::enterEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = true;
	emit HoverStateChanged();
}


// on mouse leave
void VisualizationSelectWidget::leaveEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = false;
	emit HoverStateChanged();
}


// double click event
void VisualizationSelectWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	OnSelectVisualization();
}


void VisualizationSelectWidget::UpdateInterface()
{
	if (IsHovered() == true)
		mThumbnailLabel->setPixmap( mThumbnailPixmap );
	else
		mThumbnailLabel->setPixmap( mThumbnailPixmapDisabled );

	update();
}


void VisualizationSelectWidget::OnSelectVisualization()
{
	GetManager()->GetVisualizationManager()->Start(mVisualization);
	emit VisualizationSelected();
}


void VisualizationSelectWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	if (IsHovered() == true)
	{
		QColor bgColor = QColor(87,87,86);
		painter.setPen( bgColor );
		painter.setBrush( bgColor );

		painter.drawRect( rect() );

		mSelectButton->setEnabled(true);
	}
	else
	{
		QStyleOption opt;
		opt.init(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

		mSelectButton->setEnabled(false);
	}
}
