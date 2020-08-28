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
#include "ExperienceWizardWindow.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <Backend/BackendHelpers.h>
#include <Backend/FileHierarchyGetRequest.h>
#include <Backend/FileHierarchyGetResponse.h>
#include <Backend/FilesGetRequest.h>
#include <Backend/FilesGetResponse.h>
#include <Backend/FilesCreateRequest.h>
#include <Backend/FilesCreateResponse.h>
#include <Graph/ChannelSelectorNode.h>
#include <Engine/Devices/eemagine/eemagineNodes.h>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QIcon>
#include <QMessageBox>

#define COLUMN_IDX_TYPE 0
#define COLUMN_IDX_NAME 1
#define COLUMN_IDX_EDIT 2

using namespace Core;

// constructor
ExperienceWizardWindow::ExperienceWizardWindow(const User& user, QWidget* parent) :
   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
   mUser(user),
   mFolderId(),
   mEegNode(0),
   mClassifier(0),
   mMainLayout(),
   mHeaderLayout(),
   mUserLayout(),
   mUserDesc("User:"),
   mUserLabel(user.CreateFullName().AsChar()),
   mClassifierLayout(),
   mClassifierSelectDesc("Classifier:"),
   mClassifierSelect(),
   mStateMachineLayout(),
   mStateMachineSelectDesc("State Machine:"),
   mStateMachineSelect(),
   mExperienceLayout(),
   mExperienceDesc("Experience:"),
   mExperienceEdit(),
   mSupportedDevicesDesc("Supported EEG:"),
   mSupportedDevicesList(),
   mTableWidget(),
   mHeaderType("Type"),
   mHeaderName("Name"),
   mHeaderEdit("Edit"),
   mCreateButton("Create")
{
   // set the window title
   setWindowTitle("Experience Wizard");
   setWindowIcon(GetQtBaseManager()->FindIcon("Images/Icons/Users.png"));
   setMinimumWidth(600);
   setMinimumHeight(400);
   setModal(true);
   setWindowModality(Qt::ApplicationModal);

   // add the main vertical layout
   setLayout(&mMainLayout);

   // setup the header layout
   mHeaderLayout.setSpacing(6);
   mHeaderLayout.setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
   mHeaderLayout.setAlignment(Qt::AlignLeft);
   mHeaderLayout.addLayout(&mHeaderLeftLayout);
   mHeaderLayout.addLayout(&mHeaderRightLayout);

   mHeaderLeftLayout.setAlignment(Qt::AlignLeft);
   mHeaderRightLayout.setAlignment(Qt::AlignLeft);

   // add header to main layout
   mMainLayout.addLayout(&mHeaderLayout);

   /////////////////////////////////////////////////
   // user

   mUserDesc.setMinimumWidth(100);
   mUserLabel.setMinimumWidth(200);
   mUserLayout.setSpacing(6);
   mUserLayout.setAlignment(Qt::AlignCenter);
   mUserLayout.addWidget(&mUserDesc);
   mUserLayout.addWidget(&mUserLabel);
   mHeaderLeftLayout.addLayout(&mUserLayout);

   /////////////////////////////////////////////////
   // classifier

   mClassifierSelectDesc.setMinimumWidth(100);
   mClassifierSelect.setMinimumWidth(200);
   mClassifierLayout.setSpacing(6);
   mClassifierLayout.setAlignment(Qt::AlignCenter);
   mClassifierLayout.addWidget(&mClassifierSelectDesc);
   mClassifierLayout.addWidget(&mClassifierSelect);
   mHeaderLeftLayout.addLayout(&mClassifierLayout);

   connect(&mClassifierSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ExperienceWizardWindow::OnClassifierSelectIndexChanged);

   /////////////////////////////////////////////////
   // state machine

   mStateMachineSelectDesc.setMinimumWidth(100);
   mStateMachineSelect.setMinimumWidth(200);
   mStateMachineLayout.setSpacing(6);
   mStateMachineLayout.setAlignment(Qt::AlignCenter);
   mStateMachineLayout.addWidget(&mStateMachineSelectDesc);
   mStateMachineLayout.addWidget(&mStateMachineSelect);
   mHeaderLeftLayout.addLayout(&mStateMachineLayout);

   connect(&mStateMachineSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ExperienceWizardWindow::OnStateMachineSelectIndexChanged);

   /////////////////////////////////////////////////
   // experience

   mExperienceDesc.setMinimumWidth(100);
   mExperienceDesc.setMaximumWidth(100);
   mExperienceEdit.setMinimumWidth(200);
   mExperienceEdit.setMaximumWidth(200);
   mExperienceLayout.setSpacing(6);
   mExperienceLayout.setAlignment(Qt::AlignCenter);
   mExperienceLayout.addWidget(&mExperienceDesc);
   mExperienceLayout.addWidget(&mExperienceEdit);
   mHeaderLeftLayout.addLayout(&mExperienceLayout);

   connect(&mExperienceEdit, &QLineEdit::textChanged, this, &ExperienceWizardWindow::OnExperienceTextChanged);

   /////////////////////////////////////////////////
   // supported devices

   mSupportedDevicesDesc.setFixedHeight(20);
   mSupportedDevicesDesc.setMinimumWidth(50);
   mSupportedDevicesDesc.setAlignment(Qt::AlignLeft);
   mSupportedDevicesList.setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
   mSupportedDevicesList.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
   mSupportedDevicesList.setFixedHeight(75);

   mHeaderRightLayout.addWidget(&mSupportedDevicesDesc);
   mHeaderRightLayout.addWidget(&mSupportedDevicesList);

   /////////////////////////////////////////////////
   // table

   mMainLayout.addWidget(&mTableWidget);
   mTableWidget.setEnabled(true);

   // columns
   mTableWidget.setColumnCount(3);
   mTableWidget.setColumnWidth(0, 80);
   mTableWidget.setColumnWidth(1, 120);

   // header
   mHeaderType.setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
   mHeaderName.setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
   mHeaderEdit.setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
   mTableWidget.setHorizontalHeaderItem(0, &mHeaderType);
   mTableWidget.setHorizontalHeaderItem(1, &mHeaderName);
   mTableWidget.setHorizontalHeaderItem(2, &mHeaderEdit);

   // tweaks
   //mTableWidget.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
   mTableWidget.horizontalHeader()->setStretchLastSection(true);
   mTableWidget.horizontalHeader()->show();

   // don't show the vertical header
   mTableWidget.verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
   mTableWidget.verticalHeader()->setDefaultSectionSize(128);

   mTableWidget.verticalHeader()->hide();

   // complete row selection
   mTableWidget.setSelectionBehavior(QAbstractItemView::SelectRows);
   mTableWidget.setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
   mTableWidget.setFocusPolicy(Qt::NoFocus);
   mTableWidget.setAlternatingRowColors(false);
   mTableWidget.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

   /////////////////////////////////////////////////
   // create button

   mCreateButton.setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"));
   mCreateButton.setToolTip("Create this protocol for selected user.");
   mMainLayout.addWidget(&mCreateButton);

   connect(&mCreateButton, &QPushButton::clicked, this, &ExperienceWizardWindow::OnCreateClicked);

   SyncCreateButton();

   /////////////////////////////////////////////////
   // request backend file hierarchy

   RequestFileHierarchy();
   
   /////////////////////////////////////////////////
   // finish

   GetQtBaseManager()->CenterToScreen(this);
   show();
}


