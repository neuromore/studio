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

// include required headers
#include "GraphAttributesWidget.h"
#include <AttributeWidgets/AttributeWidgetFactory.h>
#include <QtBaseManager.h>
#include <EngineManager.h>
#include "../../AppManager.h"
#include "../../MainWindow.h"
#include <Config.h>

using namespace Core;

// constructor
GraphAttributesWidget::GraphAttributesWidget(QWidget* parent) : QScrollArea(parent)
{
	LogDebug("Constructing attributes widget ...");
	mGraphObject	= NULL;
	mNameProperty	= NULL;
	mParentGroupName= "";

	setContentsMargins(0,0,0,0);
	setViewportMargins(0,0,0,0);
	setObjectName("TransparentWidget");
	setWidgetResizable( true );

	// create the new property widget
	mPropertyTreeWidget = new PropertyTreeWidget();
	connect( mPropertyTreeWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnValueChanged(Property*)) );
	
	// create the graph object view widget
	mGraphObjectView = new GraphObjectViewWidget();

	// create the main vertical layout
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->setContentsMargins(0,0,0,0);

	vLayout->addWidget(mPropertyTreeWidget);
	vLayout->addWidget(mGraphObjectView);
	setLayout( vLayout );


	// register event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	// init
	InitForNode( NULL );
	show();
#ifdef INCLUDE_DEVICE_BRAINALIVE
	//m_bleInterface = new BLEInterface(this);
	//connect(m_bleInterface, &BLEInterface::dataReceived,
	//	this, &GraphAttributesWidget::dataReceived);

	//connect(m_bleInterface, &BLEInterface::devicesNamesChanged,
	//[this](QStringList devices) {
	//mListWidget->clear();
	//mListWidget->addItems(devices);
	//});
	//connect(m_bleInterface, &BLEInterface::servicesChanged,
	//[this](QStringList services) {
	////mListWidget->clear();
	////mListWidget->addItems(services);
	//});
	//mScan_Widget = new QWidget();
	//mListWidget = new QListWidget();
    #endif
}


// destructor
GraphAttributesWidget::~GraphAttributesWidget()
{
	// unregister event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}

void GraphAttributesWidget::dataReceived(QByteArray data)
{

}
// event handler callback: an attribute has changed, check if it was one of the displayed ones
void GraphAttributesWidget::OnAttributeUpdated(Graph* graph, GraphObject* object, Attribute* attribute)
{
	// no attribute set loaded yet
	if (mAttributeSet == NULL)
		return;

	// refresh interface if attribute is currently dislayed
	if (mAttributeSet->HasAttribute(attribute) == true)
		UpdateInterface();
}


// event handler callback: graph was modified; check if graph or graphobject was displayed
void GraphAttributesWidget::OnGraphModified(Graph* graph, GraphObject* object)
{
	if (mGraphObject != NULL && (mGraphObject == graph || mGraphObject == object) )
	{
		ForceReInit();
	}
}


void GraphAttributesWidget::InitForGraphObject(GraphObject* object, bool force)
{
	if (object == NULL)
	{
		InitForNode(NULL);
		return;
	}
	else if (object == mGraphObject && force == false)
	{
		// don't update if we already display this object
		return;
	}

	if (object->GetBaseType() == Graph::BASE_TYPE)				InitForGraph( static_cast<Graph*>(object) );				// classifier
	else if (object->GetBaseType() == Node::BASE_TYPE)			InitForNode( static_cast<Node*>(object) );					// classifier node
	else if (object->GetType() == StateMachine::TYPE_ID)		InitForGraph( static_cast<StateMachine*>(object), false );	// state machine
	else if (object->GetBaseType() == State::BASE_TYPE)			InitForNode(static_cast<State*>(object));					// state
	else if (object->GetBaseType() == Connection::BASE_TYPE)	InitForConnection( static_cast<Connection*>(object) );		// classifier connection
	else														InitForNode(NULL);
}


// init for a given graph (classifier or statemachine)
void GraphAttributesWidget::InitForGraph(Graph* graph, bool showName)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = graph;
	mAttributeSet = NULL;

	// if there is no node, leave
	if (graph == NULL)
		return;
	
	// is the attribute editable?
	const bool allowsEditing = graph->AllowEditing();
	const bool sessionRunning = GetSession()->IsRunning();
	const bool isReadOnly = sessionRunning || !allowsEditing;

	// add the name as property first
	if (showName == true)
		mNameProperty = mPropertyTreeWidget->GetPropertyManager()->AddStringProperty( mParentGroupName.AsChar(), "Name", graph->GetName(), 0, isReadOnly);
	else
		mNameProperty = NULL;


	// add attributes
	AddAttributes( graph, mParentGroupName.AsChar(), isReadOnly);	

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}


