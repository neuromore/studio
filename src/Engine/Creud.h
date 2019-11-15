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

#ifndef __NEUROMORE_CREUD_H
#define __NEUROMORE_CREUD_H

// include required headers
#include "Config.h"
#include "Core/Json.h"


class ENGINE_API Creud
{
	public:
		Creud();
		Creud(bool create, bool read, bool update, bool del, bool execute);

		bool Create() const;
		bool Read() const;
		bool Execute() const;
		bool Update() const;
		bool Delete() const;

		void RevokeCreate();
		void RevokeRead();
		void RevokeExecute();
		void RevokeUpdate();
		void RevokeDelete();

		void ReadFromJson(Core::Json::Item item, bool includeExecute=true);

		void Log() const;
		Core::String ToString() const;

		// put the creud booleans into an integer and also provide functionality to extract it again
		int ConvertToInt();
		void ConvertFromInt(int creud);

		bool operator==(const Creud& creud) const;

	private:
		bool mCreate;
		bool mRead;
		bool mExecute;
		bool mUpdate;
		bool mDelete;
};


#endif
