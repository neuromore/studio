/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required files
#include "BackendParameters.h"
#include "BackendHelpers.h"
#include "../QtBaseManager.h"
#include <Core/LogManager.h>
#include <Core/Json.h>
#include <QNetworkReply>
#include <QByteArray>
#include "ParametersSetRequest.h"
#include "ParametersSetResponse.h"
#include "ParametersFindRequest.h"
#include "ParametersFindResponse.h"


using namespace Core;

// constructor
BackendParameters::BackendParameters(NetworkAccessManager* networkAccessManager, QObject* parent) : QObject(parent)
{
	mNetworkAccessManager = networkAccessManager;

	// update classifier directly after parameteres were loaded
	connect(this, &BackendParameters::FindParametersFinished, this, &BackendParameters::OnFindParametersFinished);
}


// destructor
BackendParameters::~BackendParameters()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process parameters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// load parameters for active experience, classifier and statemachine and apply them immediately
void BackendParameters::Load(bool emitSignal, const User& user)
{
	const Experience* experience = GetEngine()->GetActiveExperience();
	const Classifier* classifier = GetEngine()->GetActiveClassifier();
	const StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	Load(emitSignal, user, experience, classifier, stateMachine);
}


// Retrieve all parameters for an experience/classifier
void BackendParameters::Load(bool emitSignal, const User& user, const Experience* experience, const Classifier* classifier, const StateMachine* stateMachine)
{
	// create request
	ParametersFindRequest request(GetUser()->GetToken(), user.GetId(), experience, classifier, stateMachine );

	// check if request contains any parameters at all and then skip it
	if (request.GetNumParameters() == 0)
	{
		if (emitSignal)
			emit LoadFinished(true);
		
		return;
	}

	// TODO get rid of this!!
	mBlockLoadFinishSignalOnce = !emitSignal;

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	
	// TODO
	connect(reply, &QNetworkReply::finished, this, &BackendParameters::OnFindParametersReply);
}


// save all parameters to the backend
void BackendParameters::Save(const User& user, const Session& session) const
{
	const Experience* experience = GetEngine()->GetActiveExperience();
	const Classifier* classifier = GetEngine()->GetActiveClassifier();
	const StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	Save(user, session, experience, classifier, stateMachine);
}



// send all cloud outputs values of the classifier and all experience settings to the parameter storage
void BackendParameters::Save(const User& user, const Session& session, const Experience* experience, const Classifier* classifier, const StateMachine* stateMachine) const
{
	// collect parameters
	CloudParameters parameters(&user);
	if (experience != NULL)
		experience->SaveCloudParameters(parameters);

	if (classifier != NULL)
		classifier->SaveCloudParameters(parameters);

	// for completeness, there are no cloud parameters for state machines yet
	//if (statemachine != NULL)
	//	statemachine->SaveCloudParameters(parameters);


	LogDebug("BackendParameters::Save(): saving %i parameters", parameters.GetParameters().Size());

	// no parameters to set -> skip request
	if (parameters.GetParameters().Size() == 0)
	{
		emit SaveFinished(true);
		return;
	}

	// create request
	ParametersSetRequest request(GetUser()->GetToken(), user.GetId(), parameters);

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	
	// TODO
	connect(reply, &QNetworkReply::finished, this, &BackendParameters::OnSetParametersReply);
}


// check return code
void BackendParameters::OnSetParametersReply()
{
	// get access to the network reply
	QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

	// translate it to our data model
	ParametersSetResponse response(networkReply);

	if (response.HasError() == true)
	{
		BackendHelpers::ReportError("Error parsing SetParameter JSON reply.");
		emit SaveFinished(false);
	}
	else
	{
		emit SaveFinished(true);
	}
}


// check return code, parse retreived parameters and update the classifier
void BackendParameters::OnFindParametersReply()
{
	// get access to the network reply
	QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

	ParametersFindResponse response(networkReply);

	if (response.HasError() == true)
	{
		BackendHelpers::ReportError("Error parsing FindParameter JSON reply.");
		return;
	}

	emit FindParametersFinished( response.GetParameters() );
}


void BackendParameters::OnFindParametersFinished(CloudParameters parameters)
{
	// load parameters into the engine
	Experience* experience = GetEngine()->GetActiveExperience();
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	//StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	uint32 numLoaded = 0;

	if (experience != NULL)
		numLoaded += experience->LoadCloudParameters(parameters);

	if (classifier != NULL)
		numLoaded += classifier->LoadCloudParameters(parameters);

	//if (stateMachine != NULL)
		//numLoaded += stateMachine->LoadCloudParameters(parameters);

	const uint32 numRetrieved = parameters.GetParameters().Size();
	if (numLoaded != numRetrieved)
		LogWarning ("BackendParameters::OnFindParametersFinished: retrieved %i parameters, but loaded %i!", numRetrieved, numLoaded);

	// emit signal if not blocked
	if (mBlockLoadFinishSignalOnce == true)
		mBlockLoadFinishSignalOnce = false;	// block it only once -> reset flag
	else
		emit LoadFinished(true);
}

