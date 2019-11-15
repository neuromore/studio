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

#ifndef __NEUROMORE_SETTINGSWINDOW_H
#define __NEUROMORE_SETTINGSWINDOW_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "../Config.h"
#include <AttributeWidgets/PropertyTreeWidget.h>
#include <PluginSystem/Plugin.h>
#include <QDialog>


// forward declarations
QT_FORWARD_DECLARE_CLASS(QStackedWidget)
QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
class Plugin;


class SettingsWindow : public QDialog
{
	Q_OBJECT
	public:
		struct Category
		{
			QWidget*					mWidget;
			PropertyTreeWidget*			mPropertyTreeWidget;
			QListWidgetItem*			mListWidgetItem;
			Core::String				mName;
		};

		SettingsWindow(QWidget* parent);
		virtual ~SettingsWindow();

		void Init();
		void AddCategoriesFromPlugin(Plugin* plugin);
		PropertyTreeWidget* AddCategory(const char* categoryName, const char* relativeFilename, bool readOnly);
		void AddCategory(QWidget* widget, const char* categoryName, const char* relativeFilename, bool readOnly);

		inline Category* GetCategory(uint32 index)														{ return mCategories[index]; }
		inline uint32 GetNumCategories()																{ return mCategories.Size(); }
		Category* FindCategoryByName(const char* categoryName);
		inline PropertyTreeWidget* FindPropertyWidgetByName(const char* categoryName)					{ Category* category = FindCategoryByName(categoryName); if (category==NULL) return NULL; else return category->mPropertyTreeWidget; }

	public slots:
		void ChangePage(QListWidgetItem* current, QListWidgetItem* previous);

	private:
		void closeEvent(QCloseEvent* event);

		Core::Array<Category*>						mCategories;
		QStackedWidget*								mStackedWidget;
		QListWidget*								mCategoriesWidget;
};


#endif
