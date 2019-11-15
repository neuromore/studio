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

// include required files
#include "Json.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include "LogManager.h"
#include <iostream>
#include <fstream>
#include <sstream>


namespace Core
{

// constructor
Json::Item::Item(rapidjson::Document& document, rapidjson::Value::MemberIterator memberIterator, bool isNull) : mDocument(document), mValue(memberIterator->value), mName(memberIterator->name), mIsNull(isNull)
{
}


// constructor
Json::Item::Item(rapidjson::Document& document, rapidjson::Value& name, rapidjson::Value& value, bool isNull) : mDocument(document), mValue(value), mName(name), mIsNull(isNull)
{
}


// constructor
Json::Item::Item(rapidjson::Document& document, rapidjson::Value& value, bool isNull) : mDocument(document), mValue(value), mName(value), mIsNull(isNull)
{
}


// constructor
Json::Item::Item(rapidjson::Document& document, bool isNull) : mDocument(document), mValue(document), mName(document), mIsNull(isNull)
{
}


// const constructor
Json::Item::Item(const rapidjson::Document& document, bool isNull) : mDocument(const_cast<rapidjson::Document&>(document)), mValue(const_cast<rapidjson::Document&>(document)), mName(const_cast<rapidjson::Document&>(document)), mIsNull(isNull)
{
}



// get the name
const char* Json::Item::GetName() const
{
	if (mName.IsNull() == true || mName.IsString() == false)
		return "";

	return mName.GetString();
}


// is null?
bool Json::Item::IsNull() const
{
	return (mIsNull || mValue.IsNull());
}


// is object?
bool Json::Item::IsObject() const
{
	return (IsNull() == false && mValue.IsObject() == true);
}


// is array?
bool Json::Item::IsArray() const
{
	return (IsNull() == false && mValue.IsArray() == true);
}


// is string?
bool Json::Item::IsString() const
{
	return (IsNull() == false && mValue.IsString() == true);
}


// is number?
bool Json::Item::IsNumber() const
{
	return (IsNull() == false && mValue.IsNumber() == true);
}


// is integer?
bool Json::Item::IsInt() const
{
	return (IsNull() == false && mValue.IsInt() == true);
}


// is double?
bool Json::Item::IsDouble() const
{
	return (IsNull() == false && mValue.IsDouble() == true);
}


// is boolean?
bool Json::Item::IsBool() const
{
	return (IsNull() == false && mValue.IsBool() == true);
}


// get integer
int32 Json::Item::GetInt() const
{
	return mValue.GetInt();
}


// get double
double Json::Item::GetDouble() const
{
	return mValue.GetDouble();
}


// get bool
bool Json::Item::GetBool() const
{
	return mValue.GetBool();
}


// get string
const char* Json::Item::GetString() const
{
	return mValue.GetString();
}


// set integer
void Json::Item::SetInt(int32 value)			
{
	mValue.SetInt(value);
}


// set double
void Json::Item::SetDouble(double value)		
{
	mValue.SetDouble(value);
}


// set boolean
void Json::Item::SetBool(bool value)			
{
	mValue.SetBool(value);
}


// set string
void Json::Item::SetString(const char* value)	
{
	mValue.SetString( value, mDocument.GetAllocator() );
}


// find member
Json::Item Json::Item::Find(const char* name) const
{
	rapidjson::Value::MemberIterator memberIterator = mValue.FindMember(name);
	if (memberIterator == mValue.MemberEnd())
		return Item(mDocument, memberIterator, true);

	return Item(mDocument, memberIterator, false);
}


// add object as member
Json::Item Json::Item::AddObject(const char* name)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kObjectType );

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );

	// TODO: hm why this? returning an item linking to name and value item didn't work!
	return Find(name);
}


// add array as member
Json::Item Json::Item::AddArray(const char* name)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kArrayType );

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );

	// TODO: hm why this? returning an item linking to name and value item didn't work!
	return Find(name);
}


