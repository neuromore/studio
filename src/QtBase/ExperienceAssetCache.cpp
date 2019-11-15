/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "ExperienceAssetCache.h"
#include "QtBaseManager.h"
#include <EngineManager.h>
#include <Graph/StateMachine.h>


using namespace Core;

// constructor
ExperienceAssetCache::ExperienceAssetCache(QObject* parent) : QObject(parent) 
{
	// web data cache
	mCache = new WebDataCache( "Cache/Experiences/Assets/", this);
	connect(mCache, SIGNAL(FinishedDownload()), this, SLOT(OnFinishedPreloading()));

	// enable max cache size restriction
	//mCache->Log();
	const uint32 maxCacheSizeInMb = 5*1024; // 5GB default cache size
	mCache->RestrictCacheSize( maxCacheSizeInMb );
	mCache->RestrictAliveTime( 90 );
	mCache->CleanCache();
	//mCache->Log();
}


// destructor
ExperienceAssetCache::~ExperienceAssetCache()
{
	// web data cache
	delete mCache;
}


void ExperienceAssetCache::PreloadAssets()
{
	CORE_EVENTMANAGER.OnProgressStart( true, false, false, false );
	CORE_EVENTMANAGER.OnProgressText( "Downloading assets ..." );

	// lock the interface
	GetQtBaseManager()->UpdateInterfaceLock();

	// pre-load data
	Core::Array<String> assetUrls;

	// get the currently active state machine
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine != NULL)
	{
		stateMachine->CollectStates();
		stateMachine->CollectAssets();
		Core::Array<StateMachine::Asset> assets = stateMachine->GetAssets();
		const uint32 numAssets = assets.Size();
		for (uint32 i=0; i<numAssets; ++i)
			assetUrls.Add( assets[i].mLocation );
	}

	mCache->Download(assetUrls);
}


// called when all assets got downloaded
void ExperienceAssetCache::OnFinishedPreloading()
{
	CORE_EVENTMANAGER.OnProgressEnd();

	// lock the interface
	GetQtBaseManager()->UpdateInterfaceLock();

	emit FinishedPreloading();
}