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
#include "LoretaPlugin.h"
#ifdef OPENCV_SUPPORT
#include <Core/LogManager.h>
#include <AttributeWidgets/AttributeSetGridWidget.h>
#include <QtBaseManager.h>

// include OpenCV
#include <opencv2/opencv.hpp>


using namespace Core;

LoretaPlugin::LoretaPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Example OpenGL plugin ...");
	mLoretaWidget = NULL;

	// init threadhandler
	mLoretaThreadHandler = new LoretaThreadHandler(mLoretaWidget);
	mThread = new Thread(mLoretaThreadHandler, "LoretaThread");

	CORE_EVENTMANAGER.AddEventHandler(this);
}


// destructor
LoretaPlugin::~LoretaPlugin()
{ 
	LogDetailedInfo("Destructing Example OpenGL plugin ...");

	delete mThread;
	delete mLoretaWidget;

	CORE_EVENTMANAGER.RemoveEventHandler(this);

}


// initialize
bool LoretaPlugin::Init()
{
	LogDetailedInfo("Initializing Example OpenGL plugin ...");

	QWidget*        mainWidget = NULL;
	QHBoxLayout*	mainLayout = NULL;
	CreateDockMainWidget(&mainWidget, &mainLayout);

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	///////////////////////////////////////////////////////////////////////////
	// Settings
	///////////////////////////////////////////////////////////////////////////

	// create the attribute set grid widget
	AttributeSetGridWidget* attributeSetGridWidget = new AttributeSetGridWidget();
	attributeSetGridWidget->ReInit(this);
	// add the dialog stack to the right side
	SetSettingsWidget(attributeSetGridWidget);

	///////////////////////////////////////////////////////////////////////////
	// Add render widget at the end
	///////////////////////////////////////////////////////////////////////////

	mLoretaWidget = new LoretaWidget(this, mainWidget);
	SetRealtimeWidget(mLoretaWidget);

	///////////////////////////////////////////////////////////////////////////
	// ReInit() LORETA 
	///////////////////////////////////////////////////////////////////////////
	InitRenderMesh();
	ReInitLoreta();

	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mLoretaWidget);

	LogDetailedInfo("Example OpenGL plugin successfully initialized");

	connect( attributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChanged(Property*)) );

	return true;
}


// reinit LORETA i.e. if electrodes have changed
void LoretaPlugin::ReInitLoreta()
{
	uint32 numElectrodes = 0;

	// get loreta node from classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();

	// no classifier -> reset loreta
	if (classifier == NULL)
	{
		mLoretaThreadHandler->Reset();
		mLoretaThreadHandler->SetNumBaseVoxels(0);
	}
	else if (classifier->GetNumLoretaNodes() > 0)
	{
		LoretaNode* node = classifier->GetLoretaNode(0);
		numElectrodes = node->GetNumChannels();

		// check if channels are still the same and reinit Loreta if something has changed
		bool hasChanged = false;

		// check the number of channels first
		const uint32 lastNumElectrodes = mLoretaThreadHandler->GetNumElectrodes();
		if (numElectrodes != lastNumElectrodes)
		{
			hasChanged = true;
		}
		else
		{
			for (uint32 i = 0; i < numElectrodes; ++i)
			{
				// check if channels are still the same (pointer comparison is intentional and OK here)
				if (node->GetChannel(i) != mLoretaThreadHandler->GetChannel(i))
				{
					hasChanged = true;
					break;
				}
			}
		}

		// check if attributes have changed
		const uint32 lastNumBaseVoxel = mLoretaThreadHandler->GetNumBaseVoxel();
		const double lastConductivity = mLoretaThreadHandler->GetConductivity();
		const double lastRegParameter = mLoretaThreadHandler->GetRegParameter();

		if (GetNumVoxelSpinner() != lastNumBaseVoxel)
			hasChanged = true;

		if (GetConductivity() != lastConductivity)
			hasChanged = true;

		if (GetRegParameter() != lastRegParameter)
			hasChanged = true;

		// Loreta reinitialization
		if (hasChanged == true)
		{
			LogDebug("LoretaPlugin: Reinitializing LORETA numVoxels=%i, numElec=%i", GetNumVoxelSpinner(), numElectrodes);

			// setup Loreta from plugin settings
			mLoretaThreadHandler->SetNumBaseVoxels(GetNumVoxelSpinner());
			mLoretaThreadHandler->SetConductivity(GetConductivity());
			mLoretaThreadHandler->SetRegParameter(GetRegParameter());
			
			// wait until thread has stopped
			mLoretaThreadHandler->SetCalcLoreta(false);
			mThread->WaitForThreadFinish();

            // remove all electrodes
			mLoretaThreadHandler->Reset();

			// add all electrodes and their channels
			for (uint32 i = 0; i < numElectrodes; ++i)
				mLoretaThreadHandler->AddElectrode(node->GetElectrode(i), node->GetChannel(i));

			// setup loreta again
			mLoretaThreadHandler->Init();

			// start thread again
			mLoretaThreadHandler->SetCalcLoreta(true);

			mThread->Start();
			mLoretaThreadHandler->CalcMaskingMatrix(&mBrainMesh);
		}
	}

}


