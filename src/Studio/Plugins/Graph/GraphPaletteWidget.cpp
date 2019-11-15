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

// include the required headers
#include "GraphPaletteWidget.h"
#include "GraphPlugin.h"
#include <ImageManipulation.h>
#include <QtBaseManager.h>
#include <EngineManager.h>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTabBar>
#include <QIcon>
#include <QAction>
#include <QFile>
#include <QMimeData>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>


using namespace Core;

// return the mime data
QMimeData* GraphPaletteList::mimeData(const QList<QListWidgetItem*> items) const
{
	if (items.count() != 1)
		return NULL;

	QString textData;

	switch(mType)
	{
		case GraphWidget::CLASSIFIER:		{ textData = "Node;"; break; }
		case GraphWidget::STATEMACHINE:		{ textData = "State;"; break; }

		default: { break; }
	}

	// create the data and set the text
	QMimeData* data = new QMimeData();	
	textData += items.at(0)->data(Qt::UserRole).toString();	// the tooltip contains the class name
	data->setText( textData );

	return data;
}


// return the supported mime types
QStringList GraphPaletteList::mimeTypes() const
{
	QStringList result;
	result.append("text/plain");
	return result;
}


// get the allowed drop actions
Qt::DropActions GraphPaletteList::supportedDropActions() const
{
	return Qt::CopyAction;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

// constructor
GraphPaletteWidget::GraphPaletteWidget(GraphWidget::Type type, QWidget* parent) : QWidget(parent)
{
	LogDebug("Constructing palette widget ...");

	mType			= type;
	mGraphPlugin	= NULL;
	mTabBar			= NULL;
	mList			= NULL;

	QFont font;

	// set the initial text that appears inside the widget
	mInitialLayout = new QVBoxLayout();
	setLayout( mInitialLayout );
	QLabel* initialText = new QLabel("<c>Create and activate a <b>Classifier</b> first.<br>Then <b>drag and drop</b> items from the<br>palette into the <b>Classifier</b> window.</c>");
	initialText->setAlignment( Qt::AlignCenter );
	initialText->setTextFormat( Qt::RichText );
	initialText->setFont( font );
	mInitialLayout->addWidget( initialText );
	mInitialLayout->setMargin( 0 );
	mInitialLayout->setSpacing( 0 );
	initialText->setMaximumSize( 100000, 100000 );
	initialText->setMargin( 0 );
	initialText->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
	mInitialLayout->setSizeConstraint( QLayout::SetMaximumSize );
}


// destructor
GraphPaletteWidget::~GraphPaletteWidget()
{
}


// init the real interfaces
void GraphPaletteWidget::InitInterface()
{
	// if we already initialized, return
	if (mTabBar != NULL)
		return;
	else
	{
		delete mInitialLayout;
		mInitialLayout = NULL;
	}
	
	// create the layout
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	setLayout( layout );

	// the tab bar
	mTabBar = new QTabBar();
	connect(mTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnChangeCategoryTab(int)));

	layout->addWidget( mTabBar );

	// create the listview
	mList = new GraphPaletteList( mType );
	layout->addWidget( mList );

	// setup the list widget
	mList->setViewMode( QListView::IconMode );
	mList->setUniformItemSizes( false );
	mList->setSelectionMode( QAbstractItemView::SingleSelection );
	mList->setMovement( QListView::Static );
	mList->setWrapping( true );
	mList->setTextElideMode( Qt::ElideNone );
	mList->setDragEnabled(true);
	mList->setWordWrap(true);
	mList->setFlow( QListView::LeftToRight );
	//mList->setResizeMode( QListView::Adjust );
	mList->setDragDropMode( QAbstractItemView::DragOnly );
	mList->setIconSize( QSize(48, 48) );

	setMinimumHeight( 100 );
	setMaximumHeight( 280 );
}


