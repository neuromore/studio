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
#include "LicenseAgreement.h"

using namespace Core;

// constructor
LicenseAgreement::LicenseAgreement(QUrl licenseUrl, QWidget* parent) : QObject(parent)
{
	mLoaded = false;
	mFileDownloader = new FileDownloader( licenseUrl, this );

	connect( mFileDownloader, SIGNAL(FinishedDownload(FileDownloader*)), this, SLOT(OnFileDownloaded(FileDownloader*)) );
	connect( mFileDownloader, SIGNAL(DownloadFailed(FileDownloader*)), this, SLOT(OnFileDownloadFailed(FileDownloader*)) );
	connect( mFileDownloader, SIGNAL(DownloadProgressChanged(qint64, qint64)), this, SLOT(OnDownloadProgressChanged(qint64, qint64)) );

	mFileDownloader->Start();
}


// destructor
LicenseAgreement::~LicenseAgreement()
{
}


// called when a download status changed
void LicenseAgreement::OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
	String text;
	text.Format( "Downloading license (%i/%i)", bytesReceived, bytesTotal );
	
	mText = text;
	emit TextChanged();
}


void LicenseAgreement::OnFileDownloaded(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogDebug("LicenseAgreementWidget: Finished download '%s'", url.AsChar() );

	const QByteArray& data = downloader->GetFileBuffer();
	mLoaded = true;
	
	mText = data.data();
	emit TextChanged();
}


void LicenseAgreement::OnFileDownloadFailed(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogWarning("LicenseAgreementWidget: Failed downloading '%s'", url.AsChar() );

	String text;
	text.Format( "Downloading license from '%' failed ...", url.AsChar() );
	mLoaded = false;
	
	mText = text;
	emit TextChanged();
}
