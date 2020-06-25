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
#include <Backend/UsersGetRequest.h>
#include <Backend/UsersGetResponse.h>
#include <Backend/BackendHelpers.h>
#include <QHeaderView>
#include <QVBoxLayout>
//#include <QScrollBar>

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
   mClassifierSelect.addItem("Classifier1"); // TODO
   mClassifierSelect.addItem("Classifier2"); // TODO
   mClassifierSelect.addItem("Classifier3"); // TODO
   mClassifierLayout.setSpacing(6);
   mClassifierLayout.setAlignment(Qt::AlignCenter);
   mClassifierLayout.addWidget(&mClassifierSelectDesc);
   mClassifierLayout.addWidget(&mClassifierSelect);
   mMainLayout.addLayout(&mClassifierLayout);

   connect(&mClassifierSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ExperienceWizardWindow::OnClassifierSelectIndexChanged);

   /////////////////////////////////////////////////
   // table

   mMainLayout.addWidget(&mTableWidget);
   mTableWidget.setEnabled(false);

   // columns
   mTableWidget.setColumnCount(3);

   // header
   mHeaderType.setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
   mHeaderName.setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
   mHeaderEdit.setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
   mTableWidget.setHorizontalHeaderItem(0, &mHeaderType);
   mTableWidget.setHorizontalHeaderItem(1, &mHeaderName);
   mTableWidget.setHorizontalHeaderItem(2, &mHeaderEdit);

   // tweaks
   mTableWidget.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   mTableWidget.horizontalHeader()->show();

   // don't show the vertical header
   mTableWidget.verticalHeader()->hide();

   // complete row selection
   mTableWidget.setSelectionBehavior(QAbstractItemView::SelectRows);
   mTableWidget.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
   mTableWidget.setFocusPolicy(Qt::NoFocus);	 // get rid of rectangular selection box around single cells
   mTableWidget.setAlternatingRowColors(true);

   /////////////////////////////////////////////////
   // create button

   mCreateButton.setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"));
   mCreateButton.setToolTip("Create this protocol for selected user.");
   mMainLayout.addWidget(&mCreateButton);

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
   //TODO
}