// destructor
ExperienceWizardWindow::~ExperienceWizardWindow()
{
   mQuickConfigNodes.Clear();
   mEegDevices.Clear();
   mEegDevicesRemoved.Clear();
   mEegNode = 0;

   if (mClassifier)
   {
      delete mClassifier;
      mClassifier = 0;
   }
}

void ExperienceWizardWindow::OnClassifierSelectIndexChanged(int index)
{
   if (index < 0 || index >= mClassifierSelect.count())
      return;

   // load classifier from backend
   FilesGetRequest request(GetUser()->GetToken(), GetClassifierId().toLocal8Bit().data());
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request, Request::UIMODE_SILENT);
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());

      FilesGetResponse response(networkReply);
      if (response.HasError() == true)
         return;

      // clear old node refs
      mQuickConfigNodes.Clear();
      mEegDevices.Clear();
      mEegDevicesRemoved.Clear();
      mEegNode = 0;

      // parse the json into classifier instance
      if (mClassifier)
         delete mClassifier;

      mClassifier = new Classifier();
      mGraphImporter.LoadFromString(response.GetJsonContent(), mClassifier);

      const uint32 numNodes = mClassifier->GetNumNodes();
      for (uint32_t i = 0; i < numNodes; i++)
      {
         Node* n = mClassifier->GetNode(i);

         //TODO: Check directly for DeviceInputNode
         switch (n->GetType())
         {
         case EegDeviceNode::TYPE_ID:
         case eemagine8Node::TYPE_ID:
         case eemagine32Node::TYPE_ID:
         case eemagine64Node::TYPE_ID:
            mEegNode = (DeviceInputNode*)n;
            break;
         }

         // iterate attributes and look for the quick config setting
         const uint32 numAtt = n->GetNumAttributes();
         for (uint32_t j = 0; j < numAtt; j++)
         {
            Core::AttributeSettings* settings = n->GetAttributeSettings(j);
            Core::Attribute* attrib = n->GetAttributeValue(j);
            Core::String attribv;
            
            if (settings->GetInternalNameString() == "quickconfig" &&
                attrib->ConvertToString(attribv) && attribv == "1")
            {
               mQuickConfigNodes.Add(n);
               break;
            }
         }
      }

      // if found an eeg node above
      if (mEegNode)
      {
         // try get prototype device for found bci node device type
         // this works for explicit eeg device nodes only
         if (BciDevice* eeg = (BciDevice*)GetDeviceManager()->GetRegisteredDeviceType(mEegNode->GetDeviceType()))
            mEegDevices.Add(eeg);
         
         // handle the generic EegNode
         else if (mEegNode->GetType() == EegDeviceNode::TYPE_ID)
         {
            Core::Array<Core::String> devstr;
            devstr = mEegNode->GetStringArrayAttributeByName("allowedDevices", devstr);

            const uint32_t numDevStr = devstr.Size();
            for (uint32 i = 0; i < numDevStr; i++)
               GetDeviceManager()->GetRegisteredDeviceTypeByHardwareNamePrefix<BciDevice>(devstr[i], mEegDevices);
         }
      }

      // set ui from nodes/data
      SyncUi();
      SyncCreateButton();
   });
}

