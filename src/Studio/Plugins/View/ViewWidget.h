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

#ifndef __NEUROMORE_VIEWWIDGET_H
#define __NEUROMORE_VIEWWIDGET_H

// include required headers
#include "../../Config.h"
#include <DSP/Channel.h>
#include <Graph/Classifier.h>
#include "../../Rendering/OpenGLWidget.h"


// forward declaration
class ViewPlugin;

class ViewWidget : public OpenGLWidget
{
	Q_OBJECT
	public:
		ViewWidget(ViewPlugin* plugin, QWidget* parent=NULL);
		virtual ~ViewWidget();

		void paintGL() override final;

		ViewPlugin* GetPlugin()								{ return mPlugin; }
		Classifier* GetClassifier() const;

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:
				RenderCallback(ViewWidget* widget, OpenGLWidget* parent) : OpenGLWidgetCallback(parent)	{ mViewWidget = widget; }
				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height) override;
				void RenderTimeline(double x, double y, double width, double height) override;

			private:
				ViewWidget*		mViewWidget;
				Core::String	mTempString;
		};

		friend class RenderCallback;

		ViewPlugin*			mPlugin;
		RenderCallback*		mRenderCallback;
		double				mLeftTextWidth;
};


#endif
