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
#include "EEGElectrodesWidget.h"
#include <EngineManager.h>
#include <Core/LogManager.h>
#include "../Rendering/OpenGLWidget2DHelpers.h"
#include <QPainter>


using namespace Core;

// constructor
EEGElectrodesWidget::EEGElectrodesWidget(QWidget* parent) : OpenGLWidget(parent)
{
	// create the render callback
	mRenderCallback = new RenderCallback(this);
	SetCallback( mRenderCallback );

	mBciDevice	= NULL;
	mEmptyText	= "No headset active";
}


// destructor
EEGElectrodesWidget::~EEGElectrodesWidget()
{
	// destroy the render callback
	delete mRenderCallback;
}


// initialize OpenGL
void EEGElectrodesWidget::initializeGL()
{
	// base class initialize
	OpenGLWidget::initializeGL();

	::glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	::glClearColor(0.19f, 0.19f, 0.19f, 0.5f);			// Background Color
	::glClearDepth(1.0f);								// Depth Buffer Setup
	::glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	::glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
	::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// Really Nice Perspective Calculations

	GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]=	{ 0.0f, 0.0f, 2.0f, 1.0f };

	::glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);	// Setup The Ambient Light
	::glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);	// Setup The Diffuse Light
	::glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
	::glEnable(GL_LIGHT1);								// Enable Light One
}


// render frame
void EEGElectrodesWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	Render( 0.0 );

	// post rendering
	PostRendering();
}


void EEGElectrodesWidget::GetSensorRenderInfo(Sensor* sensor, float halfHeadWidth, float halfHeadHeight, const Vector2& screenCenter, QColor* outBrushColor, QColor* outPenColor, QRectF* outRectF, QRect* outRect, QPointF* outCircleCenter)
{
	const float circleRadius = 10.0f * devicePixelRatioF();

	// get the contact quality color for the given sensor
	const Color contactQualityColor = sensor->GetContactQualityColor();
	
	// set brush and pen color
	outBrushColor->setRgbF ( contactQualityColor.r, contactQualityColor.g, contactQualityColor.b);
	outPenColor->setRgbF ( contactQualityColor.r * 0.2, contactQualityColor.g * 0.2, contactQualityColor.b * 0.2);

	// darken all sensors that are not used by the classifier (if there is any)
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier != NULL) 
	{
		if (classifier->IsSensorUsed(sensor) == false)
			outBrushColor->setRgbF ( contactQualityColor.r * 0.4, contactQualityColor.g * 0.4, contactQualityColor.b * 0.4);
	}


	// get the sensor position
	EEGElectrodes::Electrode electrode = GetEEGElectrodes()->GetElectrodeByID(sensor->GetName());
	const Vector2 pos = GetEEGElectrodes()->Get2DPosition( electrode );

	// calculate the center point of the sensor glyph
	QPointF circleCenter;
	circleCenter.setX( screenCenter.x + pos.x * halfHeadWidth );
	circleCenter.setY( screenCenter.y - pos.y * halfHeadHeight );
	*outCircleCenter = circleCenter;

	// set the floating point rect
	outRectF->setTopLeft( QPointF(circleCenter.x() + circleRadius, circleCenter.y() + circleRadius) );
	outRectF->setBottomRight( QPointF(circleCenter.x() - circleRadius, circleCenter.y() - circleRadius) );

	QRect textRect( outRectF->x(), outRectF->y(), outRectF->width(), outRectF->height() );
	*outRect = textRect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
EEGElectrodesWidget::RenderCallback::RenderCallback(EEGElectrodesWidget* parent) : OpenGLWidgetCallback(parent)
{
	mParent = parent;

	// brain model view mat
	mBrainModelViewMat.setToIdentity();
	mBrainModelViewMat.translate( 0, 0, -500);
	mBrainModelViewMat.rotate( 90, 1, 0, 0);
	mBrainModelViewMat.rotate( 180, 0, 1, 0);

	// load the brain
	mBrainMesh = new Mesh();
	mBrainAABB.Init();
	if (mBrainMesh->Load(":/Assets/Models/Brain.obj") == false)
	{
		delete mBrainMesh;
		mBrainMesh = NULL;
	}
	else
		mBrainMesh->CalcAABB( &mBrainAABB );
}


// destructor
EEGElectrodesWidget::RenderCallback::~RenderCallback()
{
	delete mBrainMesh;
}