void LoretaPlugin::InitRenderMesh()
{
	
	switch (GetBrainPartSelection())
	{
	case 0:
		mBrainMesh.Load(":/Assets/Models/Brain.obj");
		break;
	case 1:
		mBrainMesh.Load(":/Assets/Models/Brain_Cerebellum.obj");
		break;
	case 2:
		mBrainMesh.Load(":/Assets/Models/Brain_FrontalLobe.obj");
		break;
	case 3:
		mBrainMesh.Load(":/Assets/Models/Brain_OccipitalLobe.obj");
		break;
	case 4:
		mBrainMesh.Load(":/Assets/Models/Brain_ParietalLobe.obj");
		break;
	case 5:
		mBrainMesh.Load(":/Assets/Models/Brain_TemporallLobe.obj");
		break;
	case 6:
		mBrainMesh.Load(":/Assets/Models/Sphere.obj");
		break;
	}

	// calculate AABB
	mBrainMesh.CalcAABB(&mBrainMeshAABB);

	// calculate maximum AABB box
	double brainAABBWidth  = mBrainMeshAABB.GetWidth();
	double brainAABBHeight = mBrainMeshAABB.GetHeight();
	double brainAABBDepth  = mBrainMeshAABB.GetDepth();

	double maxAABB = brainAABBWidth;

	if (brainAABBHeight > maxAABB)
	{
		maxAABB = brainAABBHeight;
	}
	if (brainAABBDepth > maxAABB)
	{
		maxAABB = brainAABBDepth;
	}

	mLoretaThreadHandler->SetVisualSize(maxAABB);
	mLoretaThreadHandler->SetCenter(mBrainMeshAABB.GetCenter());

}


// called in case any of the attributes got changed
void LoretaPlugin::OnAttributeChanged(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	if (propertyInternalName.IsEqual("numVoxelSpinner") == true)
		ReInitLoreta();

	if (propertyInternalName.IsEqual("brainComboBox") == true)
	{
		// load new mesh
		InitRenderMesh();

		// recalculate masking matrix
		mLoretaThreadHandler->CalcMaskingMatrix(&mBrainMesh);
	}

	if (propertyInternalName.IsEqual("coloringComboBox") == true)
	{
		uint32 selectedColorIndex = GetInt32AttributeByName("coloringComboBox");
		mColorMapper.SetColorMapping((ColorMapper::EColorMapping)selectedColorIndex);
	}

	if (propertyInternalName.IsEqual("numConductivity") == true)
		ReInitLoreta();

	if (propertyInternalName.IsEqual("numRegParameter") == true)
		ReInitLoreta();

}


// register attributes and create the default values
void LoretaPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// show feedback control widget
	AttributeSettings* attributeSettings = RegisterAttribute("Number of rendered voxels", "numVoxelSpinner", "Number of voxels which should be rendered.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attributeSettings->SetDefaultValue( AttributeInt32::Create(10) );
	attributeSettings->SetMinValue( AttributeInt32::Create(10) );
	attributeSettings->SetMaxValue( AttributeInt32::Create(35) );

	attributeSettings = RegisterAttribute("Brain Part Selection", "brainComboBox", "Renders the selected part of the brain.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeSettings->ResizeComboValues(7);
	attributeSettings->SetComboValue(0, "Brain");
	attributeSettings->SetComboValue(1, "Cerebellum");
	attributeSettings->SetComboValue(2, "Frontal Lobe");
	attributeSettings->SetComboValue(3, "Occipital Lobe");
	attributeSettings->SetComboValue(4, "Parietal Lobe");
	attributeSettings->SetComboValue(5, "Temporall Lobe");
	attributeSettings->SetComboValue(6, "Sphere");
	attributeSettings->SetDefaultValue(AttributeInt32::Create(6));

	attributeSettings = RegisterAttribute("Transparency", "numTransparency", "Sets the rendering transparency.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.8) );
	attributeSettings->SetMinValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(1.0) );

	attributeSettings = RegisterAttribute("Regularization parameter", "numRegParameter", "Sets the influence of the electrodes in correlation to other electrodes.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMinValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(1.0) );

	attributeSettings = RegisterAttribute("Conductivity", "numConductivity", "Sets the conductivity of the electrodes.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.33) );
	attributeSettings->SetMinValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(FLT_MAX) );

	attributeSettings = RegisterAttribute("Render brain", "renderBrainCheckbox", "Render a brain mesh.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue(AttributeBool::Create(true));

	attributeSettings = RegisterAttribute("Render head", "renderHeadCheckbox", "Render a head mesh.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue(AttributeBool::Create(true));

	attributeSettings = RegisterAttribute("Coloring", "coloringComboBox", "Changes the way colors are displayed on the brain.", Core::ATTRIBUTE_INTERFACETYPE_COLORMAPPING);
	attributeSettings->SetDefaultValue(AttributeInt32::Create(0));

	#ifdef LORETA_DEBUG
	attributeSettings = RegisterAttribute("Activate debug grid", "debugGridCheckBox", "Activates a grid for debugging.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue(AttributeBool::Create(false));

	attributeSettings = RegisterAttribute("Enable TIFF multipage export", "exportTIFFCheckBox", "Enables TIFF multipage export.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue(AttributeBool::Create(false));

	#endif

	// create default attribute values
	CreateDefaultAttributeValues();
}


#endif
