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

// include the required headers
#include "DataVisualizationSettingsWidget.h"
#include "DataVisualizationPlugin.h"
#include <QLabel>

#ifdef USE_QTDATAVISUALIZATION

using namespace Core;

// constructor
DataVisualizationSettingsWidget::DataVisualizationSettingsWidget(QWidget* parent, DataVisualizationPlugin* plugin) : QWidget(parent)
{
	mPlugin				= plugin;
	mRow				= 0;
	mGridLayout			= NULL;

	mVLayout = new QVBoxLayout();
	mVLayout->setMargin(0);
	setLayout( mVLayout );

	// add the settings
	plugin->AddSettings(this);
}


// destructor
DataVisualizationSettingsWidget::~DataVisualizationSettingsWidget()
{
}


void DataVisualizationSettingsWidget::AddAttributeSetGridWidget(QWidget* widget)
{
	mVLayout->addWidget(widget);

	mGridLayout = new QGridLayout();
	mGridLayout->setMargin(0);
	mVLayout->addLayout(mGridLayout);
}


void DataVisualizationSettingsWidget::AddWidget(const char* name, QWidget* widget)
{
	mGridLayout->addWidget( new QLabel(name), mRow, 0  );
	mGridLayout->addWidget( widget, mRow, 1 );
	mRow++;
}


void DataVisualizationSettingsWidget::AddLayout(const char* name, QLayout* layout)
{
	mGridLayout->addWidget( new QLabel(name), mRow, 0  );
	mGridLayout->addLayout( layout, mRow, 1 );
	mRow++;
}

#endif