Vector3 Project(const Vector3& point, const QMatrix4x4& viewProjMatrix, uint32 screenWidth, uint32 screenHeight)
{
	// 1. expand homogenous coordinate
	QVector4D expandedPoint(point.x, point.y, point.z, 1.0f);

	// 2. multiply by the view and the projection matrix (note that this is a four component matrix multiplication, so no affine one!)
	expandedPoint = viewProjMatrix * expandedPoint;

	// 3. perform perspective division for the x and y coordinates only
	expandedPoint.setX( expandedPoint.x() / expandedPoint.w() );
	expandedPoint.setY( expandedPoint.y() / expandedPoint.w() );

	// 4. map them to the screen space
	Vector3 result;
	result.x = (1.0f + expandedPoint.x()) * (float)screenWidth * 0.5f;
	result.y = (1.0f - expandedPoint.y()) * (float)screenHeight * 0.5f;

	// 5. get the distance to the camera lense plane
	result.z = expandedPoint.z(); //nearClipDistance + expandedPoint.z * (farClipDistance - nearClipDistance);

	return result;
}


// render callback
void EEGElectrodesWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// get the time delta since the last render call
	//const double timeDelta = mParent->GetTimeDelta();

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// draw background rect
	AddRect(-1, -1, width, height, Color(60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f));
	RenderRects();

		// set the viewport to the multi view area
		// NOTE: Retina Support: glViewport expects device pixels, but the various geometry accessors returns values in device-independent pixels
		mParent->glViewport( mOffsetX, mOffsetY, width, height );

		// enable depth testing
		mParent->glEnable( GL_DEPTH_TEST );
		mParent->glEnable( GL_LIGHTING );

		// prepare ortho mat
		QMatrix4x4 orthoMat;
		const float halfExtend = (mBrainAABB.GetMax().z - mBrainAABB.GetMin().z) * 0.5;
		const Vector3 center = mBrainAABB.GetCenter();
		orthoMat.ortho( center.x-halfExtend, center.x+halfExtend, center.z-halfExtend, center.z+halfExtend, 0.01f, 1000.0f );

		// pass the camera information to OpenGL
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( mBrainModelViewMat.data() );
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( orthoMat.data() );
		glMatrixMode(GL_MODELVIEW);

		QMatrix4x4 camViewProjMat = orthoMat * mBrainModelViewMat;
	
		// render the brain
		if (mBrainMesh != NULL)
			mBrainMesh->Render( mParent, QMatrix4x4() );

	//////////////////////////////////////////////////////////////////////////////////////////

	BciDevice* headset = mParent->mBciDevice;

	// helper variables
	Vector3 projectedMin		= Project( mBrainAABB.GetMin(), camViewProjMat, width-10, height-10 );
	Vector3 projectedMax		= Project( mBrainAABB.GetMax(), camViewProjMat, width-10, height-10 );
	const float halfHeadWidth	= Math::Abs( projectedMax.x - projectedMin.x ) * 0.5f;
	const float halfHeadHeight	= Math::Abs( projectedMax.y - projectedMin.y ) * 0.5f;
	Vector2 screenCenter		= Vector2( width*0.5, height*0.5 );

	// get the number of EEG sensors from the neuro headset
	uint32 numSensors = 0;
	if (headset != NULL)
		numSensors = headset->GetNumNeuroSensors();

	bool lighting = ::glIsEnabled(GL_LIGHTING);
	::glDisable( GL_LIGHTING );

	// render the circles and sensor names
	for (uint32 i=0; i<numSensors; ++i)
	{
		// get access to the current sensor
		Sensor* sensor = headset->GetNeuroSensor(i);

		// skip disabled sensors
		if (sensor->IsEnabled() == false)
			continue;

		// calc the sensor render info
		QColor brushColor, penColor;
		QRectF rectF;
		QRect rect;
		QPointF circleCenter;
		mParent->GetSensorRenderInfo( sensor, halfHeadWidth, halfHeadHeight, screenCenter, &brushColor, &penColor, &rectF, &rect, &circleCenter );

		// set the pen & brush
		//mPainter->setBrush( brushColor );
		//mPainter->setPen( penColor );

		// draw the circle
		//mPainter->drawEllipse( rectF );

		// set the projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho( 0.0f, width, height, 0.0f, -100.0f, 100.0f );
		//glLoadMatrixf( orthoMat.data() );

		// set the modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		/*QMatrix4x4 sphereMat;
		sphereMat.setToIdentity();

		const float scale = mBrainAABB.GetRadius() * 0.01;
		sphereMat.scale( scale, scale, scale);
		sphereMat.translate( circleCenter.x(), circleCenter.y(), -500);
		glLoadMatrixf( sphereMat.data() );*/

		mParent->glDisable(GL_DEPTH_TEST);

		RenderSphere(Vector3(circleCenter.x(), circleCenter.y(), -10), (float)rect.width()*0.5f, Color(brushColor.redF(), brushColor.greenF(), brushColor.blueF()));

		// draw the sensor name
		RenderText( sensor->GetName(), mParent->GetDefaultFontSize() * mParent->devicePixelRatioF(), penColor, circleCenter.x(), circleCenter.y(), OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_CENTER );
	}

	mParent->glEnable(GL_DEPTH_TEST);

	if (lighting == true)
		::glEnable(GL_LIGHTING);
	else
		::glDisable(GL_LIGHTING);
}
