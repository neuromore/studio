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

#ifndef __NEUROMORE_FEEDBACKHISTORYWIDGET_H
#define __NEUROMORE_FEEDBACKHISTORYWIDGET_H

// include required headers
#include "../../Config.h"
#include <DSP/Channel.h>
#include <Graph/Classifier.h>
#include "../../Rendering/OpenGLWidget.h"


// forward declaration
class FeedbackPlugin;

class FeedbackHistoryWidget : public OpenGLWidget
{
	Q_OBJECT
	public:
		FeedbackHistoryWidget(FeedbackPlugin* plugin, QWidget* parent=NULL);
		virtual ~FeedbackHistoryWidget();

		void paintGL() override final;

		FeedbackPlugin* GetPlugin()								{ return mPlugin; }
		Classifier* GetClassifier() const;

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:
				RenderCallback(FeedbackHistoryWidget* feedbackWidget, OpenGLWidget* parent) : OpenGLWidgetCallback(parent) { mFeedbackWidget = feedbackWidget; }
				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height) override;
				void RenderTimeline(double x, double y, double width, double height) override;

			private:
				FeedbackHistoryWidget*	mFeedbackWidget;
				Core::String			mTempString;
		};

		friend class RenderCallback;

		Core::Array<FeedbackNode*> mNodesToRender;
		FeedbackPlugin*		mPlugin;
		RenderCallback*		mRenderCallback;
		double				mLeftTextWidth;

		QColor				mGridColor;
		QColor				mSubGridColor;
		QColor				mTextColor;
		QColor				mFeedbackNameColor;
		QColor				mBackgroundColor;
		QColor				mAreaBgColor;
};


#endif
