/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILEMANAGER_H
#define __NEUROMORE_FILEMANAGER_H

// include required headers
#include "QtBaseConfig.h"
#include <QObject>
#include "Core/Json.h"
#include "Experience.h"
#include <Graph/GraphImporter.h>

// file manager for managing ressources (backend/local or http)
class QTBASE_API FileManager : public QObject
{
	Q_OBJECT

	public:
		FileManager();
		virtual ~FileManager();

		void Init();

		// type of file location
		enum ELocation
		{
			LOCATION_LOCAL, 
			LOCATION_BACKEND, 
			LOCATION_HTTP
		};


		// represents an opened file
		class OpenFile
		{
			friend class FileManager;

			public:
				OpenFile(ELocation location, const char* identifier, const char* name, int revision = -1)									   
					: mIdentifier(identifier), mLocation(location), mIsDirty(false), mRevision(revision), mSize(0) { mName = name; }
                virtual ~OpenFile()                             {}
				
				virtual uint32 GetType() const = 0;

				const Core::String& GetIdentifier() const		{ return mIdentifier; }
				const Core::String& GetName() const				{ return mName; }
				ELocation GetLocation() const					{ return mLocation; }
				
				int GetRevision() const							{ return mRevision; }
				bool HasRevision() const						{ return mRevision >= 0; }

				// access raw data
				const char* GetData() const						{ return mData.GetPtr(); }
				uint32		GetSize() const						{ return mData.Size(); }

				const Creud& GetCreud() const					{ return mCreud; }

				virtual bool IsDirty() const = 0;

			protected:
				virtual bool Serialize() = 0;
				virtual bool Deserialize() const = 0;
				virtual void SetUndirty() = 0;

				Core::String	mIdentifier;
				ELocation		mLocation;
				Core::String	mName;
				
				bool			mIsDirty;
				Creud			mCreud;
				int				mRevision;			// use CORE_INVALIDINDEX32 to denote that there is no revision
	
				// the raw data
				Core::Array<char>	mData;
				uint32				mSize;
		};


		class GraphOpenFile : public OpenFile
		{
			friend class FileManager;
			public:
				GraphOpenFile(Graph* graph, ELocation location, const char* identifier, const char* name, uint32 revision = CORE_INVALIDINDEX32) 
					: OpenFile(location, identifier, name, revision), mGraph(graph) {}
				enum { TYPE_ID = 0x0001 };
				uint32 GetType() const override		 { return TYPE_ID; }

			protected:
				bool Serialize() override
				{
					Core::Json json;
					Core::Json::Item rootItem = json.GetRootItem();

					// fill the json parser
					mGraph->Save( json, rootItem );

					json.WriteToString(mJsonContent);

					mData.Resize( mJsonContent.GetLength() + 1 ); // Null  byte

					Core::MemCopy( mData.GetPtr(), mJsonContent.AsChar(), mData.Size() );

					return true;
				}
				
				bool Deserialize() const override
				{
					const bool success = GraphImporter::LoadFromString(mData.GetPtr(), mGraph);

					mGraph->SetCreud(mCreud);
					mGraph->SetRevision(mRevision);
					mGraph->SetUuid( mIdentifier.AsChar() );
					mGraph->SetName( mName.AsChar() );

					mGraph->SetIsDirty(false);

					return success;
				}
				
				bool IsDirty() const override
				{
					return mGraph->IsDirty() && mGraph->GetUseSettings() == false;
				}
				
				void SetUndirty() override
				{
					mGraph->SetIsDirty(false);
				}

				Graph*			mGraph;
				Core::String	mJsonContent;
		};


		class ExperienceOpenFile : public OpenFile
		{
			friend class FileManager;
			public:
				ExperienceOpenFile(Experience* experience, ELocation location, const char* identifier, const char* name, uint32 revision = CORE_INVALIDINDEX32) 
					: OpenFile(location, identifier, name, revision), mExperience(experience) {}
				
				enum { TYPE_ID = 0x0002 };
				uint32 GetType() const override		{ return TYPE_ID; }

			protected:
				bool Serialize() override
				{
					Core::Json json;
					Core::Json::Item rootItem = json.GetRootItem();

					// fill the json parser
					mExperience->Save( json, rootItem );

					json.WriteToString(mJsonContent);

					mData.Resize(mJsonContent.GetLength() + 1); // null byte
					Core::MemCopy( mData.GetPtr(), mJsonContent.AsChar(), mData.Size() );

					return true;
				}
				
				bool Deserialize() const override
				{
					Core::Json json;
					if (json.Parse(mData.GetPtr()) == false)
						return false;

					if (mExperience->Load(json, json.GetRootItem()) == false)
						return false;

					mExperience->SetCreud(mCreud);
					mExperience->SetRevision(mRevision);

					mExperience->SetUuid( mIdentifier.AsChar() );
					mExperience->SetName( mName.AsChar() );

					return true;
				}
				
				bool IsDirty() const override
				{
					return mExperience->IsDirty();
				}
				
				void SetUndirty() override
				{
					mExperience->SetIsDirty(false);
				}

				Experience*		mExperience;
				Core::String	mJsonContent;
		};


		// open/save/close actions
		void OpenClassifier(ELocation location, const char* identifier, const char* name, int revision = -1); 
		void OpenStateMachine(ELocation location, const char* identifier, const char* name, int revision = -1); 
		void OpenGraph(Graph* graph, ELocation location, const char* identifier, const char* name, int revision = -1); 
		void OpenExperience(ELocation location, const char* identifier, const char* name, int revision = -1, bool loadReferencedGraphs = true);

		bool CloseWithPrompt(const char* identifier)			{ return Close (identifier, true); }
		bool Close(const char* identifier, bool askSaveChanges = false);
		bool Save(const char* identifier);
		
		// access open files
		uint32 GetNumOpenFiles() const							{ return mOpenFiles.Size(); }
		const OpenFile& GetOpenFile(uint32 index) const			{ return *mOpenFiles[index]; }
		uint32 FindOpenFile(const char* identifier) const;

	signals:
		void OnFileOpened(const char* identifier);	// post-open event
		void OnFileOpenFailed(const char* identifier);	// post-open event
		// TODO more file signals

		void writeToBackendFinished();

	private:
		// Note: make array accesses threadsafe?
		Core::Array<OpenFile*>	mOpenFiles;
		
		void TryInitializeExperience();
		// only for opening experiences: identifiers while loading experience; empty if nothing is loading
		Core::String			mLoadingExperienceIdentifier;
		Experience*				mLoadingExperience;
		bool					mExperienceLoaded;
		Core::String			mLoadingClassifierIdentifier;
		Classifier*				mLoadingClassifier;
		bool					mClassifierLoaded;
		Core::String			mLoadingStateMachineIdentifier;
		StateMachine*			mLoadingStateMachine;
		bool					mStateMachineLoaded;

		Core::String			mTempString;

		bool  mIsInFileSaving;

		bool Close(OpenFile* file);
		bool Save(OpenFile* file);

		void FinishReadData(OpenFile* file);

		bool ReadData(OpenFile* file);
		bool ReadDataFromBackend(OpenFile* file);
		// TODO reader for other locations

		bool WriteData(OpenFile* file);
		bool WriteDataToBackend(OpenFile* file);
		// TODO writer for other locations
};


#endif