// add boolean as member
void Json::Item::AddBool(const char* name, bool value)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kNumberType );

	valueItem.SetBool(value);

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );
}


// add integer as member
void Json::Item::AddInt(const char* name, int32 value)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kNumberType );

	valueItem.SetInt(value);

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );
}


// add double as member
void Json::Item::AddDouble(const char* name, double value)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kNumberType );
	
	valueItem.SetDouble(value);

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );
}


// add string as member
void Json::Item::AddString(const char* name, const char* value)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( rapidjson::kStringType );

	valueItem.SetString( value, mDocument.GetAllocator() );

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );
}


// add another json subtree (creates deep copy)
void Json::Item::AddJson(const char* name, const Json& value)
{
	Json::Item root = const_cast<Json&>(value).GetRootItem(); // TODO get rid of const cast! (but _dont_ make value non-const!! this is the wrong direction)
	AddJson(name, root);
}


// TODO test me
// add another json subtree by creating a deep copy of all elements
void Json::Item::AddJson(const char* name, const Json::Item& value)
{
	rapidjson::Value nameItem( name, mDocument.GetAllocator() );
	rapidjson::Value valueItem( value.mValue, mDocument.GetAllocator() );

	mValue.AddMember( nameItem, valueItem, mDocument.GetAllocator() );
}


// array operation: add object
Json::Item Json::Item::AddObject()
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::AddObject(): Adding unnamed object only allowed for array type.");
		CORE_ASSERT(false);
		return Item(mDocument, true);
	}

	rapidjson::Value valueItem( rapidjson::kObjectType );
	mValue.PushBack( valueItem, mDocument.GetAllocator() );
	
	const int32 index = mValue.Size()-1;
	CORE_ASSERT( index >= 0 );
	return operator[](index);
}


// array operation: add boolean
void Json::Item::AddBool(bool value)
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::AddBool(): Adding unnamed boolean only allowed for array type.");
		CORE_ASSERT(false);
		return;
	}

	rapidjson::Value valueItem( rapidjson::kNumberType );
	valueItem.SetBool(value);

	mValue.PushBack( valueItem, mDocument.GetAllocator() );
}


// array operation: add integer
void Json::Item::AddInt(int32 value)
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::AddInt(): Adding unnamed integer only allowed for array type.");
		CORE_ASSERT(false);
		return;
	}

	rapidjson::Value valueItem( rapidjson::kNumberType );
	valueItem.SetInt(value);

	mValue.PushBack( valueItem, mDocument.GetAllocator() );
}


// array operation: add double
void Json::Item::AddDouble(double value)
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::AddDouble(): Adding unnamed double only allowed for array type.");
		CORE_ASSERT(false);
		return;
	}

	rapidjson::Value valueItem( rapidjson::kNumberType );
	valueItem.SetDouble(value);

	mValue.PushBack( valueItem, mDocument.GetAllocator() );
}


// array operation: add string
void Json::Item::AddString(const char* value)
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::AddString(): Adding unnamed string only allowed for array type.");
		CORE_ASSERT(false);
		return;
	}

	rapidjson::Value valueItem( rapidjson::kStringType );
	valueItem.SetString( value, mDocument.GetAllocator() );

	mValue.PushBack( valueItem, mDocument.GetAllocator() );
}


// array operation: get size
uint32 Json::Item::Size() const
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::Size(): Cannot get size. Only allowed for array type.");
		CORE_ASSERT(false);
		return 0;
	}

	return mValue.Size();
}


