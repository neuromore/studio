/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PLUGIN_H
#define __NEUROMORE_PLUGIN_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/AttributeSet.h>
#include "../DockWidget.h"
#include "../ImageButton.h"
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


// forward declaration
class SettingsWindow;
class DockHeader;

class QTBASE_API Plugin : public QObject, public Core::AttributeSet
{
	Q_OBJECT
	public:
		enum
		{
			ATTRIB_SETTINGSVISIBLE = 0,
			NUM_BASEATTRIBUTES
		};

		// constructing & destructing
		Plugin(const char* typeUuid);
		virtual ~Plugin();

		// plugin name and uuid
		virtual const char* GetName() const = 0;
		virtual const char* GetRuleName() const = 0;
		const char* GetTypeUuid() const															{ return mTypeUuid.AsChar(); }
		const Core::String& GetTypeUuidString() const											{ return mTypeUuid; }

		// real-time identification (regenerated at object construction time and NOT serialization consistent)
		uint32 GetId() const																	{ return mId; }
		
		// initializing & cloning
		virtual void RegisterAttributes();
		virtual bool Init() = 0;
		virtual Plugin* Clone() = 0;

		// settings & keyboard shortcuts
		virtual void AddSettings(SettingsWindow* settingsWindow)								{}

		// interface updating
		virtual void UpdateInterface()															{}
		virtual bool NeedRegularUpdateInterface()												{ return false; }
		void SetVisible(bool isVisible);
		bool IsVisible();
		
		// lock down plugin in main window so it can't be move (fails for tabbified widgets)
		void SetLocked(bool locked = true);
		bool IsLocked() const																	{ return mIsLocked; }

		// is settings panel open?
		bool GetSettingsVisibility() const														{ if (mSettingsWidget == NULL) return false; return mSettingsWidget->isVisible(); }

		// real-time plugins
		void SetRealtimeWidget(QWidget* widget)													{ mRealtimeWidget = widget; }
		virtual void RealtimeUpdate();
		void SetShowPerformanceInfo(bool show)													{ mShowPerformanceInfo = show; }
		bool GetShowPerformanceInfo() const														{ return mShowPerformanceInfo; }

		// dock behavior
		virtual bool AllowMultipleInstances() const												{ return false; }
		virtual bool IsClosable() const															{ return true;	}
		virtual bool IsFloatable() const														{ return true;	}
		virtual bool IsVertical() const															{ return false;	}
		virtual bool IsMovable() const															{ return true;  }

		// event callbacks
		virtual void OnPostAuthenticationInit()													{}
		virtual void OnAfterLoadLayout()														{}

		QString GetObjectName() const															{ assert(mDock); return mDock->objectName(); }
		void SetObjectName(const QString& name)													{ mDock->setObjectName(name); }

		void SetSettingsWidget(QWidget* widget, bool bottomSpacer=true, int margin=5);
		void CreateBaseInterface(const char* objectName);

		inline DockWidget* GetDockWidget()														{ return mDock; }

		// attributes & settings
		void SaveSettings(Core::Json& json, Core::Json::Item& pluginItem)						{ AttributeSet::Write( json, pluginItem, true ); }
		bool LoadSettings(const Core::Json& json, const Core::Json::Item& item)					{ return AttributeSet::Read(json, item, true); }

		void SetTitle(const char* title);
		void SetTitleBarVisible(bool visible = true);

	signals:
		void RemovePlugin();
		
	public slots:
		void OnOpenCloseSettings();
	private slots:
		void OnRemovePlugin()																	{ emit RemovePlugin(); }
		void UpdateSettingsButtonVisibility();

	protected:
		void CreateDockMainWidget(QWidget** outMainWidget, QHBoxLayout** outMainHLayout);
		void FillLayouts(QWidget* mainWidget, QHBoxLayout* mainHLayout, const Core::Array<QWidget*>& toolbarWidgets, const char* buttonText, const char* iconPrefix, QWidget* renderWidget);
		QDockWidget::DockWidgetFeatures GetDockWidgetFeatures() const;

		Core::String						mTypeUuid;
		DockHeader*							mDockHeader;
		DockWidget*							mDock;
		bool								mIsLocked;
		uint32								mId;
		bool								mForceEnableSettings;

		// settings interface
		ImageButton*						mSettingsButton;
		QWidget*							mSettingsWidget;

		// real-time widget
		QWidget*							mRealtimeWidget;
		bool								mShowPerformanceInfo;
};


#endif
