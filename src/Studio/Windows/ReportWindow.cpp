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
#include "ReportWindow.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <QHBoxLayout>
#include <QDesktopServices>


using namespace Core;

// constructor
ReportWindow::ReportWindow(const Core::String& dataChunkId, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	// set the window title
	setWindowTitle( "Report" );

	setModal(true);
	setWindowModality( Qt::ApplicationModal );

	// add the main layout
	QHBoxLayout* mainLayout = new QHBoxLayout();
	setLayout(mainLayout);

	// create the back to selection button
	QPushButton* backButton = new QPushButton("Back to selection");
	mainLayout->addWidget(backButton);
	connect( backButton, &QPushButton::clicked, this, [=]
	{
		// switch back to experience selection
		if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
			GetLayoutManager()->SwitchToLayoutByName("Experience Selection");

		// close window
		accept();
	});

	// create show report button
	mShowReportButton = new QPushButton("");
	mShowReportButton->setEnabled(false);
	mShowReportButton->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Report.png"));
	mainLayout->addWidget(mShowReportButton);
	connect( mShowReportButton, &QPushButton::clicked, this, [dataChunkId]
	{
		ShowReport( dataChunkId );
	});

	// interface update timer
	mTimer = new QTimer(this);
	connect( mTimer, &QTimer::timeout, this, &ReportWindow::UpdateInterface);
	mTimer->setInterval(100);
	mTimer->start();

	// avoid resizing
	setSizeGripEnabled(false);
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	setFixedSize( QSize(500, sizeHint().height()) );

	// position window in the screen center
	GetQtBaseManager()->CenterToScreen(this);

	show();
}


// destructor
ReportWindow::~ReportWindow()
{
}


// show report
void ReportWindow::ShowReport(const Core::String& dataChunkId)
{
	if (dataChunkId.IsEmpty() == true)
		return;

	String finalUrl = GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPreset().mWebserverUrl;
	finalUrl += "/#/report?dataChunkId=";
	finalUrl += dataChunkId;
	finalUrl += "&token=";
	finalUrl += GetUser()->GetToken();
	finalUrl += "&clamping=false";

	// start-up the online statistics in the default browser
	QDesktopServices::openUrl( QUrl(finalUrl.AsChar()) );
}


// update interface
void ReportWindow::UpdateInterface()
{
	BackendUploader* uploader = GetBackendInterface()->GetUploader();

	if (uploader->IsEnabled() == false)
	{
		mShowReportButton->setEnabled(false);
		mShowReportButton->setText("Report not available");
	}
	else
	{
		if (uploader->IsBusy() == true)
		{
			mShowReportButton->setEnabled(false);
			mShowReportButton->setText("Generating report, please wait ...");
		}
		else
		{
			mShowReportButton->setEnabled(true);
			mShowReportButton->setText("Show Report");
		}
	}
}