// init for a given node (from classifier or state machine)
void GraphAttributesWidget::InitForNode(Node* node, bool showName)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = node;
	mAttributeSet = NULL;

	// if there is no node, leave
	if (node == NULL)
		return;

	// we can't handle nodes without parents here (they always belong to a graph anyways...)
	Graph* parent = node->GetParent();
	if (parent == NULL)
		return;

	// graph editable? (Read access and not locked)
	const bool graphNotEditable = (parent->AllowEditing() == false);
	
	// FIXME this is a little hacky:
	//Experience* experience = GetEngine()->GetActiveExperience();

	const bool graphSettingsMode = GetMainWindow()->IsSettingsModeEnabled();
	const bool sessionRunning = GetSession()->IsRunning();
	const bool nodeLocked = node->IsLocked();

	const bool isAttributeReadOnly = sessionRunning || nodeLocked || ( !graphSettingsMode && graphNotEditable);
	const bool isNameReadOnly = sessionRunning || nodeLocked || graphNotEditable;

	// add the name as property first
	if (showName == true)
	{
		mNameProperty = mPropertyTreeWidget->GetPropertyManager()->AddStringProperty(mParentGroupName.AsChar(), "Node Name", node->GetName(), 0, isNameReadOnly);
		if (node->GetNameString() == "BrainAlive")
		{
                /*  mChannel_Config = mPropertyTreeWidget->GetPropertyManager()
                          ->AddButtonProperty(mParentGroupName.AsChar(),
                                              "Channel Settings",
                                              "Configuration", true);
			mNameProperty_1 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 1", ch_data, Val[0].ToInt(), false);
			mNameProperty_2 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 2", ch_data_2, Val[1].ToInt(), false);
			mNameProperty_3 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 3", ch_data_3, Val[2].ToInt(), false);
			mNameProperty_4 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 4", ch_data_4, Val[3].ToInt(), false);
			mNameProperty_5 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 5", ch_data_5, Val[4].ToInt(), false);
			mNameProperty_6 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 6", ch_data_6, Val[5].ToInt(), false);
			mNameProperty_7 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 7", ch_data_7, Val[6].ToInt(), false);
			mNameProperty_8 = mPropertyTreeWidget->GetPropertyManager()->AddComboBoxProperty(mParentGroupName.AsChar(), "Ch 8", ch_data_8, Val[7].ToInt(), false);
			mNameProperty_9 = mPropertyTreeWidget->GetPropertyManager()->AddButtonProperty(mParentGroupName.AsChar(), "Apply Settings", "Apply", true);
			mNameProperty_10 = mPropertyTreeWidget->GetPropertyManager()->AddButtonProperty(mParentGroupName.AsChar(), "Start Scaning", "Scan", true);
			mNameProperty_11 = mPropertyTreeWidget->GetPropertyManager()->AddStringProperty(mParentGroupName.AsChar(), "Send Command","", 0, isNameReadOnly);
			mNameProperty_12 = mPropertyTreeWidget->GetPropertyManager()->AddButtonProperty(mParentGroupName.AsChar(), " ", "Send", true);*/
			// create and return the property

			
		}
	}
	else
		mNameProperty = NULL;

	// node attributes
	AddAttributes( node, mParentGroupName.AsChar(), isAttributeReadOnly );

	// if node is a state: add actions
	if (node->GetBaseType() == State::BASE_TYPE)
		AddActions( node, isAttributeReadOnly );

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}


