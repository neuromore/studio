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

#ifndef __NEUROMORE_GRAPHATTRIBUTESWIDGET_H
#define __NEUROMORE_GRAPHATTRIBUTESWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <Graph/Node.h>
#include <Graph/Connection.h>
#include <Graph/State.h>
#include <Graph/ActionState.h>
#include <Graph/StateTransition.h>
#include <Graph/StateTransitionCondition.h>
#include <AttributeWidgets/PropertyTreeWidget.h>
#include <Widgets/GraphObjectViewWidget.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>


class GraphAttributesWidget : public QScrollArea, public Core::EventHandler
{
	Q_OBJECT
	public:
		GraphAttributesWidget(QWidget* parent=NULL);
		virtual ~GraphAttributesWidget();

		void InitForGraphObject(GraphObject* object, bool force=false);
		GraphObject* GetGraphObject() const										{ return mGraphObject; }

		void UpdateInterface();

		// event handlers
		void OnStartSession() override											{ ForceReInit(); }
		void OnStopSession() override											{ ForceReInit(); }
		void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute) override;
		void OnGraphModified(Graph* graph, GraphObject* object) override;

	public slots:
		void ReInit()															{ InitForGraphObject( mGraphObject ); }
		void ForceReInit()														{ InitForGraphObject( mGraphObject, true ); }
		void OnValueChanged(Property* property);

	private slots:
		// conditions
		void OnAddConditionButtonClicked();
		void OnAddCondition();
		void OnRemoveCondition();

		// actions
		void OnAddActionButtonClicked(uint32 actionTypeIndex);
		void OnAddEnterActionButtonClicked()									{ OnAddActionButtonClicked(0); }
		void OnAddExitActionButtonClicked()										{ OnAddActionButtonClicked(1); }

		void OnAddAction(uint32 actionTypeIndex);
		void OnAddEnterAction()													{ OnAddAction(0); }
		void OnAddExitAction()													{ OnAddAction(1); }

		void OnRemoveAction(uint32 actionTypeIndex);
		void OnRemoveEnterAction()												{ OnRemoveAction(0); }
		void OnRemoveExitAction()												{ OnRemoveAction(1); }

	private:
		struct AttributeLink
		{
			AttributeWidget*			mWidget;
			GraphObject*				mGraphObject;
			uint32						mAttributeIndex;
			Property*					mProperty;

			AttributeLink()
			{
				mWidget			= NULL;
				mProperty		= NULL;
				mGraphObject	= NULL;
				mAttributeIndex	= CORE_INVALIDINDEX32;
			}
		};

		struct ButtonLookup
		{
			QObject*	mButton;
			uint32		mIndex;
		};
		Core::Array<ButtonLookup>	mRemoveButtonTable;

		void InitForGraph(Graph* node, bool showName=true);
		void InitForNode(Node* node, bool showName=true);
		void InitForConnection(Connection* connection);

		void AddAttributes(GraphObject* object, const char* parentGroupName, bool disableAttributes);

		void AddConditions(GraphObject* object, bool readOnly);
		void AddActions(GraphObject* object, bool readOnly);

		uint32 FindRemoveButtonIndex(QObject* button) const;

		GraphObject*					mGraphObject;
		GraphObjectViewWidget*			mGraphObjectView;
		PropertyTreeWidget*				mPropertyTreeWidget;
		Property*						mNameProperty;
		Property*						mNameProperty_1;
		Property*						mNameProperty_2;
		Property*						mNameProperty_3;
		Property*						mNameProperty_4;
		Property*						mNameProperty_5;
		Property*						mNameProperty_6;
		Property*						mNameProperty_7;
		Property*						mNameProperty_8;
		Core::Array<AttributeLink>		mAttributeLinks;
		Core::AttributeSet*				mAttributeSet;
		Core::String					mParentGroupName;
		Core::String					mTempString;
};


#endif