void ExperienceWizardWindow::OnStateMachineSelectIndexChanged(int index)
{
   if (index < 0 || index >= mStateMachineSelect.count())
      return;

   SyncCreateButton();
}

void ExperienceWizardWindow::OnExperienceTextChanged(const QString& text)
{
   SyncCreateButton();
}

void ExperienceWizardWindow::OnCreateClicked()
{
   if (!mClassifier)
      return;

   // Experience Generation

   Experience    exp; // experience to create
   GraphSettings set; // classifier settings to use

   // set name, classifier and state machine
   exp.SetName(mExperienceEdit.text().toLocal8Bit().data());
   exp.SetClassifierUuid(this->GetClassifierId().toLocal8Bit().data());
   exp.SetStateMachineUuid(this->GetStateMachineId().toLocal8Bit().data());

   mClassifier->CreateSettings(set, true); // create settings from (modified) classifier
   exp.SetClassifierSettings(set);         // and set them on experience

   // Serialization

   Core::Json   json;  // exp as json
   Core::String jsons; // exp as json string

   Core::Json::Item rootItem = json.GetRootItem();

   exp.Save(json, rootItem);
   json.WriteToString(jsons);

   // Store on Backend

   FilesCreateRequest request(GetUser()->GetToken(), exp.GetName(), mFolderId.AsChar(), "EXPERIENCE", jsons);
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request, Request::UIMODE_SILENT);
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());
      FilesCreateResponse response(networkReply);

      // something went wrong
      if (response.HasError() == true)
      {
         QMessageBox::warning(NULL, "Error", "Failed to create experience.", QMessageBox::Ok);
         return;
      }

      // done, signal and close
      else
         emit(OnExperienceCreated(mUser));
         close();
   });
}

