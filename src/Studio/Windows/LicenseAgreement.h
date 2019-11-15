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

#ifndef __NEUROMORE_LICENSEAGREEMENT_H
#define __NEUROMORE_LICENSEAGREEMENT_H

// include required headers
#include "../Config.h"
#include <Core/String.h>
#include <Backend/FileDownloader.h>
#include <QUrl>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>


class LicenseAgreement : public QObject
{
	Q_OBJECT
	public:
		LicenseAgreement(QUrl licenseUrl, QWidget* parent);
		virtual ~LicenseAgreement();

		const char* GetText() const				{ return mText.AsChar(); }
		bool IsLoaded() const					{ return mLoaded; }

	signals:
		void TextChanged();

	private slots:
		void OnFileDownloaded(FileDownloader* downloader);
		void OnFileDownloadFailed(FileDownloader* downloader);
		void OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

	private:
		Core::String	mText;
		FileDownloader*	mFileDownloader;
		bool			mLoaded;
};


#endif
