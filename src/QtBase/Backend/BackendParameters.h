/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BACKENDPARAMETERS_H
#define __NEUROMORE_BACKENDPARAMETERS_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "NetworkAccessManager.h"
#include <Graph/Classifier.h>
#include <EngineManager.h>


// class for handling backend parameters
class QTBASE_API BackendParameters : public QObject
{
	Q_OBJECT

	public:

		// constructor & destructor
		BackendParameters(NetworkAccessManager* networkAccessManager, QObject* parent=NULL);
		virtual ~BackendParameters();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Process experience/classifier parameters
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// retrieve settings for individual 
		void Load(bool emitSignal, const User& user);
		void Load(bool emitSignal, const User& user, const Experience* experience = NULL, const Classifier* classifier = NULL, const StateMachine* stateMachine = NULL);

		// set all parameters in the backend storage
		void Save(const User& user, const Session& session) const;
		void Save(const User& user, const Session& session, const Experience* experience, const Classifier* classifier = NULL, const StateMachine* stateMachine = NULL) const;

	private slots:
		void OnFindParametersReply();
		void OnSetParametersReply();
		void OnFindParametersFinished(CloudParameters parameters);

	signals:
		void FindParametersFinished(CloudParameters parameters);

		void LoadFinished(bool success) const;
		void SaveFinished(bool success) const;

	private:
		NetworkAccessManager*	mNetworkAccessManager;
		bool					mBlockLoadFinishSignalOnce;

};


#endif