void ExperienceWizardWindow::RequestFileHierarchy()
{
   // clear and disable classifier combobox
   mClassifierSelect.setEnabled(false);
   mClassifierSelect.clear();
   mClassifierSelect.blockSignals(true);

   // clear and disable state machine combobox
   mStateMachineSelect.setEnabled(false);
   mStateMachineSelect.clear();
   mStateMachineSelect.blockSignals(true);

   FileHierarchyGetRequest request(GetUser()->GetToken(), mUser.GetIdString());
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request, Request::UIMODE_SILENT);
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());

      FileHierarchyGetResponse response(networkReply);
      if (response.HasError() == true)
         return;

      const Core::Json& json = response.GetJson();

      // walk file hierarchy
      Json::Item rootItem = json.GetRootItem();
      if (rootItem.IsNull() == false)
      {
         Json::Item dataItem = rootItem.Find("data");
         if (dataItem.IsNull() == false)
         {
            Json::Item rootFoldersItem = dataItem.Find("rootFolders");
            if (rootFoldersItem.IsArray() == true)
            {
               const uint32 numRootFolders = rootFoldersItem.Size();
               for (uint32 i = 0; i < numRootFolders; ++i)
                  ProcessFolder(rootFoldersItem[i]);
            }
         }
      }

      // show warning about not found user/home folder
      if (mFolderId == "")
         QMessageBox::warning(NULL, "Warning", "No homefolder was found for this user.", QMessageBox::Ok);

      // sort and enable classifier combobox
      mClassifierSelect.model()->sort(0);
      mClassifierSelect.setEnabled(true);
      mClassifierSelect.blockSignals(false);

      // set index or trigger manually
      if (mClassifierSelect.currentIndex() == 0) 
         OnClassifierSelectIndexChanged(0);
      else 
         mClassifierSelect.setCurrentIndex(0);

      // sort and enable statemachine combobox
      mStateMachineSelect.model()->sort(0);
      mStateMachineSelect.setEnabled(true);
      mStateMachineSelect.blockSignals(false);

      if (mStateMachineSelect.currentIndex() == 0)
         OnStateMachineSelectIndexChanged(0);
      else
         mStateMachineSelect.setCurrentIndex(0);
   });
}

void ExperienceWizardWindow::ProcessFolder(const Json::Item& folder)
{
   const Json::Item nameItem = folder.Find("name");
   const Json::Item folderIdItem = folder.Find("folderId");
   const Json::Item attributesItem = folder.Find("attributes");

   // look for private/home folder of user
   if (mUser.GetIdString() == nameItem.GetString())
      mFolderId = folderIdItem.GetString();

   // iterate files in this folder
   Json::Item filesItem = folder.Find("files");
   if (filesItem.IsArray() == true)
   {
      const uint32 numFiles = filesItem.Size();
      for (uint32 i = 0; i < numFiles; ++i)
      {
         // find attributes
         Json::Item nameItem = filesItem[i].Find("name");
         Json::Item typeItem = filesItem[i].Find("type");
         Json::Item fileIdItem = filesItem[i].Find("fileId");
         Json::Item revisionItem = filesItem[i].Find("revision");

         // convert to QtStrings
         QString name(nameItem.GetString());
         QString type(typeItem.GetString());
         QString id(fileIdItem.GetString());

         // add classifiers to combobox
         if (type == "CLASSIFIER")
            mClassifierSelect.addItem(name, id);

         // add statemachines to combobox
         else if (type == "STATEMACHINE")
            mStateMachineSelect.addItem(name, id);
      }
   }

   // recursively add subfolders
   Json::Item foldersItem = folder.Find("folders");
   if (foldersItem.IsArray() == true)
   {
      const uint32 numFolders = foldersItem.Size();
      for (uint32 i = 0; i < numFolders; ++i)
         ProcessFolder(foldersItem[i]);
   }
}

void ExperienceWizardWindow::SyncNodes()
{
   // get count of rows and nodes
   const int numRows  = mTableWidget.rowCount();
   const int numNodes = (int)mQuickConfigNodes.Size();

   // clear used channels (re-populated in ReadChannelSelectorRow)
   mChannelsUsed.Clear();

   // something wrong here (add logging..)
   if (numRows != numNodes)
      return;

   // iterate and sync ui to quick config nodes
   for (int i = 0; i < numRows; i++)
   {
      Node* n = mQuickConfigNodes.GetItem((uint32_t)i);

      // call specific handler for node type
      switch (n->GetType())
      {
      case ChannelSelectorNode::TYPE_ID:
         ReadChannelSelectorRow(i);
         break;
      default:
         break;
      }
   }

   // add all removed devices and clear them before check
   mEegDevices.Add(mEegDevicesRemoved);
   mEegDevicesRemoved.Clear();

   // remove devices that do not supported selected channels
   const uint32 numDevs = mEegDevices.Size();
   if (numDevs > 0)
   {
      // iterade devices backwards due to remove
      for (uint32 i = numDevs - 1; i != UINT32_MAX; i--)
      {
         const auto& electrodes = mEegDevices[i]->GetElectrodes();
         const uint32 numElectrodes = electrodes.Size();
         const uint32 numChannels = mChannelsUsed.Size();

         // check each used channel with this device
         for (uint32 k = 0; k < numChannels; k++)
         {
            bool found = false;

            // check if an electrode matches the channel
            for (uint32 j = 0; j < numElectrodes; j++)
            {
               if (electrodes[j].GetNameString() == mChannelsUsed[k])
               {
                  found = true;
                  break;
               }
            }

            // device doesn't have that channel, remove it
            if (!found)
            {
               mEegDevicesRemoved.Add(mEegDevices[i]);
               mEegDevices.Remove(i);
               break;
            }
         }
      }
   }

   SyncUi(); // needed
}

