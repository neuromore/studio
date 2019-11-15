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
#include "SettingsWindow.h"
#include <QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include "../AppManager.h"
#include <QSettings>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QGridLayout>


using namespace Core;

// constructor
SettingsWindow::SettingsWindow(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
}


// destructor
SettingsWindow::~SettingsWindow()
{
	// get rid of the categories
	const uint32 numCategories = mCategories.Size();
	for (uint32 i=0; i<numCategories; ++i)
		delete mCategories[i];
	mCategories.Clear();
}


// initialize the preferences window
void SettingsWindow::Init()
{
	setWindowTitle("Settings");
	setSizeGripEnabled(false);

	const int32 iconSize	= 64;
	const int32 spacing		= 6;

	// the icons on the left which list the categories
	mCategoriesWidget = new QListWidget();
	mCategoriesWidget->setViewMode(QListView::IconMode);
	mCategoriesWidget->setIconSize( QSize(iconSize, iconSize) );
	mCategoriesWidget->setMovement(QListView::Static);
	mCategoriesWidget->setMaximumWidth( iconSize + 3*spacing );
	mCategoriesWidget->setMinimumHeight( 5 * iconSize + 5*spacing );
	mCategoriesWidget->setSpacing(spacing);
	mCategoriesWidget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
	connect( mCategoriesWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),	this, SLOT(ChangePage(QListWidgetItem *, QListWidgetItem*)));

	// create the stacked widget, this one will store all the different property browsers for the categories
	mStackedWidget = new QStackedWidget();
	mStackedWidget->setMinimumSize(500, 500);

	QHBoxLayout* horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(mCategoriesWidget);
	horizontalLayout->addWidget(mStackedWidget);
	setLayout(horizontalLayout);
}


// add categories from the given plugin or in case the parameter is NULL from all active plugins
void SettingsWindow::AddCategoriesFromPlugin(Plugin* plugin)
{
	PluginManager* pluginManager = GetPluginManager();
	const uint32 numPlugins = pluginManager->GetNumActivePlugins();
	for (uint32 i=0; i<numPlugins; ++i)
	{
		Plugin* currentPlugin = pluginManager->GetActivePlugin(i);

		if (plugin == NULL || currentPlugin == plugin)
			currentPlugin->AddSettings(this);
	}

	mCategoriesWidget->setCurrentRow(0);
}


// called when the dialog got closed
void SettingsWindow::closeEvent(QCloseEvent* event)
{
}


// add a new category
void SettingsWindow::AddCategory(QWidget* widget, const char* categoryName, const char* relativeFilename, bool readOnly)
{
	// create the category button
	QListWidgetItem* categoryButton = new QListWidgetItem();

	// load the category image and pass it to the category buttom
	categoryButton->setIcon( GetQtBaseManager()->FindIcon(relativeFilename) );

	// set the category button name and style it
	categoryButton->setText( categoryName );
	categoryButton->setTextAlignment( Qt::AlignHCenter );
	categoryButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

	// add the button to the categories list and the property widget to the stacked widget on the right
	mCategoriesWidget->addItem( categoryButton );
	mStackedWidget->addWidget( widget );

	// create a category object so that we can get access to the data later on again
	Category* category				= new Category();
	category->mName					= categoryName;
	category->mWidget				= widget;
	category->mPropertyTreeWidget	= NULL;
	category->mListWidgetItem		= categoryButton;

	// add the new category object to the categories array
	mCategories.Add(category);
}


// add a new category
PropertyTreeWidget* SettingsWindow::AddCategory(const char* categoryName, const char* relativeFilename, bool readOnly)
{
	// create the category button
	QListWidgetItem* categoryButton = new QListWidgetItem();

	// load the category image and pass it to the category buttom
	categoryButton->setIcon( GetQtBaseManager()->FindIcon(relativeFilename) );

	// set the category button name and style it
	categoryButton->setText( categoryName );
	categoryButton->setTextAlignment( Qt::AlignHCenter );
	categoryButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

	// create the new property widget
	PropertyTreeWidget* propertyWidget = new PropertyTreeWidget(this);
	propertyWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	
	// add the button to the categories list and the property widget to the stacked widget on the right
	mCategoriesWidget->addItem( categoryButton );
	mStackedWidget->addWidget( propertyWidget );

	// create a category object so that we can get access to the data later on again
	Category* category				= new Category();
	category->mName					= categoryName;
	category->mPropertyTreeWidget	= propertyWidget;
	category->mWidget				= propertyWidget;
	category->mListWidgetItem		= categoryButton;

	// add the new category object to the categories array and return the property widget
	mCategories.Add(category);
	return propertyWidget;
}


// find category by name
SettingsWindow::Category* SettingsWindow::FindCategoryByName(const char* categoryName)
{
	// get the number of categories and iterate through them
	const uint32 numCategories = mCategories.Size();
	for (uint32 i=0; i<numCategories; ++i)
	{
		Category* category = mCategories[i];

		// compare the passed name with the current category and return if they are the same
		if (category->mName.IsEqual(categoryName) == true)
			return category;
	}

	// in case we haven't found the category with the given name return NULL
	return NULL;
}


// new category icon pressed, change to the corresponding property browser
void SettingsWindow::ChangePage(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current == NULL)
		current = previous;

	mStackedWidget->setCurrentIndex( mCategoriesWidget->row(current) );
}
