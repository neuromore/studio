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

// include required headers
#include "EmailValidator.h"


using namespace Core;

// constructor
EmailValidator::EmailValidator(bool allowCommaSeparatedMultipleEmails, QObject* parent) : QValidator(parent),
      mValidMailRegExp("^([a-zA-Z0-9_\\+\\-\\.]+)@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.)|(([a-zA-Z0-9\\-]+\\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\\]?)$"),
      mIntermediateMailRegExp("[a-z0-9._%+-]*@?[a-z0-9.-]*\\.?[a-z]*")
{
	mAllowCommaSeparatedMultipleEmails = allowCommaSeparatedMultipleEmails;
}


QValidator::State EmailValidator::validate(QString& text, int& pos) const
{
    Q_UNUSED(pos)

	if (mAllowCommaSeparatedMultipleEmails == false)
	{
		fixup(text);

		if (mValidMailRegExp.exactMatch(text))
			return Acceptable;
		if (mIntermediateMailRegExp.exactMatch(text))
			return Intermediate;

		return Invalid;
	}
	else
	{
		QStringList emails = GetEmailStringList(text);
		if (emails.size() == 0)
			return Invalid;

		bool intermediate	= true;
		bool acceptable		= true;

		// check all emails
		QString email;
		const int numEmails = emails.size();
		for (int i=0; i<numEmails; ++i)
		{
			email = emails[i];

			if (mValidMailRegExp.exactMatch(email) == false)
				acceptable = false;

			if (mIntermediateMailRegExp.exactMatch(email) == false)
				intermediate = false;
		}

		if (acceptable == true)
			return Acceptable;
		if (intermediate == true)
			return Intermediate;

		return Invalid;
	}
}


void EmailValidator::fixup(QString &text) const
{
    text = text.trimmed().toLower();
}


QStringList EmailValidator::GetEmailStringList(QString& text) const
{
	QStringList result;

	// split up the string into separated email addresses
	QStringList emails = text.split(';');

	// fixup all emails
	const int numEmails = emails.size();
	for (int i=0; i<numEmails; ++i)
		result.append( emails[i].trimmed().toLower() );

	return result;
}


Array<String> EmailValidator::GetEmails(QString& text) const
{
	Array<String> result;
	QStringList emails = GetEmailStringList(text);

	const int numEmails = emails.size();
	for (int i=0; i<numEmails; ++i)
		result.Add( emails[i].toUtf8().data() );

	return result;
}
