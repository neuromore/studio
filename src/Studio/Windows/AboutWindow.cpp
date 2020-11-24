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
#include "AboutWindow.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <QLabel>
#include <QSizePolicy>
#include <QPixmap>
#include <QTextEdit>
#include <QVBoxLayout>

#ifdef NEUROMORE_BRANDING_ANT
#define ABOUTIMAGE ":/Images/About-ANT.png"
#else
#define ABOUTIMAGE ":/Images/About-neuromore.png"
#endif

using namespace Core;

// constructor
AboutWindow::AboutWindow(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	// update title of the about dialog
	Core::String windowTitle = "About ";
	windowTitle += GetManager()->GetAppName();
	setWindowTitle( windowTitle.AsChar() );

	// window icon
	setWindowIcon( GetQtBaseManager()->FindIcon("Images/Icons/Info.png") );

	// create the about dialog's layout
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->setMargin(0);
	layout->setSpacing(3);

	// load the about dialog image and add it to the dialog
	QPixmap aboutImage(ABOUTIMAGE);
	QLabel* aboutImageLabel = new QLabel(this);
	aboutImageLabel->setPixmap(aboutImage);
	layout->addWidget(aboutImageLabel);

	// separator label
	layout->addWidget(new QLabel(""));

	// made with love
	//QString madeWithLoveString = QString("Engineered with ") + QChar(0x2764) + QString(" by neuromore");
	//QString madeWithLoveString = QString("engineered by neuromore");
	//QLabel* madeWithLoveLabel = new QLabel(madeWithLoveString);
	//madeWithLoveLabel->setStyleSheet( "color: rgb(255,0,0); font: 16px; font-weight: bold;" );
	//layout->addWidget( madeWithLoveLabel, 0, Qt::AlignHCenter );

	// show studio version
	layout->addWidget( new QLabel( String().Format("Version: v%s", GetManager()->GetVersion().AsString().AsChar()).AsChar(), this), 0, Qt::AlignHCenter );

	// show compilation date
	layout->addWidget( new QLabel( String().Format("Compilation Date: %s", CORE_DATE).AsChar(), this), 0, Qt::AlignHCenter );

	// show engine version
	layout->addWidget( new QLabel( String().Format("neuromore Engine: v%s", GetEngine()->GetVersion().AsString().AsChar()).AsChar(), this), 0, Qt::AlignHCenter );

	// show Qt version
	//String qtVersionString;
	//qtVersionString.Format("Qt v%s", QT_VERSION_STR);
	//layout->addWidget(new QLabel(qtVersionString.AsChar()), 0, Qt::AlignHCenter);

	// separator label
	layout->addWidget(new QLabel(""));

	// automatically read out the year used in the copyright information from the compilation date
	Array<String> splitStrings = String(CORE_DATE).Split( StringCharacter(' ') );
	if (splitStrings.Size() > 0)
		layout->addWidget(new QLabel(String().Format("Copyright(c) %s %s", splitStrings[splitStrings.Size() - 1].AsChar(), GetManager()->GetDeveloperName()).AsChar()), 0, Qt::AlignHCenter);
		//layout->addWidget( new QLabel( String().Format("Copyright(c) 2012-%s %s", splitStrings[splitStrings.Size()-1].AsChar(), GetManager()->GetDeveloperName()).AsChar()), 0, Qt::AlignHCenter );
	//layout->addWidget( new QLabel( String().Format("Copyright(c) %s", GetManager()->GetDeveloperName()).AsChar()), 0, Qt::AlignHCenter );
	layout->addWidget( new QLabel("All Rights Reserved."), 0, Qt::AlignHCenter );

	// separator label
	layout->addWidget(new QLabel(""));

	// credits text field
	mTextEdit = new QTextEdit();
	mTextEdit->setMinimumHeight( 350 );
	mTextEdit->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
	layout->addWidget(mTextEdit);
	SetText("Loading ...");

	// load credits
	mFileDownloader = new FileDownloader( QUrl("http://neuromore-installer.s3.amazonaws.com/legal/credits/studio.txt"), this );
	connect( mFileDownloader, SIGNAL(FinishedDownload(FileDownloader*)), this, SLOT(OnFileDownloaded(FileDownloader*)) );
	connect( mFileDownloader, SIGNAL(DownloadFailed(FileDownloader*)), this, SLOT(OnFileDownloadFailed(FileDownloader*)) );

	mFileDownloader->Start();
}


// destructor
AboutWindow::~AboutWindow()
{
}


void AboutWindow::SetText(const char* text)
{
	mTextEdit->clear();
	mTextEdit->append( text );
	
	// set the cursor to the beginning
	QTextCursor textCursor = mTextEdit->textCursor();
	textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	mTextEdit->setTextCursor( textCursor );

	mTextEdit->setReadOnly(true);
	mTextEdit->setStyleSheet("color: rgb(0,159,227);");
}


void AboutWindow::OnFileDownloaded(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogDebug("AboutWindow: Finished download '%s'", url.AsChar() );

	const QByteArray& data = downloader->GetFileBuffer();
	SetText( data.data() );
}


void AboutWindow::OnFileDownloadFailed(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogWarning("LicenseAgreementWidget: Failed downloading '%s'", url.AsChar() );
	SetText("Loading credits failed.");
}
