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

#ifndef __NEUROMORE_JSON_H
#define __NEUROMORE_JSON_H

// include required headers
#include "Config.h"
#include "StandardHeaders.h"
#include "String.h"
#include <rapidjson/document.h>

namespace Core
{

// the JSON class
class ENGINE_API Json
{
	public:
		class Item
		{
			public:
				// name
				const char* GetName() const;

				// type check
				bool IsNull() const;
				bool IsObject() const;
				bool IsArray() const;
				bool IsString() const;
				bool IsNumber() const;
				bool IsInt() const;
				bool IsDouble() const;
				bool IsBool() const;

				// accessors
				int32 GetInt() const;
				double GetDouble() const;
				bool GetBool() const;
				const char* GetString() const;

				// mutators
				void SetInt(int32 value);
				void SetDouble(double value);
				void SetBool(bool value);
				void SetString(const char* value);

				// member operations
				Item Find(const char* name) const;
				Item AddObject(const char* name);
				Item AddArray(const char* name);
				void AddBool(const char* name, bool value);
				void AddInt(const char* name, int32 value);
				void AddDouble(const char* name, double value);
				void AddString(const char* name, const char* value);
				void AddJson(const char* name, const Json& value);			// Try turning the value into a const ref and see what happens :D
				void AddJson(const char* name, const Json::Item& value);		// Try turning the value into a const ref and see what happens :D

				// array operations (precondition: IsArray() == true)
				uint32 Size() const;
				Item operator[](uint32 index) const;
				Item AddObject();
				void AddBool(bool value);
				void AddInt(int32 value);
				void AddDouble(double value);
				void AddString(const char* value);

			private:
				friend class Json;
				Item(rapidjson::Document& document, rapidjson::Value::MemberIterator memberIterator, bool isNull);
				Item(rapidjson::Document& document, rapidjson::Value& value, bool isNull);
				Item(rapidjson::Document& document, rapidjson::Value& name, rapidjson::Value& value, bool isNull);
				Item(rapidjson::Document& document, bool isNull);

				Item(const rapidjson::Document& document, bool isNull);

				rapidjson::Document&	mDocument;
				rapidjson::Value&		mValue;
				rapidjson::Value&		mName;
				bool					mIsNull;
		};

		// constructor
		Json();

		// copy and move constructor
		Json(const Json& other);
		//Json (Json&& other);	// TODO move constructor

		// construct new json from another json subtree (item has to be an object)
		Json(const Json::Item& object);

		// destructor
		~Json();

		// clear document
		void Clear();

		// find item
		Item Find(const char* name);

		// get the root item
		Item GetRootItem();
		const Item GetRootItem() const;

		void Log(bool pretty=true) const;

		// TODO: make this const ("Item NullItem() const {...}" ) and be amazed!
		Item NullItem()															{ return Item(mDocument, true); }

		// parse and keep the json model in memory
		bool Parse(const Core::String& input);
		bool Parse(const char* input);
		bool ParseFile(const char* filename);

		// serialize json to string
		void WriteToString(Core::String& outString, bool pretty=true) const;
		bool WriteToFile(const char* filename, bool pretty=true) const;
		String ToString(bool pretty = true) const								{ String str; WriteToString(str, pretty); return str; }

		// copy assignment operator
		Json& operator=(const Json& other);

		// TODO move assignment operator // need to know how 
		//Json& operator=(Json&& other);

	private:
		rapidjson::Document mDocument;
};

}; // namespace Core


#endif
