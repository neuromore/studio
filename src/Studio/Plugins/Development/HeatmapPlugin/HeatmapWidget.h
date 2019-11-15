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

#ifndef __NEUROMORE_HEATMAPWIDGET_H
#define __NEUROMORE_HEATMAPWIDGET_H

// include required headers
#include "../../../Config.h"
#include "../../../Rendering/OpenGLWidget2DHelpers.h"
#include "qopenglshaderprogram.h"
#include <Engine/EngineManager.h>


// forward declaration
class HeatmapPlugin;

class HeatmapWidget : public OpenGLWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		HeatmapWidget(HeatmapPlugin* plugin, QWidget* parent);
		virtual ~HeatmapWidget();

		// render frame
		void paintGL() override final;
        void initializeGL() override final;

	private:

        void GetSensorRenderInfo(Sensor* sensor, float halfHeadWidth, float halfHeadHeight, const Core::Vector2& screenCenter, QColor* outBrushColor, QColor* outPenColor, QRectF* outRectF, QRect* outRect);
        void InitShader();

		HeatmapPlugin*	                mPlugin;
        QOpenGLShaderProgram*           mProgram;
};


#endif