void ExperienceWizardWindow::SyncUi()
{
   // set supported devices list
   mSupportedDevicesList.clear();
   const uint32 numDevices = mEegDevices.Size();
   for (uint32 i = 0; i < numDevices; i++)
      mSupportedDevicesList.addItem(mEegDevices[i]->GetHardwareName());

   // clear old ui table
   mTableWidget.setRowCount(0);

   // iterate found quick config nodes
   const uint32 size = mQuickConfigNodes.Size();
   for (uint32 i = 0; i < size; i++)
   {
      Node* n = mQuickConfigNodes.GetItem(i);

      // create table row
      const uint32 row = mTableWidget.rowCount();
      mTableWidget.insertRow(row);

      // column: TYPE/ICON
      QLabel* lblIcon = new QLabel();
      QHBoxLayout* lblLayout = new QHBoxLayout();
      QWidget* lblWidget = new QWidget();

      lblIcon->setFixedSize(64, 64);
      lblIcon->setScaledContents(true);
      lblLayout->setAlignment(Qt::AlignCenter);
      lblLayout->addWidget(lblIcon);
      lblWidget->setLayout(lblLayout);

      // column: NAME
      QTableWidgetItem* secondItem = new QTableWidgetItem(n->GetName());
      secondItem->setTextAlignment(Qt::AlignCenter);
      secondItem->setFlags(secondItem->flags() ^ Qt::ItemIsEditable);

      // column: EDIT
      QWidget* container = new QWidget();
      switch (n->GetType())
      {
      case ChannelSelectorNode::TYPE_ID:
         lblIcon->setPixmap(QPixmap(":/Images/Graph/" + QString(ChannelSelectorNode::Uuid()) + ".png"));
         CreateChannelSelectorEditColumn(n, container);
         break;
      }

      // add them
      mTableWidget.setCellWidget(row, COLUMN_IDX_TYPE, lblWidget);
      mTableWidget.setItem(row, COLUMN_IDX_NAME, secondItem);
      mTableWidget.setCellWidget(row, COLUMN_IDX_EDIT, container);
   }
}

