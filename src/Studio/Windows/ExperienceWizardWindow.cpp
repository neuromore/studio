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
#include <QHeaderView>
#include <QVBoxLayout>
#include <QIcon>
#include <Graph/ChannelSelectorNode.h>
#include <Backend/FileHierarchyGetRequest.h>
#include <Backend/FileHierarchyGetResponse.h>

using namespace Core;

// constructor
ExperienceWizardWindow::ExperienceWizardWindow(const User& user, QWidget* parent) :
   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
   mUser(user),
   mMainLayout(),
   mUserLayout(),
   mUserDesc("User:"),
   mUserLabel(user.CreateFullName().AsChar()),
   mClassifierLayout(),
   mClassifierSelectDesc("Classifier:"),
   mClassifierSelect(),
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

   /////////////////////////////////////////////////
   // user

   mUserDesc.setMinimumWidth(100);
   mUserLabel.setMinimumWidth(200);
   mUserLayout.setSpacing(6);
   mUserLayout.setAlignment(Qt::AlignCenter);
   mUserLayout.addWidget(&mUserDesc);
   mUserLayout.addWidget(&mUserLabel);
   mMainLayout.addLayout(&mUserLayout);

   /////////////////////////////////////////////////
   // classifier

   mClassifierSelectDesc.setMinimumWidth(100);
   mClassifierSelect.setMinimumWidth(200);
   mClassifierLayout.setSpacing(6);
   mClassifierLayout.setAlignment(Qt::AlignCenter);
   mClassifierLayout.addWidget(&mClassifierSelectDesc);
   mClassifierLayout.addWidget(&mClassifierSelect);
   mMainLayout.addLayout(&mClassifierLayout);

   connect(&mClassifierSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ExperienceWizardWindow::OnClassifierSelectIndexChanged);

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
   // TABLE ROWS DUMMY PART

   CreateRowChannelSelector("Inhibit");
   CreateRowChannelSelector("Augment");

   /////////////////////////////////////////////////
   // create button

   mCreateButton.setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"));
   mCreateButton.setToolTip("Create this protocol for selected user.");
   mMainLayout.addWidget(&mCreateButton);

   connect(&mCreateButton, &QPushButton::clicked, this, &ExperienceWizardWindow::OnCreateClicked);

   /////////////////////////////////////////////////
   // request backend data

   RequestClassifiers();

   /////////////////////////////////////////////////
   // finish

   GetQtBaseManager()->CenterToScreen(this);
   show();
}


// destructor
ExperienceWizardWindow::~ExperienceWizardWindow()
{
}

void ExperienceWizardWindow::OnClassifierSelectIndexChanged(int index)
{
   QString text(mClassifierSelect.itemText(index));
   QString id(mClassifierSelect.itemData(index).toString());

   // DEBUG
   printf("selected: %s %s \n", text.toLocal8Bit().data(), id.toLocal8Bit().data());

   // TODO: load from backend
}

void ExperienceWizardWindow::OnCreateClicked()
{
   //TODO
}

void ExperienceWizardWindow::RequestClassifiers()
{
   mClassifierSelect.setEnabled(false);
   mClassifierSelect.clear();
   mClassifierSelect.blockSignals(true);

   FileHierarchyGetRequest request(GetUser()->GetToken(), GetUser()->GetIdString());
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request, Request::UIMODE_SILENT);
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());

      FileHierarchyGetResponse response(networkReply);
      if (response.HasError() == true)
         return;

      const Core::Json& json = response.GetJson();

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

      mClassifierSelect.model()->sort(0);
      mClassifierSelect.setEnabled(true);
      mClassifierSelect.blockSignals(false);
      mClassifierSelect.setCurrentIndex(0);
   });
}

void ExperienceWizardWindow::ProcessFolder(const Json::Item& folder)
{
   const Json::Item nameItem = folder.Find("name");
   const Json::Item folderIdItem = folder.Find("folderId");
   const Json::Item attributesItem = folder.Find("attributes");

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

void ExperienceWizardWindow::CreateRowChannelSelector(const char* name)
{
   const uint32 row = mTableWidget.rowCount();
   mTableWidget.insertRow(row);

   //////////////////////////////////////////////////////////////////////////////////
   // 1: TYPE/ICON

   QLabel* lblIcon = new QLabel();
   lblIcon->setPixmap(QPixmap(":/Images/Graph/" + QString(ChannelSelectorNode::Uuid()) + ".png"));
   lblIcon->setFixedSize(64, 64);
   lblIcon->setScaledContents(true);
   
   QHBoxLayout* lblLayout = new QHBoxLayout();
   lblLayout->setAlignment(Qt::AlignCenter);
   lblLayout->addWidget(lblIcon);
   
   QWidget* lblWidget = new QWidget();
   lblWidget->setLayout(lblLayout);
   
   //////////////////////////////////////////////////////////////////////////////////
   // 2: NAME

   QTableWidgetItem* secondItem = new QTableWidgetItem(name);
   secondItem->setTextAlignment(Qt::AlignCenter);
   secondItem->setFlags(secondItem->flags() ^ Qt::ItemIsEditable);

   //////////////////////////////////////////////////////////////////////////////////
   // 3: EDIT

   QWidget* container = new QWidget();
   QVBoxLayout* vl = new QVBoxLayout(container);

   QListWidget* list = new QListWidget();
   list->setSpacing(0);
   list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   list->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

   CreateChannelSelectorListItem(*list, "Fpz", "Alpha");
   CreateChannelSelectorListItem(*list, "F3", "Beta1");
   CreateChannelSelectorListItem(*list, "F4", "Beta2");

   QHBoxLayout* hlnew = new QHBoxLayout();
   hlnew->setContentsMargins(0, 0, 0, 0);

   QComboBox* qboxch = new QComboBox();
   qboxch->addItem("Fpz");
   qboxch->addItem("F3");
   qboxch->addItem("Fz");
   qboxch->addItem("F4");
   qboxch->addItem("C3");
   qboxch->addItem("Cz");
   qboxch->addItem("C4");
   qboxch->addItem("Pz");

   QComboBox* qboxband = new QComboBox();
   qboxband->addItem("Alpha");
   qboxband->addItem("Alpha/Theta");
   qboxband->addItem("Beta1");
   qboxband->addItem("Beta2");
   qboxband->addItem("Beta3");
   qboxband->addItem("Delta");
   qboxband->addItem("SMR");
   qboxband->addItem("Theta");
   qboxband->addItem("Theta/Beta");

   QPushButton* qbtnadd = new QPushButton();
   qbtnadd->setToolTip("Add this combination");
   qbtnadd->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"));

   hlnew->addWidget(qboxch);
   hlnew->addWidget(qboxband);
   hlnew->addWidget(qbtnadd);

   vl->addWidget(list);
   vl->addLayout(hlnew);

   //////////////////////////////////////////////////////////////////////////////////

   mTableWidget.setCellWidget(row, 0, lblWidget);
   mTableWidget.setItem(      row, 1, secondItem);
   mTableWidget.setCellWidget(row, 2, container);
}

void ExperienceWizardWindow::CreateChannelSelectorListItem(QListWidget& list, const char* channel, const char* band)
{
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

   // configure delete button
   btnDelete->setToolTip("Remove this combination");
   btnDelete->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Minus.png"));

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
