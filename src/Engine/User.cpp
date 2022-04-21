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
#include <Engine/Precompiled.h>

// include required files
#include "User.h"
#include "Core/LogManager.h"


using namespace Core;

// constructor
User::User()
{
	mAutoPowerLineFrequency	= 0.0;
}


// destructor
User::~User()
{
}


Core::String User::CreateDisplayableName() const
{
	String name = CreateFullName();

	// fallback to displayname
	if (name.IsEmpty() == true)
		name = mDisplayName;

	// fallback to something non-empty
	if (name.ContainsVisibleCharacter() == false)
		name = "Unnamed User";

	return name;
}


Core::String User::CreateFullName() const
{
	String result;

	if (mFirstName.IsEmpty() == false)
		result = mFirstName;

	if (mMiddleName.IsEmpty() == false)
		result += " " + mMiddleName;
	
	if (mLastName.IsEmpty() == false)
		result += " " + mLastName;
	
	// make sure we return an empty string and not just invisible characters (for whatever reason it happened)
	if (result.ContainsVisibleCharacter() == false)
		result = "";

	return result;
}


void User::AddRule(const Rule& rule)
{
	CORE_ASSERT( FindRule(rule.GetName()) == NULL );

	if (FindRule(rule.GetName()) != NULL)
		return;

	mRules.Add(rule);
}


void User::LogRules()
{
	// get the number of rules and iterate through them
	const uint32 numRules = mRules.Size();
	LogInfo(" - Rules (%i):", numRules);
	for (uint32 i=0; i<numRules; ++i)
	{
		const Rule& rule = mRules[i];
		LogDetailedInfo("    + Rule %i: name=%s, creud=%i%i%i%i%i", i, rule.GetName(), (int)rule.GetCreud().Create(), (int)rule.GetCreud().Read(), (int)rule.GetCreud().Execute(), (int)rule.GetCreud().Update(), (int)rule.GetCreud().Delete());
	}
}


void User::ReadRules(Json::Item item)
{
	// find the rules item
	Json::Item rulesItem = item.Find("rules");
	if (rulesItem.IsNull())
	{
		LogWarning( "Json does not contain a 'rules' item or the 'rules' item." );
		return;
	}

	// get the number of rules and iterate through them
	String ruleName;
	const uint32 numRules = rulesItem.Size();
	for (uint32 i=0; i<numRules; ++i)
	{
		// get the rule and its name
		Json::Item ruleItem	= rulesItem[i];
		Json::Item nameItem	= ruleItem.Find("name");
		
		// read creud
		Creud creud;
		creud.ReadFromJson(ruleItem, false);

		// read name
		if (nameItem.IsNull() == false && nameItem.IsString() == true)
			ruleName = nameItem.GetString();
		else
			ruleName.Clear();

		// create and add the new rule
		Rule rule( ruleName.AsChar(), creud );
		AddRule( rule );
	}
}


const User::Rule* User::FindRule(const char* ruleName) const
{
	// get the number of rules and iterate through them
	const uint32 numRules = mRules.Size();
	for (uint32 i=0; i<numRules; ++i)
	{
		if (mRules[i].GetNameString().IsEqual(ruleName) == true)
			return &mRules[i];
	}

	return NULL;
}
