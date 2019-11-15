/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LAYOUTMANAGER_H
#define __NEUROMORE_LAYOUTMANAGER_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/Array.h>
#include <Core/Mutex.h>
#include "Layout.h"
#include "LayoutMenu.h"
#include "ColorPalette.h"
#include <QBasicTimer>


// forward declaration
class LayoutComboBox;

// layout manager
class QTBASE_API LayoutManager : public QObject
{
	Q_OBJECT
	public:
		LayoutManager();
		virtual ~LayoutManager();

		void SetComboBox(LayoutComboBox* combobox)						{ mComboBox = combobox; }
		LayoutMenu* ConstructMenu();

		// TODO make this dependent on the type of layout group that is loaded
		void EnableCustomizing(bool enableCustomizing);
		bool IsCustomizingEnabled() const								{ return mCustomizingEnabled; }
		
		void Clear();
		void ReInit();

		// TODO add 'layout groups' so we can switch between for example 1) built in layouts 2) design layouts 
		uint32 GetNumLayouts() const									{ return mLayouts.Size(); }
		Layout* GetLayout(uint32 index)									{ return mLayouts[index]; }
		Layout* GetActiveLayout() const									{ return mActiveLayout; }
		uint32 FindLayoutIndexByName(const char* layoutName);
		Layout* FindLayoutByName(const char* layoutName);

		// layout switching
		void SwitchToLayoutByName(const char* layoutName);
		void SwitchToLayoutByIndex(uint32 index);

		void OnRemoveLayout(int layoutIndex);

		// icons and colors for menu and combobox
		static const QIcon& GetHddIcon();
		static QColor GetHddColor()										{ return ColorPalette::Shared::GetBackgroundQColor(); }
		static const QIcon& GetCloudIcon();
		static QColor GetCloudColor()									{ return ToQColor(ColorPalette::Shared::GetCiColorBlue()); }
		static const QIcon& GetBoxIcon();
		static QColor GetBoxColor()										{ return ToQColor(ColorPalette::Shared::GetCiColorGreyBright()); }

	public slots:
		void OnSaveLayoutAs(); // DEPRECATE
		void OnSaveLayout();

	public:
		Core::String GetLocalCustomLayoutsFolder() const;							 // DEPRECATE
		Core::String ConstructLocalCustomLayoutFilename(const char* layoutName);	 // DEPRECATE	

	private:

		// TODO add 'layout groups' so we can switch between for example 1) built in layouts 2) design layouts 
		void RegisterLayout(Layout* layout);

		Core::Array<Layout*>	mLayouts;
		Layout*					mActiveLayout;
		Core::Json				mJson;

		bool					mCustomizingEnabled;
		Core::Mutex				mThreadLock;
		bool					mSwitchHappening;
		bool					mAutoLoadDefaultLayout;								// DEPRECATE

		LayoutComboBox*			mComboBox;
		LayoutMenu*				mMenu;
};


#endif
