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
#include "GraphInfoWidget.h"

using namespace Core;

// constructor
GraphInfoWidget::GraphInfoWidget(QWidget* parent, Classifier* classifier) : QWidget(parent)
{
	mClassifier = classifier;
	
	// register event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	Init();
}


// destructor
GraphInfoWidget::~GraphInfoWidget()
{
	// unregister event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init widgets
void GraphInfoWidget::Init()
{

	// if classifier is not set yet, get the active one 
	if (mClassifier == NULL)
		mClassifier = GetEngine()->GetActiveClassifier();

	// vertical layout
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin(0);

	// create information tree
	mInfoTree = new QTreeWidget();
	//mInfoTree->setFixedHeight(400);
	mInfoTree->setSelectionMode( QAbstractItemView::SingleSelection);
	mInfoTree->setSortingEnabled(false);
	mInfoTree->setAlternatingRowColors(true);
	mInfoTree->setAnimated(true);
	
	// column and header settings
	mInfoTree->setColumnCount(2);
	//mInfoTree->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
	mInfoTree->header()->setStretchLastSection(true);
	
	QStringList headerLabels;
	headerLabels << "Property" << "Value";
	mInfoTree->setHeaderLabels(headerLabels);

	//
	// add tree items
	//

	AddItems();


	// add tree
	layout->addWidget(mInfoTree);
	
	setLayout(layout);
}


// ReInit: clear information tree, and add all the items again
void GraphInfoWidget::ReInit()
{
	mInfoTree->clear();
	AddItems();
}


// update displayed interface information
void GraphInfoWidget::UpdateInterface()
{
	UpdateClassifierSectionItems(mInfoTree);
	UpdateInputsSectionItems(mInputsSection);
	UpdateOutputsSectionItems(mOutputsSection);
}

// add all items to the tree
void GraphInfoWidget::AddItems()
{
	// classifier section
	AddClassifierSectionItems(mInfoTree);
	
	// inputs section
	mInputsSection = new QTreeWidgetItem();
	mInputsSection->setText(0, "Inputs");
	mInfoTree->addTopLevelItem(mInputsSection);

	AddInputsSectionItems(mInputsSection);
	mInputsSection->setExpanded(true);
		
	// outputs section
	mOutputsSection = new QTreeWidgetItem();
	mOutputsSection->setText(0, "Outputs");
	mInfoTree->addTopLevelItem(mOutputsSection);

	AddOutputsSectionItems(mOutputsSection);
	mOutputsSection->setExpanded(true);

}

// fill the classifier section with items
void GraphInfoWidget::AddClassifierSectionItems(QTreeWidget* parent)
{
	QTreeWidgetItem* item;

	// name
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Name");

	// number of nodes, connections
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Nodes / Cons");

	// number of buffers (node outputs only, for now)
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Buffers");
	
	// graph max delay
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Graph Delay");

	// graph max latency
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Graph Latency");

	// input latency
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Input Latency");

	// buffer memory usage
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Buffers");

	// sample memory usage
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Storage (In/Out/Sum)");

	// uploaded data
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Upload");
	
	// performance
	item = new QTreeWidgetItem(parent);
	item->setText(0, "Performance");
}


// udpate the value of the items
void GraphInfoWidget::UpdateClassifierSectionItems(QTreeWidget* parent)
{
	if (mClassifier == NULL)
		return;

	QTreeWidgetItem* item;

	// name
	item = parent->topLevelItem(0);
	//item->setText(1, mClassifier->GetName());

	// number of nodes, connections
	item = parent->topLevelItem(1);
	mTempString.Format("%i / %i", mClassifier->GetNumNodes(), mClassifier->GetNumConnections());
	item->setText(1, mTempString.AsChar());

	// number of buffer channels
	item = parent->topLevelItem(2);
	mTempString.Format("%i", mClassifier->CalcNumBufferChannelsUsed());
	item->setText(1, mTempString.AsChar());
	
	// graph max delay
	item = parent->topLevelItem(3);
	const double delayInSeconds = mClassifier->FindMaximumPathDelay();
	mTempString.Format("%.3f ms", delayInSeconds * 1000.0);
	item->setText(1, mTempString.AsChar());

	// graph max latency
	item = parent->topLevelItem(4);
	const double latencyInSeconds = mClassifier->FindMaximumLatency();
	mTempString.Format("%.3f ms", latencyInSeconds * 1000.0);
	item->setText(1, mTempString.AsChar());

	// input latency
	item = parent->topLevelItem(5);
	const double inputLatencyInSeconds = mClassifier->FindMaximumInputLatency();
	mTempString.Format("%.3f ms", inputLatencyInSeconds * 1000.0);
	item->setText(1, mTempString.AsChar());

	// buffer memory usage
	item = parent->topLevelItem(6);
	const double kiloByteAllocated = mClassifier->CalculateBufferMemoryAllocated() / 1024.0;
	const double kiloByteUsed = mClassifier->CalculateBufferMemoryUsed() / 1024.0;
	mTempString.Format("%.2f / %.2f KB", kiloByteUsed, kiloByteAllocated);
	item->setText(1, mTempString.AsChar());

	// sample memory usage
	item = parent->topLevelItem(7);
	const double inputKB = mClassifier->CalculateInputMemoryUsed() / 1024.0;
	const double outputKB = mClassifier->CalculateOutputMemoryUsed() / 1024.0;
	mTempString.Format("%.2f / %.2f / %.2f KB", inputKB, outputKB, inputKB + outputKB);
	item->setText(1, mTempString.AsChar());

	// upload usage
	item = parent->topLevelItem(8);
	const double uploadKBpm = mClassifier->CalculateUploadData(60) / 1024.0;
	mTempString.Format("%.2f KB / min", uploadKBpm);
	item->setText(1, mTempString.AsChar());
	
	// performance
	item = parent->topLevelItem(9);

	const double bestCase = mClassifier->GetFpsCounter().GetBestCaseTiming();
	const double worstCase = mClassifier->GetFpsCounter().GetWorstCaseTiming();
	const double averageTime = mClassifier->GetFpsCounter().GetAveragedTimeDelta();

	mTempString.Format("%.3f ms / %.3f ms / %.3f ms", bestCase * 1000.0, averageTime * 1000.0, worstCase * 1000.0);
	item->setText(1, mTempString.AsChar() );
}


// fill the inputs section with items
void GraphInfoWidget::AddInputsSectionItems(QTreeWidgetItem* parent)
{
	if (mClassifier == NULL)
		return;

	const uint32 numInputs = mClassifier->GetNumInputNodes();
	for (uint32 i=0; i<numInputs; ++i)
	{
		InputNode* inputNode = mClassifier->GetInputNode(i);

		// add toplevel item
		QTreeWidgetItem* section = new QTreeWidgetItem(parent);
		section->setText(0, inputNode->GetName());

		// max latency
		QTreeWidgetItem* item = new QTreeWidgetItem(section);
		item->setText(0, "Max Latency");

		// memory used
		item = new QTreeWidgetItem(section);
		item->setText(0, "Memory Used");
	}

}


// udpate the value of the items
void GraphInfoWidget::UpdateInputsSectionItems(QTreeWidgetItem* parent)
{
	if (mClassifier == NULL)
		return;

	// skip update if parent is collapsed
	if (parent->isExpanded() == false)
		return;

	const int32 numInputs = mClassifier->GetNumInputNodes();
	for (int32 i=0; i<numInputs; ++i)
	{
		InputNode* inputNode = mClassifier->GetInputNode(i);

		// make sure this node is in the list
		if (i >= parent->childCount())
			continue;

		// get section
		QTreeWidgetItem* section = parent->child(i);
		
		// skip update if section is collapsed
		if (section->isExpanded() == false)
			continue;

		// max latency
		QTreeWidgetItem* item = section->child(0);
		mTempString.Format("%.3f ms", inputNode->FindMaximumInputLatency() * 1000.0);
		item->setText(1, mTempString.AsChar());

		// memory used
		item = section->child(1);
		mTempString.Format("%.2f KB", inputNode->CalculateInputMemoryUsed() / 1024.0);
		item->setText(1, mTempString.AsChar());
	}

}


// fill the outputs section with items
void GraphInfoWidget::AddOutputsSectionItems(QTreeWidgetItem* parent)
{
	if (mClassifier == NULL)
		return;

	const uint32 numOutputs = mClassifier->GetNumOutputNodes();
	for (uint32 i=0; i<numOutputs; ++i)
	{
		OutputNode* outputNode = mClassifier->GetOutputNode(i);

		// add toplevel item
		QTreeWidgetItem* section = new QTreeWidgetItem(parent);
		section->setText(0, outputNode->GetName());

		// path delay
		QTreeWidgetItem* item = new QTreeWidgetItem(section);
		item->setText(0, "Path Delay" );

		// num samples
		item = new QTreeWidgetItem(section);
		item->setText(0, "Samples");

		// memory usage
		item = new QTreeWidgetItem(section);
		item->setText(0, "Memory Used");
	}
}


// udpate the value of the items
void GraphInfoWidget::UpdateOutputsSectionItems(QTreeWidgetItem* parent)
{
	if (mClassifier == NULL)
		return;

	// skip update if parent is collapsed
	if (parent->isExpanded() == false)
		return;

	const int32 numOutputs = mClassifier->GetNumOutputNodes();
	for (int32 i=0; i<numOutputs; ++i)
	{
		OutputNode* outputNode = mClassifier->GetOutputNode(i);

		// make sure this node is in the list
		if (i >= parent->childCount())
			continue;

		// get section
		QTreeWidgetItem* section = parent->child(i);

		// skip update if section is collapsed
		if (section->isExpanded() == false)
			continue;

		// path delay
		QTreeWidgetItem* item = section->child(0);
		mTempString.Format("%.3f ms", outputNode->FindMaximumDelay() * 1000.0);
		item->setText(1, mTempString.AsChar());

		// num samples
		item = section->child(1);
		if (outputNode->GetNumInputPorts() > 0)
			mTempString.Format("%i", outputNode->GetOutputChannel(0)->GetNumSamples());
		item->setText(1, mTempString.AsChar());

		// memory usage
		item = section->child(2);
		const double kiloByteUsed = outputNode->CalculateOutputMemoryUsed() / 1024.0;
		mTempString.Format("%.2f KB", kiloByteUsed);
		item->setText(1, mTempString.AsChar());
	}
}