// called in case any of the properties gets changed
void GraphAttributesWidget::OnValueChanged(Property* property)
{
	// when changing the node name
	if (property == mNameProperty)
		mGraphObject->SetName(property->AsString().AsChar());
#ifdef INCLUDE_DEVICE_BRAINALIVE
	//else if (property == mNameProperty_1)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[0]);
	//	data_1 = ch_data.GetItem(Val[0].ToInt());
	//}
	//else if (property == mNameProperty_2)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[1]);
	//	data_2 = ch_data_2.GetItem(Val[1].ToInt());
	//}
	//else if (property == mNameProperty_3)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[2]);
	//	data_3 = ch_data_3.GetItem(Val[2].ToInt());

	//}
	//else if (property == mNameProperty_4)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[3]);
	//	data_4 = ch_data_4.GetItem(Val[3].ToInt());
	//}
	//else if (property == mNameProperty_5)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[4]);
	//	data_5 = ch_data_5.GetItem(Val[4].ToInt());
	//}
	//else if (property == mNameProperty_6)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[5]);
	//	data_6 = ch_data_6.GetItem(Val[5].ToInt());
	//}
	//else if (property == mNameProperty_7)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[6]);
	//	data_7 = ch_data_7.GetItem(Val[6].ToInt());
	//}
	//else if (property == mNameProperty_8)
	//{
	//	property->GetAttributeValue()->ConvertToString(Val[7]);
	//	data_8 = ch_data_8.GetItem(Val[7].ToInt());
	//}
	//else if (property == mNameProperty_10)
	//{
	//	
	//	 if((mScan_Widget->isVisible() )== false)
	//	{
	//		Connect = new QPushButton();
	//		Connect->setText(" Connect ");
	//		
	//		mScan_Widget->setFixedHeight(400);
	//		mScan_Widget->setFixedWidth(400);
	//		QVBoxLayout* vLayout = new QVBoxLayout();


	//		mListWidget->setFixedHeight(380);
	//		mListWidget->setFixedWidth(380);
	//		vLayout->setMargin(10);
	//		vLayout->setSpacing(10);

	//		vLayout->addWidget(mListWidget, 1, Qt::AlignTop);
	//		vLayout->addWidget(Connect, 1, Qt::AlignBottom);
	//		mScan_Widget->setLayout(vLayout);
	//		mScan_Widget->setVisible(true);
	//		m_bleInterface->scanDevices();
	//		connect(Connect, &QPushButton::clicked, this, &GraphAttributesWidget::On_connect);
	//	} 
	//	 else
	//	 {
	//		 mScan_Widget->show();
	//		 //mScan_Widget->showMaximized();
	//		 mScan_Widget->activateWindow();
	//		 mListWidget->show();

	//	 }

	//		
	//} else if (property == mChannel_Config) {
 //         gain_combo = new QComboBox();
 //         channel_no = new QComboBox();
 //         input_type = new QComboBox();
 //         bias_type = new QComboBox();
 //         Power_down = new QComboBox();
 //         sendcmd_2 = new QPushButton();
 //         QCheckBox *mShowUsedCheckbox_4 = new QCheckBox();
 //         QWidget *menuWidget = new QWidget();
 //         QHBoxLayout *hLayout_1 = new QHBoxLayout();
 //         hLayout_1->setMargin(10);
 //         hLayout_1->setSpacing(10);
 //         channel_no->setFixedHeight(20);
 //         channel_no->setFixedWidth(75);
 //         Power_down->setFixedHeight(20);
 //         Power_down->setFixedWidth(75);
 //         gain_combo->setFixedHeight(20);
 //         gain_combo->setFixedWidth(75);
 //         input_type->setFixedHeight(20);
 //         input_type->setFixedWidth(75);
 //         bias_type->setFixedHeight(20);
 //         bias_type->setFixedWidth(75);
 //         channel_no->addItem("Channel");
 //         channel_no->addItem("1");
 //         channel_no->addItem("2");
 //         channel_no->addItem("3");
 //         channel_no->addItem("4");
 //         channel_no->addItem("5");
 //         channel_no->addItem("6");
 //         channel_no->addItem("7");
 //         channel_no->addItem("8");
 //         channel_no->addItem("All channel");
 //         Power_down->addItem("Channel State");
 //         Power_down->addItem("Enable");
 //         Power_down->addItem("Disable");
 //         gain_combo->addItem("Gain");
 //         gain_combo->addItem("1");
 //         gain_combo->addItem("2");
 //         gain_combo->addItem("3");
 //         gain_combo->addItem("4");
 //         gain_combo->addItem("6");
 //         gain_combo->addItem("8");
 //         gain_combo->addItem("12");
 //         input_type->addItem("Input Type");
 //         input_type->addItem("Normal");
 //         input_type->addItem("Shorted");
 //         input_type->addItem("Bias Meas");
 //         input_type->addItem("MVDD");
 //         input_type->addItem("Temp");
 //         input_type->addItem("Test");
 //         input_type->addItem("BIAS_DRP");
 //         input_type->addItem("BIAS_DRN");
 //         bias_type->addItem("BIAS");
 //         bias_type->addItem("Not Include");
 //         bias_type->addItem("Include");
 //         hLayout_1->addWidget(channel_no, 0, Qt::AlignTop);
 //         hLayout_1->addWidget(Power_down, 0, Qt::AlignTop);
 //         hLayout_1->addWidget(gain_combo, 0, Qt::AlignTop);
 //         hLayout_1->addWidget(input_type, 0, Qt::AlignTop);
 //         hLayout_1->addWidget(bias_type, 0, Qt::AlignTop);
 //         sendcmd_2->setText("Send");
 //         hLayout_1->addWidget(sendcmd_2, 0, Qt::AlignBottom);
 //         menuWidget->setLayout(hLayout_1);
 //         menuWidget->setStyleSheet("color: rgb(0,159,227);");
 //         menuWidget->setFixedHeight(100);
 //         menuWidget->setFixedWidth(500);

 //         menuWidget->setWindowTitle("Channel Settings");
 //         menuWidget->setVisible(true);

 //         connect(channel_no, &QComboBox::currentTextChanged, this,
 //                 &GraphAttributesWidget::Channel_config);
 //         connect(Power_down, &QComboBox::currentTextChanged, this,
 //                 &GraphAttributesWidget::Channel_config);
 //         connect(gain_combo, &QComboBox::currentTextChanged, this,
 //                 &GraphAttributesWidget::Channel_config);
 //         connect(input_type, &QComboBox::currentTextChanged, this,
 //                 &GraphAttributesWidget::Channel_config);
 //         connect(bias_type, &QComboBox::currentTextChanged, this,
 //                 &GraphAttributesWidget::Channel_config);
 //       }
	//else if (property == mNameProperty_12)
	//{
	//	mNameProperty_11->GetAttributeValue()->ConvertToString(Val[8]);
	//	m_bleInterface->write_data((QByteArray)Val[8]);
	//}
	else if (property == mNameProperty_9)
	{

		

	}
#endif
	UpdateInterface();
}
#ifdef INCLUDE_DEVICE_BRAINALIVE
// void GraphAttributesWidget::Channel_config()
//{
//  int cmd_data_2[10] = {0};
//  int temp = 0;
//  cmd_data_2[1] = channel_no->currentIndex();
//  if (cmd_data_2[1] == 1) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 1;
//  } else if (cmd_data_2[1] == 2) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 2;
//  } else if (cmd_data_2[1] == 3) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 3;
//  } else if (cmd_data_2[1] == 4) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 4;
//  } else if (cmd_data_2[1] == 5) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 5;
//  } else if (cmd_data_2[1] == 6) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 6;
//  } else if (cmd_data_2[1] == 7) {
//    ss_cmd[4] = 0;
//    ss_cmd[5] = 7;
//  } else if (cmd_data_2[1] == 8) {
//    ss_cmd[5] = 8;
//    ss_cmd[4] = 0;
//  } else if (cmd_data_2[1] == 9) {
//    ss_cmd[4] = 0x0f;
//    ss_cmd[5] = 0x0f;
//  }
//
//  cmd_data_2[2] = Power_down->currentIndex();
//  if (cmd_data_2[2] == 1)
//    ss_cmd[6] = 0;
//  else if (cmd_data_2[2] == 2)
//    ss_cmd[6] = 1;
//
//  cmd_data_2[3] = gain_combo->currentIndex();
//  if (cmd_data_2[3] == 1)
//    ss_cmd[7] = 1;
//  else if (cmd_data_2[3] == 2)
//    ss_cmd[7] = 2;
//  else if (cmd_data_2[3] == 3)
//    ss_cmd[7] = 3;
//  else if (cmd_data_2[3] == 4)
//    ss_cmd[7] = 4;
//  else if (cmd_data_2[3] == 5)
//    ss_cmd[7] = 0;
//  else if (cmd_data_2[3] == 6)
//    ss_cmd[7] = 5;
//  else if (cmd_data_2[3] == 7)
//    ss_cmd[7] = 6;
//
//  cmd_data_2[4] = input_type->currentIndex();
//  if (cmd_data_2[4] == 1)
//    ss_cmd[8] = 0;
//  else if (cmd_data_2[4] == 2)
//    ss_cmd[8] = 1;
//  else if (cmd_data_2[4] == 3)
//    ss_cmd[8] = 2;
//  else if (cmd_data_2[4] == 4)
//    ss_cmd[8] = 3;
//  else if (cmd_data_2[4] == 5)
//    ss_cmd[8] = 4;
//  else if (cmd_data_2[4] == 6)
//    ss_cmd[8] = 5;
//  else if (cmd_data_2[4] == 7)
//    ss_cmd[8] = 6;
//  else if (cmd_data_2[4] == 8)
//    ss_cmd[8] = 7;
//
//  cmd_data_2[5] = bias_type->currentIndex();
//  if (cmd_data_2[5] == 1)
//    ss_cmd[9] = 0;
//  else if (cmd_data_2[5] == 2)
//    ss_cmd[9] = 1;
//  temp = (ss_cmd[6] << 7) | (ss_cmd[7] << 4) | (ss_cmd[8] << 1) | ss_cmd[9];
//  ss_cmd[6] = temp / 0x10;
//  ss_cmd[7] = temp % 0x10;
//  connect(sendcmd_2, &QPushButton::clicked, this,
//          &GraphAttributesWidget::config_data);
//}
//
// void GraphAttributesWidget::config_data() {
//
//  ss_cmd[0] = '0';
//  ss_cmd[1] = 'a';
//  ss_cmd[2] = '8';
//  ss_cmd[3] = '1';
//
//  for (int i = 4; i < 8; i++) 
//  {
//    switch (ss_cmd[i]) {
//    case 0:
//      ss_cmd[i] = '0';
//      break;
//    case 1:
//      ss_cmd[i] = '1';
//      break;
//    case 2:
//      ss_cmd[i] = '2';
//      break;
//    case 3:
//      ss_cmd[i] = '3';
//      break;
//    case 4:
//      ss_cmd[i] = '4';
//      break;
//    case 5:
//      ss_cmd[i] = '5';
//      break;
//    case 6:
//      ss_cmd[i] = '6';
//      break;
//    case 7:
//      ss_cmd[i] = '7';
//      break;
//    case 8:
//      ss_cmd[i] = '8';
//      break;
//    case 9:
//      ss_cmd[i] = '9';
//      break;
//    case 10:
//      ss_cmd[i] = 'a';
//      break;
//    case 11:
//      ss_cmd[i] = 'b';
//      break;
//    case 12:
//      ss_cmd[i] = 'c';
//      break;
//    case 13:
//      ss_cmd[i] = 'd';
//      break;
//    case 14:
//      ss_cmd[i] = 'e';
//      break;
//    case 15:
//      ss_cmd[i] = 'f';
//      break;
//    }
//  }
//  ss_cmd[8] = '0';
//  ss_cmd[9] = 'd';
//  if (m_bleInterface->BLE_Satus() == true) {
//    m_bleInterface->write_data(ss_cmd);
//  }
//  //config_c = true;
//}
//
//void  GraphAttributesWidget::On_Ok()
//{
//	Ok_Button->setVisible(false);
//	mwidget_2->close();
//}
//void  GraphAttributesWidget::On_connect()
//{
//	if (mListWidget->currentRow() > (-1))
//	{
//		m_bleInterface->set_currentDevice(mListWidget->currentRow());
//		m_bleInterface->connectCurrentDevice();
//		if (m_bleInterface->BLE_Satus() == true)
//		{
//		//	m_bleInterface->m_deviceDiscoveryAgent->stop();
//			Connect->setVisible(false);
//			mScan_Widget->close();
//
//		}
//	}
//	else
//	{
//	
//		Ok_Button = new QPushButton();
//		Ok_Button->setText(" OK ");
//		mwidget_2 = new QWidget();
//		mwidget_2->setFixedHeight(100);
//		mwidget_2->setFixedWidth(250);
//		mListWidget_2 = new QListWidget();
//		QVBoxLayout* vLayout = new QVBoxLayout();
//		vLayout->setMargin(10);
//		vLayout->setSpacing(10);
//		mListWidget_2->setFixedHeight(80);
//		mListWidget_2->setFixedWidth(230);
//		mListWidget_2->addItem("Please Select Any Device");
//		vLayout->addWidget(mListWidget_2, 1, Qt::AlignTop);
//		vLayout->addWidget(Ok_Button, 1, Qt::AlignBottom);
//		mwidget_2->setLayout(vLayout);
//		mwidget_2->setVisible(true);
//
//		connect(Ok_Button, &QPushButton::clicked, this, &GraphAttributesWidget::On_Ok);
//		
//	}
//		
//
//}
#endif
// update the states
void GraphAttributesWidget::UpdateInterface()
{
	const uint32 numLinks = mAttributeLinks.Size();
	for (uint32 i=0; i<numLinks; ++i)
	{
		const AttributeLink& curLink = mAttributeLinks[i];
		if (curLink.mWidget == NULL)
			continue;

		GraphObject*			object				= curLink.mGraphObject;
		Property*				property			= curLink.mProperty;
		Attribute*				attribute			= object->GetAttributeValue( curLink.mAttributeIndex );
		AttributeSettings*		attributeSettings	= object->GetAttributeSettings( curLink.mAttributeIndex );
		const bool				isEnabled			= attributeSettings->IsEnabled();
		const bool				isVisible			= attributeSettings->IsVisible();

		curLink.mWidget->EnableWidgets( isEnabled );
		curLink.mWidget->SetValue( attribute );

		mPropertyTreeWidget->SetVisible( property, isVisible );
	}

	// update widget view
	mGraphObjectView->ReInit(mGraphObject);
}


