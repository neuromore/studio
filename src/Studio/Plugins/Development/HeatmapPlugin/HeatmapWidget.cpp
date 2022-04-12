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

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "HeatmapWidget.h"
#include "HeatmapPlugin.h"
#include <Core/LogManager.h>
#include <QPainter>
#include <QtBase/QtBaseManager.h>

using namespace Core;


// constructor
HeatmapWidget::HeatmapWidget(HeatmapPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	mPlugin		= plugin;
	mEmptyText	= "This is the text shown in case no split view is rendered (e.g. in case no classifier or device is active)";
}


// destructor
HeatmapWidget::~HeatmapWidget()
{
    mProgram->release();
	delete mProgram;
}


void HeatmapWidget::initializeGL()
{
    // base class initialize
	OpenGLWidget::initializeGL();
	InitShader();
}


// render frame
void HeatmapWidget::paintGL()
{
    // get the time delta since the last render call
	//const double timeDelta = GetTimeDelta();
    //int timeSinceRenderStart = QTime::currentTime().msec();

	if (GetDeviceManager()->GetNumDevices() == 0)
		return;

    Device* device = GetDeviceManager()->GetDevice(0);
    BciDevice* headset = static_cast<BciDevice*>(device);
    const uint32 numElectrodes = headset->GetNumSensors();
    
	// FIXME: dynamic allocation or usage of electrode.max()
    QVector2D electrodeAngles[8];
	GLfloat electrodeSignals[8];

	// set colorcode
	QVector3D colorcodeRGBA = QVector3D(1.0, 0.0, 0.0);
	GLint exponent          = 30;

    for (uint32 i = 0; i < numElectrodes; ++i)
    {
        Sensor* sensor = headset->GetNeuroSensor(i);
		// get coordinates of signals
		// TODO: normalize this
        EEGElectrodes::Electrode electrode = GetEEGElectrodes()->GetElectrodeByID(sensor->GetName());
        const float theta = electrode.GetTheta();
		const float phi   = electrode.GetPhi();
        // set electrode angles
        electrodeAngles[i] = QVector2D(theta, phi);

		electrodeSignals[i] = 0.0;
		// set signal strength
		electrodeSignals[0] = 1.0;
		//electrodeSignals[1] = 1.0;
		electrodeSignals[2] = 1.0;
		electrodeSignals[5] = 1.0;
    }

    // pass attributes to shader
    mProgram->setAttributeValue("width", width());
    mProgram->setAttributeValue("height", height());
	mProgram->setAttributeValue("numElectrodes", numElectrodes);
	// pass uniforms to shader
    mProgram->setUniformValueArray("electrodeAngles", electrodeAngles, 8);
    mProgram->setUniformValueArray("electrodeSignals", electrodeSignals, 8, 1);
	mProgram->setUniformValue("colorcode", colorcodeRGBA);
	mProgram->setUniformValue("exponent", exponent);

    glEnable(GL_BLEND);
	glBegin(GL_TRIANGLES);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// first triangle
	glVertex3f(-1.0, -1.0, 0.0);
	glVertex3f(-1.0,  1.0, 0.0);
	glVertex3f( 1.0,  1.0, 0.0);
	// second triangle
	glVertex3f(-1.0, -1.0, 0.0);
	glVertex3f( 1.0, -1.0, 0.0);
	glVertex3f( 1.0,  1.0, 0.0);
	glEnd();
	update();
	glDisable(GL_BLEND);
}


void HeatmapWidget::InitShader()
{

	mProgram = new QOpenGLShaderProgram();

	// Compile vertex shader
	if (!mProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, GetAppDir() + "../Source/Resources/Assets/Shaders/HeatmapVertexShader.glsl"))
		close();

	// Compile fragment shader
	if (!mProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, GetAppDir() + "../Source/Resources/Assets/Shaders/HeatmapFragmentShader.glsl"))
		close();

    // Link shader pipeline
    if (!mProgram->link())
        close();

    // Bind shader pipeline for use
    if (!mProgram->bind())
        close();
}
