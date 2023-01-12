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

#ifndef __NEUROMORE_USER_H
#define __NEUROMORE_USER_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Array.h"
#include "Core/Json.h"
#include "Creud.h"


class ENGINE_API User
{
	public:
		// constructor & destructor
		User();
		~User();

		class Rule
		{
			public:
				Rule(const char* name, const Creud& creud)
				{
					mName			= name;
					mCreud			= creud;
				}

				// name
				const char* GetName() const							{ return mName.AsChar(); }
				const Core::String& GetNameString() const			{ return mName; }

				// creud
				const Creud& GetCreud() const						{ return mCreud; }

			private:
				Core::String		mName;
				Creud				mCreud;
		};

		void SetId(const char* id)											{ mId = id; }
		inline const char* GetId() const									{ return mId.AsChar(); }
		inline const Core::String& GetIdString() const						{ return mId; }

		// first, middle and last name
		void SetFirstName(const char* firstName)							{ mFirstName = firstName; }
		void SetMiddleName(const char* middleName)							{ mMiddleName = middleName; }
		void SetLastName(const char* lastName)								{ mLastName = lastName; }

		const char* GetFirstName() const									{ return mFirstName.AsChar(); }
		const char* GetMiddleName() const									{ return mMiddleName.AsChar(); }
		const char* GetLastName() const										{ return mLastName.AsChar(); }
		const Core::String& GetFirstNameString() const						{ return mFirstName; }
		const Core::String& GetMiddleNameString() const						{ return mMiddleName; }
		const Core::String& GetLastNameString() const						{ return mLastName; }

		void SetEmail(const char* email)									{ mEmail = email; }
		const char* GetEmail() const										{ return mEmail.AsChar(); }
		const Core::String& GetEmailString() const							{ return mEmail; }

		void SetBirthday(const char* birthday)								{ mBirthday = birthday; }
		const char* GetBirthday() const										{ return mBirthday.AsChar(); }

		Core::String CreateDisplayableName() const;
		Core::String CreateFullName() const;

		void SetToken(const char* id)										{ mToken = id; }
		inline const char* GetToken() const									{ return mToken.AsChar(); }
		inline const Core::String& GetTokenString() const					{ return mToken; }

		void SetDisplayName(const char* name)								{ mDisplayName = name; }
		inline const char* GetDisplayName() const							{ return mDisplayName.AsChar(); }
		inline const Core::String& GetDisplayNameString() const				{ return mDisplayName; }

		// rules management
		void AddRule(const Rule& rule);
		uint32 GetNumRules() const											{ return mRules.Size(); }
		const Rule& GetRule(uint32 index) const								{ return mRules[index]; }
		
		void ReadRules(Core::Json::Item item);
		void LogRules();

		// rule permissions
		const Rule* FindRule(const char* ruleName) const;
		bool CreateAllowed(const char* ruleName)  const { if (const Rule* r = FindRule(ruleName)) return r->GetCreud().Create(); else return false; }
		bool ReadAllowed(const char* ruleName)    const { if (const Rule* r = FindRule(ruleName)) return r->GetCreud().Read(); else return false; }
		bool ExecuteAllowed(const char* ruleName) const { if (const Rule* r = FindRule(ruleName)) return r->GetCreud().Execute(); else return false; }
		bool UpdateAllowed(const char* ruleName)  const { if (const Rule* r = FindRule(ruleName)) return r->GetCreud().Update(); else return false; }
		bool DeleteAllowed(const char* ruleName)  const { if (const Rule* r = FindRule(ruleName)) return r->GetCreud().Delete(); else return false; }

		// parent company ids
		void AddParentCompanyId(const char* userId)							{ mParentCompanyIds.Add(userId); }
		uint32 GetNumParentCompanyIds() const								{ return mParentCompanyIds.Size(); }
		const char* GetParentCompanyId(uint32 index) const					{ return mParentCompanyIds[index]; }
		void ClearParentCompanyIds()										{ mParentCompanyIds.Clear(); }
		const Core::Array<Core::String>& GetParentCompanyIds() const { return mParentCompanyIds; }

		void SetAutoPowerLineFrequency(double frequency)					{ mAutoPowerLineFrequency = frequency; }
		double GetAutoPowerLineFrequency() const							{ return mAutoPowerLineFrequency; }

	private:
		Core::String				mFirstName;
		Core::String				mMiddleName;
		Core::String				mLastName;
		Core::String				mEmail;
		Core::String				mBirthday;
		Core::Array<Rule>			mRules;
		Core::Array<Core::String>	mParentCompanyIds;
		Core::String				mDisplayName;
		Core::String				mId;
		Core::String				mToken;
		double						mAutoPowerLineFrequency;
};


#endif