void GraphAttributesWidget::AddAttributes(GraphObject* object, const char* parentGroupName, bool disableAttributes)
{
	mAttributeSet = object;

	QWidget* lastTablableWidget = NULL;

	// add all attributes
	Core::String labelString;
	const uint32 numAttributes = object->GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		// get the attribute and the corresponding attribute settings
		Core::Attribute*			attribute			= object->GetAttributeValue(i);
		Core::AttributeSettings*	attributeSettings	= object->GetAttributeSettings(i);

		// create the attribute and add it to the layout
		const bool isReadOnly = (attributeSettings->IsEnabled() == false) || (disableAttributes == true);
		AttributeWidget* attributeWidget = GetQtBaseManager()->GetAttributeWidgetFactory()->CreateAttributeWidget( attribute, attributeSettings, object, isReadOnly);
		Property* property = mPropertyTreeWidget->AddProperty( parentGroupName, attributeSettings->GetName(), attributeWidget, attribute, attributeSettings, false);
		mAttributeLinks.AddEmpty();
		mAttributeLinks.GetLast().mProperty = property;
		mAttributeLinks.GetLast().mAttributeIndex = i;
		mAttributeLinks.GetLast().mGraphObject = object;
		mAttributeLinks.GetLast().mWidget = attributeWidget;

		// set tab order
		QWidget* tablableWidget = attributeWidget->GetTablableWidget();

		if (lastTablableWidget != NULL && tablableWidget != NULL)
		{
			setTabOrder( lastTablableWidget, tablableWidget );
			lastTablableWidget = tablableWidget;
		}
		else if (lastTablableWidget == NULL && tablableWidget != NULL)
		{
			lastTablableWidget = tablableWidget;

			if (mNameProperty != NULL)
				setTabOrder( mNameProperty->GetAttributeWidget(), tablableWidget );
		}
	}
}