void GraphPaletteWidget::Init()
{
	// create the tabbar etc
	InitInterface();

	// remove all tabs
	mTabBar->blockSignals(true);
	while (mTabBar->count() > 0)
		mTabBar->removeTab(0);

	switch(mType)
	{
		case GraphWidget::CLASSIFIER:
		{
			// add all category names to the tab bar and set tooltips
			for (uint32 i = 0; i < SPNode::NUM_CATEGORIES; ++i)
			{
				mTabBar->addTab(SPNode::GetCategoryName((SPNode::ECategory)i));
				mTabBar->setTabToolTip(i, SPNode::GetCategoryDescription((SPNode::ECategory)i));
			}
			break;
		}

		case GraphWidget::STATEMACHINE:
		{
			// add all category names to the tab bar
			for (uint32 i=0; i<State::NUM_CATEGORIES; ++i)
				mTabBar->addTab( State::GetCategoryName((State::ECategory)i) );
			
			break;
		}

		default: { break; }
	}

	mTabBar->blockSignals(false);

	if (mGraphPlugin != NULL)
	{
		const uint32 index = mGraphPlugin->GetPaletteTabIndex();
		mTabBar->setCurrentIndex( index );
		OnChangeCategoryTab(index);
	}
}


QString GraphPaletteWidget::GetNodeIconFileName(Node* node)
{
	return GetNodeIconFileName(node->GetTypeUuid());
}


QString GraphPaletteWidget::GetNodeIconFileName(QString nodeTypeString)
{
	QString fileName = ":/Images/Graph/";
	fileName = fileName + nodeTypeString + ".png";

	// return special icon if resource does not contain it
	if (QFile::exists( fileName ) == false)
		return ":/Images/Graph/UnknownNode.png";	

	return fileName;
}


QIcon GraphPaletteWidget::GetNodeIcon(Node* node)
{
	const bool createRight = GraphHelpers::GetCreud(node).Create();

	// get the corresponding icon
	QString iconFilename = GetNodeIconFileName(node);
	QPixmap orgIconPixmap(iconFilename);
	QPixmap iconPixmap;
		
	if (createRight == true)
		return orgIconPixmap;

	return ChangeOpacity(orgIconPixmap, 0.25);
}


// register list widget icons
void GraphPaletteWidget::RegisterItems(uint32 category)
{
	// clear the list
	mList->clear();

	QFont font;
    font.setPixelSize( (float)GetQtBaseManager()->GetMainWindow()->GetDefaultFontSize() * 0.9f );
	String iconString;
	QString iconFilename;

	// get the graph object factory
	GraphObjectFactory* objectFactory = GetGraphObjectFactory();

	// for all registered objects in the object factory
	const uint32 numRegistered = objectFactory->GetNumRegisteredObjects();
	for (uint32 i=0; i<numRegistered; ++i)
	{
		// get the object and only iterate through nodes
		GraphObject* registeredObject = objectFactory->GetRegisteredObject(i);

		if (mType == GraphWidget::CLASSIFIER && registeredObject->GetBaseType() != Node::BASE_TYPE)
			continue;

		if (mType == GraphWidget::STATEMACHINE && registeredObject->GetBaseType() != State::BASE_TYPE)
			continue;

		// get the node
		Node* node = static_cast<Node*>(registeredObject);

		// only load objects from the category we want
		if (node->GetPaletteCategory() != category)
			continue;

		// never display deprecated nodes or unstable nodes in production
#ifdef PRODUCTION_BUILD
		if (node->IsDeprecated() == true || node->IsUnstable() == true)
			continue;
#endif

		mTempString = node->GetReadableType();
		mTempString.Replace( StringCharacter::space, StringCharacter('\n') );
		
		// create the list widget item
		QListWidgetItem* item = new QListWidgetItem( GetNodeIcon(node), mTempString.AsChar(), mList, GraphPaletteList::NODETYPE_NODE );
		item->setData( Qt::UserRole, node->GetTypeUuid() );
		item->setToolTip( node->GetReadableType() );
		item->setFont( font );
	}
}


// a tab changed
void GraphPaletteWidget::OnChangeCategoryTab(int index)
{
	if (mGraphPlugin == NULL)
		return;

	switch (mType)
	{
		case GraphWidget::CLASSIFIER:
		{
			if (index < SPNode::NUM_CATEGORIES)
			{
				RegisterItems((SPNode::ECategory)index);
				mGraphPlugin->SetPaletteIndex(index);
			}
			else
				Core::LogError("GraphPaletteWidget::OnChangeCategoryTab() - Unknown category type (%d)", index);

			break;
		}

		case GraphWidget::STATEMACHINE:
		{
			if (index < State::NUM_CATEGORIES)
			{
				RegisterItems((State::ECategory)index);
				mGraphPlugin->SetPaletteIndex(index);
			}
			else
				Core::LogError("GraphPaletteWidget::OnChangeCategoryTab() - Unknown category type (%d)", index);
			
		}

		default: { break; }
	}

}
