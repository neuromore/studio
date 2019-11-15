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

#ifndef __NEUROMORE_GRAPHINFOWIDGET_H
#define __NEUROMORE_GRAPHINFOWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <QWidget>
#include <QTreeWidget>

// shows realtime information of the running classifier
class GraphInfoWidget : public QWidget, public Core::EventHandler
{
	Q_OBJECT
	public:
		GraphInfoWidget(QWidget* parent = NULL, Classifier* classifier = NULL);
		~GraphInfoWidget();

		inline void SetClassifier(Classifier* classifier)						{ mClassifier = classifier; }

		// (re)init widget
		void Init();
		void ReInit();

		// update displayed information
		virtual void UpdateInterface();

		// EVENTS
		void OnActiveClassifierChanged(Classifier* classifier) override			{ SetClassifier(classifier); ReInit(); }
		void OnGraphModified(Graph* graph, GraphObject* object) override		{ ReInit(); }
	
	private:

		// UI elements
		QTreeWidget*			mInfoTree;						// the device information in tree form
	
		void AddItems();

		QTreeWidgetItem*		mClassifierSection;				// subsection item "Classifier"
		QTreeWidgetItem*		mInputsSection;					// subsection item "Inputs"
		QTreeWidgetItem*		mOutputsSection;				// subsection item "Outputs"

		void AddClassifierSectionItems(QTreeWidget* parent);
		void UpdateClassifierSectionItems(QTreeWidget* parent);

		void AddInputsSectionItems(QTreeWidgetItem* parent);
		void UpdateInputsSectionItems(QTreeWidgetItem* parent);

		void AddOutputsSectionItems(QTreeWidgetItem* parent);
		void UpdateOutputsSectionItems(QTreeWidgetItem* parent);


		// helpers
		Core::String			mTempString;
		
	protected:
		Classifier*				mClassifier;					// the classifier that provides the information
};

#endif