void ExperienceWizardWindow::SyncCreateButton()
{
   if (mExperienceEdit.text() == "")
   {
      mCreateButton.setText("Set name of experience");
      mCreateButton.setEnabled(false);
   }
   else if (this->GetClassifierId() == "")
   {
      mCreateButton.setText("Select a classifier");
      mCreateButton.setEnabled(false);
   }
   else if (this->GetStateMachineId() == "")
   {
      mCreateButton.setText("Select a state machine");
      mCreateButton.setEnabled(false);
   }
   else if (mEegDevices.Size() == 0)
   {
      mCreateButton.setText("No supported EEG device");
      mCreateButton.setEnabled(false);
   }
   else
   {
      mCreateButton.setText("Create");
      mCreateButton.setEnabled(true);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANNEL SELECTOR
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExperienceWizardWindow::ReadChannelSelectorRow(int idx)
{
   Node*        n = mQuickConfigNodes.GetItem((uint32_t)idx);
   QWidget*     w = mTableWidget.cellWidget(idx, COLUMN_IDX_EDIT);
   QListWidget* l = w->findChild<QListWidget*>("List");
   uint32_t     a = n->FindAttributeIndexByInternalName("channels");
   
   Core::String s;
   
   // build channels attribute string from ui elements
   const uint32 numItems = l->count();
   for (uint32 j = 0; j < numItems; j++)
   {
      // find widgets
      QListWidgetItem* lwi = l->item(j);
      QWidget* w = l->itemWidget(lwi);
      QLabel* c = w->findChild<QLabel*>("Channel");
      QLabel* b = w->findChild<QLabel*>("Band");

      // build string
      s += b->text().toLocal8Bit().data();
      s += ' ';
      s += c->text().toLocal8Bit().data();
      if (j < numItems - 1)
         s += ',';

      // add this channel to list of all used channels if not listed yet
      if (!mChannelsUsed.Contains(c->text().toLocal8Bit().data()))
         mChannelsUsed.Add(c->text().toLocal8Bit().data());
   }

   // set node channels attribute value
   if (!n->GetAttributeValue(a)->InitFromString(s))
   {
      // TODO: something failed
   }
}

void ExperienceWizardWindow::CreateChannelSelectorEditColumn(Node* node, QWidget* container)
{
   //////////////////////////////////////////////////////////////////////////////////
   // create ui elements

   QVBoxLayout* vl = new QVBoxLayout(container);
   QListWidget* list = new QListWidget();
   QHBoxLayout* hlnew = new QHBoxLayout();
   QComboBox*   qboxch = new QComboBox();
   QComboBox*   qboxband = new QComboBox();
   QPushButton* qbtnadd = new QPushButton();

   // setup list
   list->setObjectName("List");
   list->setSpacing(0);
   list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   list->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

   // no margin
   hlnew->setContentsMargins(0, 0, 0, 0);

   // build selectable electrodes
   Core::Array<Core::String> qboxchitems;
   const uint32 numDevices = mEegDevices.Size();
   if (numDevices > 0)
   {
      for (uint32 j = 0; j < numDevices; j++)
      {
         const uint32_t numElectrodes = mEegDevices[j]->GetNumNeuroSensors();
         for (uint32_t i = 0; i < numElectrodes; i++)
            if (!qboxchitems.Contains(mEegDevices[j]->GetNeuroSensor(i)->GetName()))
               qboxchitems.Add(mEegDevices[j]->GetNeuroSensor(i)->GetName());
      }
      for (uint32 i = 0; i < qboxchitems.Size(); i++)
         qboxch->addItem(qboxchitems[i].AsChar());
   }
   else
   {
      // list all known electrodes if no bci device
      const uint32_t numElectrodes = GetEEGElectrodes()->GetNumElectrodes();
      for (uint32_t i = 0; i < numElectrodes; i++)
         qboxch->addItem(GetEEGElectrodes()->GetElectrode(i).GetName());
   }

   // sort alphabetically
   qboxch->model()->sort(0);

   // configure combobox band
   qboxband->addItem("Alpha");
   qboxband->addItem("Beta1");
   qboxband->addItem("Beta Sum");
   qboxband->addItem("CustomBand1");
   qboxband->addItem("CustomBand2");
   qboxband->addItem("CustomBand3");
   qboxband->addItem("Delta");
   qboxband->addItem("DeltaThetaSum");
   qboxband->addItem("Gamma");
   qboxband->addItem("High Beta");
   qboxband->addItem("Low Beta");
   qboxband->addItem("SMR");
   qboxband->addItem("Theta");
   qboxband->addItem("ThetaAlphaSum");

   // configure add button
   qbtnadd->setToolTip("Add this combination");
   qbtnadd->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"));
   qbtnadd->setProperty("List", QVariant::fromValue((void*)list));
   qbtnadd->setProperty("Channel", QVariant::fromValue((void*)qboxch));
   qbtnadd->setProperty("Band", QVariant::fromValue((void*)qboxband));

   // handler for add button
   connect(qbtnadd, &QPushButton::clicked, this, &ExperienceWizardWindow::OnChannelSelectorListItemAdd);

   // widgets setup
   hlnew->addWidget(qboxch);
   hlnew->addWidget(qboxband);
   hlnew->addWidget(qbtnadd);
   vl->addWidget(list);
   vl->addLayout(hlnew);

   //////////////////////////////////////////////////////////////////////////////////
   // add internal list items

   const uint32_t attidx = node->FindAttributeIndexByInternalName("channels");

   // channels attribute not found
   if (attidx == CORE_INVALIDINDEX32)
   {
      //TODO log
      return;
   }

   // get the attribute
   Core::Attribute* att = node->GetAttributeValue(attidx);

   // not expected type
   if (att->GetType() != AttributeStringArray::TYPE_ID)
   {
      //TODO log
      return;
   }

   // always mark it as changed (to get into exp json)
   node->OnAttributeChanged(att);

   // cast to expected type
   AttributeStringArray* channels = (AttributeStringArray*)att;

   // iterate channels
   const uint32_t numCh = channels->GetNumStrings();
   for (uint32_t i = 0; i < numCh; i++)
   {
      Core::String& s = channels->GetString(i);

      // remove some spaces before split by space
      s.Trim();

      // split channel words by whitespaces
      auto words = s.Split(StringCharacter::space);

      // read channel and band parts from it
      if (words.Size() >= 2)
      {
         const uint32_t last = words.Size() - 1;

         // last word is channel
         Core::String ch = words[last];

         // others belong to band
         Core::String band;
         for (uint32_t j = 0; j < last; j++)
         {
            band += words[j];
            if (j < last - 1)
               band += ' ';
         }

         // create the item
         CreateChannelSelectorListItem(*list, ch, band);
      }
   }
}

void ExperienceWizardWindow::CreateChannelSelectorListItem(QListWidget& list, const char* channel, const char* band)
{
   // avoid duplicates
   if (HasChannelSelectorListItem(list, channel, band))
      return;

   // create the list item and its internal widget/layout
   QListWidgetItem* item   = new QListWidgetItem();
   QWidget*         widget = new QWidget();
   QHBoxLayout*     layout = new QHBoxLayout(widget);

   // no margin
   layout->setContentsMargins(0, 0, 0, 0);

   // create the internal widgets
   QLabel*      lblChannel = new QLabel(channel);
   QLabel*      lblBand    = new QLabel(band);
   QPushButton* btnDelete  = new QPushButton();

   // set names for lookup
   lblChannel->setObjectName("Channel");
   lblBand->setObjectName("Band");
   btnDelete->setObjectName("Delete");

   // configure delete button
   btnDelete->setToolTip("Remove this combination");
   btnDelete->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Minus.png"));
   btnDelete->setProperty("ListWidgetItem", QVariant::fromValue((void*)item));

   // link delete button click event
   connect(btnDelete, &QPushButton::clicked, this, &ExperienceWizardWindow::OnChannelSelectorListItemDelete);

   // add everything to layout
   layout->addWidget(lblChannel);
   layout->addWidget(lblBand);
   layout->addWidget(btnDelete);
   layout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

   // set item size from widget size
   item->setSizeHint(widget->sizeHint());

   // add it to the list
   list.addItem(item);
   list.setItemWidget(item, widget);
}

bool ExperienceWizardWindow::HasChannelSelectorListItem(QListWidget& list, const char* channel, const char* band)
{
   const uint32 numItems = list.count();
   for (uint32 i = 0; i < numItems; i++)
   {
      // find widgets
      QListWidgetItem* lwi = list.item(i);
      QWidget*         w   = list.itemWidget(lwi);
      QLabel*          c   = w->findChild<QLabel*>("Channel");
      QLabel*          b   = w->findChild<QLabel*>("Band");

      // match
      if (c && b && c->text() == channel && b->text() == band)
         return true;
   }
   return false;
}

void ExperienceWizardWindow::OnChannelSelectorListItemAdd()
{
   if (!sender())
      return;

   // get button, list and ch+band
   QPushButton* btn  = qobject_cast<QPushButton*>(sender());
   QListWidget* list = (QListWidget*)btn->property("List").value<void*>();
   QComboBox*   ch   = (QComboBox*)btn->property("Channel").value<void*>();
   QComboBox*   band = (QComboBox*)btn->property("Band").value<void*>();

   // create entry in list
   CreateChannelSelectorListItem(*list, ch->currentText().toLocal8Bit().data(), band->currentText().toLocal8Bit().data());

   // update nodes from ui
   SyncNodes();
}

void ExperienceWizardWindow::OnChannelSelectorListItemDelete()
{
   if (!sender())
      return;

   // get button and listitem
   QPushButton*     b = qobject_cast<QPushButton*>(sender());
   QListWidgetItem* w = (QListWidgetItem*)b->property("ListWidgetItem").value<void*>();

   // delete entry from list
   delete w;

   // update nodes from ui
   SyncNodes();
}
