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

#ifndef __NEUROMORE_PARAMETERSWIDGETS_H
#define __NEUROMORE_PARAMETERSWIDGETS_H

// include required headers
#include "../../Config.h"
#include <QScrollArea>
#include "ParameterWidgets.h"
#include <EngineManager.h>
#include <QVBoxLayout>


class ParametersWidget : public QScrollArea, public Core::EventHandler
{
	Q_OBJECT
	public:
		ParametersWidget(QWidget* parent);
		virtual ~ParametersWidget();

		// reinit plugin for parameters
		void ReInit(Classifier* classifier);
		
		// called regularily to update the information on the interface
		void UpdateInterface();

		// EVENTS ----------------------------------------------
		void OnGraphModified(Graph* graph, GraphObject* object) override final;
		void OnGraphReset(Graph* graph) override final;
		void OnActiveClassifierChanged(Classifier* classifier) override final			{ ReInit(classifier); }
		void OnRemoveNode(Graph* graph, Node* node) override final;
		void OnNodeStarted(Graph* graph, SPNode* node) override final;
		void OnNodeStopped(Graph* graph, SPNode* node) override final;
		void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute) override;
		void OnStartSession() override;
		// -----------------------------------------------------

		// get slider for widget pointer
		ParameterWidget* FindWidgetForNode(ParameterNode* node) const;

	private:
		Classifier*								mClassifier;
		QVBoxLayout*							mMainLayout;

		Core::Array<ParameterWidget*>			mWidgets;

		QLabel*									mInfoLabel;
		QWidget*								mSpacerWidget;

		Core::String							mTempString;

		void ClearLayout();
		void FillLayout();

		void UpdateWidget(ParameterNode* node);
		ParameterWidget* AddWidget(ParameterNode* node);
		bool RemoveWidget(ParameterNode* node);
		void SortWidgets();
		void ApplyParameters();
		void UpdateWidgets();

};


#endif