// find the index for the given button
uint32 GraphAttributesWidget::FindRemoveButtonIndex(QObject* button) const
{
	// for all table entries
	const uint32 numButtons = mRemoveButtonTable.Size();
	for (uint32 i=0; i<numButtons; ++i)
	{
		if (mRemoveButtonTable[i].mButton == button)	// this is button we search for
			return mRemoveButtonTable[i].mIndex;
	}

	return CORE_INVALIDINDEX32;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connections
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphAttributesWidget::InitForConnection(Connection* connection)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = connection;
	mAttributeSet = NULL;
	mNameProperty = NULL;
	
	// if there is no condition, leave
	if (connection == NULL)
		return;

	// add attributes
	bool disableAttributes = GetSession()->IsRunning() == true;
	AddAttributes( connection, mParentGroupName.AsChar(), disableAttributes );

	// add conditions only for state transition objects
	if (connection->GetBaseType() == StateTransition::BASE_TYPE &&
		connection->GetType() == StateTransition::TYPE_ID)
		AddConditions( connection, false );

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transition Conditions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add the conditions management for to a given layout
void GraphAttributesWidget::AddConditions(GraphObject* object, bool readOnly)
{
	if (object->GetType() != StateTransition::TYPE_ID)
		return;

	mRemoveButtonTable.Clear();

	const char* groupName = "Conditions";


	// create the add condition attribute widget and add it as property to the property widget
	ButtonAttributeWidget* addConditionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
	
	String conditionsGroupName;
	//conditionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	conditionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( conditionsGroupName.AsChar(), "Add Condition", addConditionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	QPushButton* addConditionButton = addConditionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addConditionButton, "/Images/Icons/Plus.png", "Add new state transition condition." );
	connect(addConditionButton, SIGNAL(clicked()), this, SLOT(OnAddConditionButtonClicked()));


	// convert the object into a state transition
	StateTransition* stateTransition = static_cast<StateTransition*>( object );

	// for all transition conditions
	String conditionName;
	String conditionGroupName;
	const uint32 numConditions = stateTransition->GetNumConditions();
	for (uint32 c=0; c<numConditions; ++c)
	{
		StateTransitionCondition* condition = stateTransition->GetCondition(c);

		// create the condition group name
		conditionName.Format( "#%i: %s", c, condition->GetReadableType() );
		
		//conditionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, conditionName.AsChar() );
		conditionGroupName.Format( "%s.%s", groupName, conditionName.AsChar() );

		// create the remove condition attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeConditionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( conditionGroupName.AsChar(), "Remove Condition", removeConditionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeConditionButton = removeConditionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeConditionButton, "/Images/Icons/Remove.png", "Remove condition from the transition." );
		connect(removeConditionButton, SIGNAL(clicked()), this, SLOT(OnRemoveCondition()));

		// add the remove button to the table, so we know this button would remove what condition
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeConditionButton;
		mRemoveButtonTable.GetLast().mIndex		= c;

		// add condition attributes
		AddAttributes( condition, conditionGroupName.AsChar(), readOnly );

		// expand the condition group
		mPropertyTreeWidget->SetExpanded( conditionGroupName.AsChar(), true );
	}

	// expand the conditions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );
}


// when we press the add conditions button
void GraphAttributesWidget::OnAddConditionButtonClicked()
{
	if (mGraphObject == NULL || mGraphObject->GetType() != StateTransition::TYPE_ID )
		return;

	// get the sender push button
	CORE_ASSERT( sender()->inherits("QPushButton") == true );
	QPushButton* addConditionButton = qobject_cast<QPushButton*>( sender() );

	// add context menu

	QMenu menu(this);
	menu.setVisible(false);

	// for all registered objects, add it in case the object is a condition
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 i=0; i<numObjects; ++i)
	{
		// get the object and skip in case the object is no action
		GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
		if (object->GetBaseType() != StateTransitionCondition::BASE_TYPE)
			continue;

		QAction* addAction = menu.addAction( object->GetReadableType(), this, SLOT(OnAddCondition()) );
		addAction->setData( object->GetType() );
		//addAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/PlusFolder.png") );
	}

	if (menu.isEmpty() == true)
		return;

	menu.exec( addConditionButton->mapToGlobal(addConditionButton->rect().bottomLeft()) );
	menu.setVisible(true);
}


