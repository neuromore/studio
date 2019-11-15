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

#ifndef __NEUROMORE_GRAPHPALETTEWIDGET_H
#define __NEUROMORE_GRAPHPALETTEWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <Graph/Graph.h>
#include <Graph/GraphObjectFactory.h>
#include "GraphPlugin.h"
#include <QWidget>
#include <QListWidget>

QT_FORWARD_DECLARE_CLASS(QTabBar)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QVBoxLayout)


// forward declaration
class GraphPlugin;

class GraphPaletteList : public QListWidget
{
	public:
		enum
		{
			NODETYPE_NOTUSED		= 0,
			NODETYPE_NODE			= 1
		};

		GraphPaletteList(GraphWidget::Type type, QWidget* parent=NULL) : QListWidget(parent)	{ mType=type; }
		virtual ~GraphPaletteList()										{}

	protected:
		QMimeData* mimeData(const QList<QListWidgetItem*> items) const;
		QStringList mimeTypes() const;
		Qt::DropActions supportedDropActions() const;

	private:
		GraphWidget::Type	mType;
};


// the graph node palette widget
class GraphPaletteWidget : public QWidget
{
	Q_OBJECT

	public:
		GraphPaletteWidget(GraphWidget::Type type, QWidget* parent=NULL);
		virtual ~GraphPaletteWidget();

		void SetGraphPlugin(GraphPlugin* plugin)					{ mGraphPlugin = plugin; }

		void Init();
		static QString GetNodeIconFileName(Node* node);
		static QString GetNodeIconFileName(QString nodeTypeString);
		static QIcon GetNodeIcon(Node* node);

	public slots:
		void OnChangeCategoryTab(int index);

	private:
		GraphWidget::Type	mType;
		GraphPaletteList*	mList;
		QTabBar*			mTabBar;
		QVBoxLayout*		mInitialLayout;
		GraphPlugin*		mGraphPlugin;
		Core::String		mTempString;

		void RegisterItems(uint32 category);
		void InitInterface();
};


#endif
