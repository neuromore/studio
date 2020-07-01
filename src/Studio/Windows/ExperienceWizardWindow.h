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

#ifndef __NEUROMORE_EXPERIENCEWIZARDWINDOW_H
#define __NEUROMORE_EXPERIENCEWIZARDWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Timer.h>
#include "../Config.h"
#include <ImageButton.h>
#include <QLayout>
#include <QComboBox>
#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QListWidget>
#include <User.h>

// window to select and quick configure a classifier into an experience for a user
class ExperienceWizardWindow : public QDialog
{
   Q_OBJECT
   public:
      ExperienceWizardWindow(const User& user, QWidget* parent);
      virtual ~ExperienceWizardWindow();

   private slots:
      void OnClassifierSelectIndexChanged(int index);
      void OnCreateClicked();

   private:
      void CreateRowChannelSelector(const char* name);
      void CreateChannelSelectorListItem(QListWidget& list, const char* channel, const char* band);

   private:
      const User&      mUser;
      QVBoxLayout      mMainLayout;
      ////////////////////////////////////////
      QHBoxLayout      mUserLayout;
      QLabel           mUserDesc;
      QLabel           mUserLabel;
      ////////////////////////////////////////
      QHBoxLayout      mClassifierLayout;
      QLabel           mClassifierSelectDesc;
      QComboBox        mClassifierSelect;
      ////////////////////////////////////////
      QTableWidget     mTableWidget;
      QTableWidgetItem mHeaderType;
      QTableWidgetItem mHeaderName;
      QTableWidgetItem mHeaderEdit;
      ////////////////////////////////////////
      QPushButton      mCreateButton;
};


#endif