void GraphAttributesWidget::OnAddCondition()
{
	// get the sender action
	CORE_ASSERT( sender()->inherits("QAction") == true );
	QAction* qAction= qobject_cast<QAction*>( sender() );

	// get the selected transition condition type
	const uint32 selectedConditionType = qAction->data().toInt();

	// add it to the transition
	StateTransition* transition = static_cast<StateTransition*>( mGraphObject );
	
	// create the new condition object
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeID( transition->GetStateMachine(), selectedConditionType );
	if (object == NULL)
		return;

	CORE_ASSERT( object->GetBaseType() == StateTransitionCondition::BASE_TYPE );

	// add the condition to the transition
	StateTransitionCondition* condition = static_cast<StateTransitionCondition*>(object);

	// add the condition to the transition
	transition->GetStateMachine()->AddCondition(transition, condition);

	// reinit the interface
	ForceReInit();
}


// when we press the remove condition button
void GraphAttributesWidget::OnRemoveCondition()
{
	if (mGraphObject == NULL || mGraphObject->GetType() != StateTransition::TYPE_ID )
		return;

	// find the condition to remove
	const uint32 index = FindRemoveButtonIndex( sender() );
	if (index == CORE_INVALIDINDEX32)
		return;

	GraphObject* transitionObject = mGraphObject;

	// clear the interface
	InitForGraphObject( NULL, true );

	// remove the condition
	StateTransition* transition = static_cast<StateTransition*>( transitionObject );
	transition->GetStateMachine()->RemoveCondition( transition,  mRemoveButtonTable[index].mIndex );

	// reinit the interface
	InitForGraphObject( transitionObject, true );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// State Actions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add the actions management for to a given layout
void GraphAttributesWidget::AddActions(GraphObject* object, bool readOnly)
{
	if (object->GetType() != ActionState::TYPE_ID)
		return;

	// convert the object into a action state
	ActionState* state = static_cast<ActionState*>(object);

	mRemoveButtonTable.Clear();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On Enter Actions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* groupName = "On-Enter Actions";

	// create the add action attribute widget and add it as property to the property widget
	ButtonAttributeWidget* addActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
	
	String actionsGroupName;
	//actionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	actionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( actionsGroupName.AsChar(), "Add Action", addActionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	QPushButton* addActionButton = addActionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addActionButton, "/Images/Icons/Plus.png", "Add new action." );
	connect(addActionButton, SIGNAL(clicked()), this, SLOT(OnAddEnterActionButtonClicked()));


	// for all state actions
	String actionName;
	String actionGroupName;
	uint32 numActions = state->GetOnEnterActions().GetNumActions();
	for (uint32 a=0; a<numActions; ++a)
	{
		Action* action = state->GetOnEnterActions().GetAction(a);

		// create the action group name
		actionName.Format( "#%i: %s", a, action->GetReadableType() );
		
		//actionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, actionName.AsChar() );
		actionGroupName.Format( "%s.%s", groupName, actionName.AsChar() );

		// create the remove action attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( actionGroupName.AsChar(), "Remove Action", removeActionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeActionButton = removeActionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeActionButton, "/Images/Icons/Remove.png", "Remove action." );
		connect(removeActionButton, SIGNAL(clicked()), this, SLOT(OnRemoveEnterAction()));

		// add the remove button to the table, so we know this button would remove what action
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeActionButton;
		mRemoveButtonTable.GetLast().mIndex		= a;

		// add action attributes
		AddAttributes( action, actionGroupName, readOnly );

		// expand the action group
		mPropertyTreeWidget->SetExpanded( actionGroupName.AsChar(), true );
	}

	// expand the actions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On Exit Actions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	groupName = "On-Exit Actions";

	// create the add action attribute widget and add it as property to the property widget
	addActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );

	//actionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	actionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( actionsGroupName.AsChar(), "Add Action", addActionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	addActionButton = addActionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addActionButton, "/Images/Icons/Plus.png", "Add new action." );
	connect(addActionButton, SIGNAL(clicked()), this, SLOT(OnAddExitActionButtonClicked()));

	// for all state actions
	numActions = state->GetOnExitActions().GetNumActions();
	for (uint32 a=0; a<numActions; ++a)
	{
		Action* action = state->GetOnExitActions().GetAction(a);

		// create the action group name
		actionName.Format( "#%i: %s", a, action->GetReadableType() );
		
		//actionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, actionName.AsChar() );
		actionGroupName.Format( "%s.%s", groupName, actionName.AsChar() );

		// create the remove action attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( actionGroupName.AsChar(), "Remove Action", removeActionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeActionButton = removeActionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeActionButton, "/Images/Icons/Remove.png", "Remove action." );
		connect(removeActionButton, SIGNAL(clicked()), this, SLOT(OnRemoveExitAction()));

		// add the remove button to the table, so we know this button would remove what action
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeActionButton;
		mRemoveButtonTable.GetLast().mIndex		= a;

		// add action attributes
		AddAttributes( action, actionGroupName, readOnly );

		// expand the action group
		mPropertyTreeWidget->SetExpanded( actionGroupName.AsChar(), true );
	}

	// expand the actions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );
}


