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

#ifndef __NEUROMORE_GRAPHPLUGIN_H
#define __NEUROMORE_GRAPHPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include <DockHeader.h>
#include <AttributeWidgets/AttributeWidgetFactory.h>
#include <EngineManager.h>
#include <ImageButton.h>
#include "GraphWidget.h"
#include "GraphAttributesWidget.h"
#include "GraphInfoWidget.h"
#include <QMenuBar>
#include <QPushButton>


// forward declaration
class GraphPaletteWidget;

class GraphPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		GraphPlugin(GraphWidget::Type type, const char* typeUuid);
		virtual ~GraphPlugin();

		GraphWidget::Type GetType() const												{ return mType; }

		void RealtimeUpdate() override													{ mGraphWidget->update(); }
		bool NeedRegularUpdateInterface() override										{ return true; }
		void UpdateInterface() override;

		void OnAfterLoadLayout() override;

		// overloaded main init function
		bool Init() override;

		int32 GetPaletteTabIndex()														{ return mPaletteTabIndex; }
		void SetPaletteIndex(int32 newIndex)											{ mPaletteTabIndex = newIndex; }

		GraphWidget*				GetGraphWidget() const								{ return mGraphWidget; }
		GraphAttributesWidget*		GetGraphAttributesWidget() const					{ return mAttributesWidget; }
		GraphPaletteWidget*			GetGraphPaletteWidget() const						{ return mPaletteWidget; }

		void SetTitleFromGraph(Graph* graph);
	
		// EVENTS ----------------------------------------------
		void OnActiveClassifierChanged(Classifier* classifier) override;
		void OnActiveStateMachineChanged(StateMachine* stateMachine) override;
		void OnGraphModified(Graph* graph, GraphObject* object) override;
		// -----------------------------------------------------

		void OnGraphChanged(Graph* graph);

	public slots:
		void OnPlayButtonClicked();
		void OnPauseButtonClicked();
		void OnStopButtonClicked();
		void OnResyncButtonClicked();
		void OnShowErrorsButtonClicked();
		void OnShowWarningsButtonClicked();

		void OnSave();

	private:
		// attribute change callback
		class AttributeWidgetCallback : public AttributeWidgetFactory::Callback
		{
			public:
				AttributeWidgetCallback(GraphPlugin* plugin)			{ mPlugin = plugin; }
				virtual ~AttributeWidgetCallback()						{}

				void OnAttributeChanged(Core::Attribute* attribute);

			private:
				GraphPlugin* mPlugin;
		};

		GraphWidget::Type			mType;

		// graph widget
		GraphWidget*				mGraphWidget;

		// attribute widget
		GraphAttributesWidget*		mAttributesWidget;

		// palette widget
		GraphPaletteWidget*			mPaletteWidget;

		// information widget
		GraphInfoWidget*			mInfoWidget;

		// toolbar (left hand side)
		QPushButton*				mSaveButton;

		ImageButton*				mAlignLeftButton;
		ImageButton*				mAlignRightButton;
		ImageButton*				mAlignTopButton;
		ImageButton*				mAlignBottomButton;

		ImageButton*				mPlayButton;
		ImageButton*				mPauseButton;
		ImageButton*				mStopButton;
		ImageButton*				mResyncButton;

		ImageButton*				mShowErrorsButton;
		ImageButton*				mShowWarningsButton;	// TODO add warning lists and show all warnings
		ImageButton*				mUnstableIcon;			// Todo remove and move unstable nodes to warnings

		// callbacks
		AttributeWidgetCallback*	mAttributeWidgetCallback;

		int32						mPaletteTabIndex;

		Core::String				mTempString;
};


#endif