// array accessor
Json::Item Json::Item::operator[](uint32 index) const
{
	// array operation only
	if (IsArray() == false)
	{
		LogWarning("Json::Item::operator[]: Cannot get item. Only allowed for array type.");
		CORE_ASSERT(false);
		return Item(mDocument, true);
	}

	return Item(mDocument, mValue[index], false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
Json::Json()
{
	// define the document as an object
	mDocument.SetObject();
}


// copy constructor
Json::Json(const Json& other)
{
	// define the document as an object
	mDocument.CopyFrom( other.mDocument, mDocument.GetAllocator() );
}


// subtree copy constructor
Json::Json(const Json::Item& object)
{
	CORE_ASSERT(object.IsObject() == true);

	// define the document as an object
	mDocument.SetObject();

	// TODO find better way to copy over a subtree of the DOM
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	mDocument.Accept(writer);
}


// destructor
Json::~Json()
{
}


// clear
void Json::Clear()
{
	// define the document as an object
	mDocument.SetObject();
}


// find member
Json::Item Json::Find(const char* name)
{
	rapidjson::Value::MemberIterator memberIterator = mDocument.FindMember(name);
	if (memberIterator == mDocument.MemberEnd())
		return Item(mDocument, memberIterator, true);

	return Item(mDocument, memberIterator, false);
}


// get root item
Json::Item Json::GetRootItem()
{
	return Item(mDocument, false);
}


// get root item
const Json::Item Json::GetRootItem() const
{
	return Item(mDocument, false);
}



// log
void Json::Log(bool pretty) const
{
	String jsonText;
	WriteToString( jsonText, pretty );

	if (jsonText.GetLength() >= 4*4096)
	{
		jsonText.Resize(4*4095);
		LogDebug(jsonText.AsChar());
		LogDebug("WARNING: JSON log message truncated due to big size! The remaining part won't be logged.");
	}
	else
	{
		const uint32 length = jsonText.GetLength();
		jsonText.RemoveChars("%");
		LogDebug(jsonText.AsChar());
		//OutputDebugStringA( jsonText.AsChar() );
	}
}


// parse
bool Json::Parse(const String& input)
{
	return Parse(input.AsChar());
}


// parse
bool Json::Parse(const char* input)
{
	// "normal" parsing, decode strings to new buffers
	if (mDocument.Parse(input).HasParseError())
		return false;

	return true;
}


// parse from file
bool Json::ParseFile(const char* filename)
{
	// create the file on disk
	FILE* file;
	file = fopen(filename, "rb\0");
	if (file == NULL)
	{
		LogError( "Json::ParseFromFile(): Cannot open file '%s'.", filename );
		return false;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// load the whole file into a string
	char* stringData = new char[fileSize+1];

	if (fread(stringData, fileSize, 1, file) < 0)
	{
		LogError( "Json::ParseFromFile(): Cannot read file '%s'.", filename );
		delete[] stringData;
		return false;
	}
	stringData[fileSize] = '\0';

	fclose(file);

	// load json from string data
	bool result = Parse( stringData );
	delete[] stringData;
	return result;
}


// serialize to string
void Json::WriteToString(Core::String& outString, bool pretty) const
{
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	if (pretty == true)
	{
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		mDocument.Accept(writer);
	}
	else
	{
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		mDocument.Accept(writer);
	}

	outString = buffer.GetString();
}


// serialize to disk
bool Json::WriteToFile(const char* filename, bool pretty) const
{
	// create the file on disk
	FILE* file;
	file = fopen(filename, "wt\0");
	if (file == NULL)
	{
		LogError("Core::Json::WriteToFile() - Cannot open or create file '%s'.", filename);
		return false;
	}

	// build the string and write it
	String result;
	result.Reserve( 10*4096 );
	WriteToString( result, pretty );

	if (fputs(result.AsChar(), file) < 0)
	{
		LogError("Core::Json::WriteToFile() - Cannot write to file file '%s'...", filename);
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}


// copy assignment operator
Json& Json::operator=(const Json& other)
{
	mDocument.CopyFrom( other.mDocument, mDocument.GetAllocator() );
	return *this;
}


//
//// move assignment operator
//Json& Json::operator=(Json&& other)
//{
// if (this != &other)
// {
//	// TODO how :D?
// }
//	return *this;
//}



}; // namespace Core