// when we press the add actions button
void GraphAttributesWidget::OnAddActionButtonClicked(uint32 actionTypeIndex)
{
	if (mGraphObject == NULL || mGraphObject->GetType() != ActionState::TYPE_ID )
		return;

	// get the sender push button
	CORE_ASSERT( sender()->inherits("QPushButton") == true );
	QPushButton* addActionButton = qobject_cast<QPushButton*>( sender() );

	// add context menu

	QMenu menu(this);
	menu.setVisible(false);

	// for all registered objects, add it in case the object is an action
	String actionReadableType;
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 i=0; i<numObjects; ++i)
	{
		// get the object and skip in case the object is no action
		GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
		if (object->GetBaseType() != Action::BASE_TYPE)
			continue;

		QAction* addAction = NULL;

		// construct the readable type
		actionReadableType = object->GetReadableType();
		actionReadableType.RemoveAllParts(" Action");

		if (actionTypeIndex == 0)
			addAction = menu.addAction( actionReadableType.AsChar(), this, SLOT(OnAddEnterAction()) );
		else
			addAction = menu.addAction( actionReadableType.AsChar(), this, SLOT(OnAddExitAction()) );

		addAction->setData( object->GetType() );
		//addAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/PlusFolder.png") );
	}

	if (menu.isEmpty() == true)
		return;

	menu.exec( addActionButton->mapToGlobal(addActionButton->rect().bottomLeft()) );
	menu.setVisible(true);
}


// actually adds the action after using the button's context menu
void GraphAttributesWidget::OnAddAction(uint32 actionTypeIndex)
{
	// get the sender action
	CORE_ASSERT( sender()->inherits("QAction") == true );
	QAction* qAction= qobject_cast<QAction*>( sender() );

	// get the selected transition action type
	const uint32 actionType = qAction->data().toInt();

	ActionState* state = static_cast<ActionState*>(mGraphObject);
	
	// create the new action object
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeID( state->GetParent(), actionType );
	if (object == NULL)
		return;

	CORE_ASSERT( object->GetBaseType() == Action::BASE_TYPE );

	// add action to state
	Action* action = static_cast<Action*>(object);

	if (actionTypeIndex == 0)
		state->GetOnEnterActions().Add(action);
	else
		state->GetOnExitActions().Add(action);

	// reinit the interface
	ForceReInit();
}


// when we press the remove action button
void GraphAttributesWidget::OnRemoveAction(uint32 actionTypeIndex)
{
	if (mGraphObject == NULL || mGraphObject->GetType() != ActionState::TYPE_ID)
		return;

	// find the action to remove
	const uint32 index = FindRemoveButtonIndex( sender() );
	if (index == CORE_INVALIDINDEX32)
		return;

	GraphObject* stateObject = mGraphObject;

	// clear the interface
	InitForGraphObject( NULL, true );

	// remove the action
	ActionState* state = static_cast<ActionState*>(stateObject);

	if (actionTypeIndex == 0)
		state->GetOnEnterActions().RemoveByIndex( mRemoveButtonTable[index].mIndex );
	else
		state->GetOnExitActions().RemoveByIndex( mRemoveButtonTable[index].mIndex );

	// reinit the interface
	InitForGraphObject( stateObject, true );
}
