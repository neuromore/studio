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

// include the required headers
#include "OpenGLWidget.h"
#include <QtBaseManager.h>


using namespace Core;

// constructor
OpenGLWidgetCallback::OpenGLWidgetCallback(OpenGLWidget* parent)
{
	mParent = parent;
	mPainter = NULL;
	mLines.Reserve(1024*4);
	mRects.Reserve(1024*4);

	mSphereMesh = NULL;
	InitSphereMesh();

	mFont = new QFont();
	mFont->setPixelSize( GetQtBaseManager()->GetMainWindow()->GetDefaultFontSize() );
	mFontMetrics = new QFontMetrics(*mFont);
}


// destructor
OpenGLWidgetCallback::~OpenGLWidgetCallback()
{
	delete mFontMetrics;
	delete mFont;
	delete mSphereMesh;
}


void OpenGLWidgetCallback::ResetPerformanceStatistics()
{
	mNumDrawn2DLines		= 0;
	mNumDrawn2DRects		= 0;
	mNumDrawnTexts			= 0;
	mNumDrawCalls			= 0;
}


void OpenGLWidgetCallback::RenderLines(double lineWidth)
{
	//const float devicePixelRatio = mParent->devicePixelRatio();

	double oldLineWidth;
	::glGetDoublev( GL_LINE_WIDTH, &oldLineWidth );
	::glLineWidth(lineWidth);

	//Core::Timer time;

	const uint32 numLines = mLines.Size();

	// if there are no lines to render, return directly
	if (numLines == 0)
		return;

	// remember the rendering settings
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	// get the viewport dimensions
	float viewportDimensions[4];
	glGetFloatv( GL_VIEWPORT, viewportDimensions );
	uint32 screenWidth  = (uint32)viewportDimensions[2];
	uint32 screenHeight = (uint32)viewportDimensions[3];

	// setup orthographic view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, screenWidth, screenHeight, 0.0, -1.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	// new version
#ifdef NEUROMORE_PLATFORM_WINDOWS
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	const uint32 stride = sizeof(double)*2 + sizeof(Color);
	glVertexPointer( 2, GL_DOUBLE, stride, mLines.GetPtr() );
	glColorPointer( 4, GL_FLOAT, stride, &(mLines[0].color1) );

	glDrawArrays( GL_LINES, 0, numLines*2 );

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#else
	// OpenGL 1 version
	glBegin(GL_LINES);
		for (uint32 i=0; i<numLines; ++i)
		{
            const Color& color1 = mLines[i].color1;
            const Color& color2 = mLines[i].color2;
            
			glColor4f(color1.r, color1.g, color1.b, color1.a);
			glVertex2d(mLines[i].x1, mLines[i].y1);

			glColor4f(color2.r, color2.g, color2.b, color2.a);
			glVertex2d(mLines[i].x2, mLines[i].y2);
		}
	glEnd();
#endif

	glDisable( GL_BLEND );

	glPopAttrib();

	//const float render2DTime = time.GetTime();
	//LOG("numLines=%i, renderTime=%.3fms", numLines, render2DTime*1000);

	::glLineWidth(oldLineWidth);

	mNumDrawn2DLines += numLines;
	mNumDrawCalls++;

	mLines.Clear(false);
}


void OpenGLWidgetCallback::AddLine(double x1, double y1, const Core::Color& color1, double x2, double y2, const Core::Color& color2)
{
	Line2D line;

	line.color1	= color1;
	line.x1		= mOffsetX+x1;
	line.y1		= mOffsetY+y1;
	
	line.color2	= color2;
	line.x2		= mOffsetX+x2;
	line.y2		= mOffsetY+y2;

	mLines.Add( line );
}


void OpenGLWidgetCallback::AddRect(const Rect& rect)
{
	Rect adjusted = rect;

	adjusted.mX1 = rect.mX1 + mOffsetX;			adjusted.mY1 = rect.mY1 + mOffsetY;
	adjusted.mX2 = rect.mX2 + mOffsetX;			adjusted.mY2 = rect.mY2 + mOffsetY;
	adjusted.mX3 = rect.mX3 + mOffsetX;			adjusted.mY3 = rect.mY3 + mOffsetY;
	adjusted.mX4 = rect.mX4 + mOffsetX;			adjusted.mY4 = rect.mY4 + mOffsetY;

	mRects.Add( adjusted );
}


void OpenGLWidgetCallback::AddRect(double x, double y, double width, double height, const Core::Color& color)
{
	Rect rect;

	rect.mColor1 = rect.mColor2 = rect.mColor3 = rect.mColor4 = color;

	// 1
	rect.mX1 = x;
	rect.mY1 = y;

	// 2
	rect.mX2 = x + width;
	rect.mY2 = y;

	// 3
	rect.mX3 = x + width;
	rect.mY3 = y + height;

	// 4
	rect.mX4 = x;
	rect.mY4 = y + height;

	AddRect( rect );
}


void OpenGLWidgetCallback::RenderRects()
{
	//const double devicePixelRatio	= mParent->devicePixelRatio();

	//Core::Timer time;

	// if there are no lines to render, return directly
	const uint32 numRects = mRects.Size();
	if (numRects == 0)
		return;

	// remember the rendering settings
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	// get the viewport dimensions
	float viewportDimensions[4];
	glGetFloatv( GL_VIEWPORT, viewportDimensions );
	uint32 screenWidth  = (uint32)viewportDimensions[2];
	uint32 screenHeight = (uint32)viewportDimensions[3];

	// setup orthographic view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, screenWidth, screenHeight, 0.0, -1.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	// new version
/*#ifdef NEUROMORE_PLATFORM_WINDOWS
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//const uint32 stride = sizeof(float) * 2 + sizeof(uint32);
	//glVertexPointer( 2, GL_FLOAT, stride, lines );
	//glColorPointer( 4, GL_UNSIGNED_BYTE, stride, &(lines[0].mColor1) );

	const uint32 stride = sizeof(float) * 2 + 4*sizeof(float);
	glVertexPointer( 2, GL_FLOAT, stride, lines );
	glColorPointer( 4, GL_FLOAT, stride, &(lines[0].mColor1) );

	glDrawArrays( GL_LINES, 0, numLines*2 );

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#else*/
	// OpenGL 1 version
	glBegin(GL_QUADS);
		for (uint32 i=0; i<numRects; ++i)
		{
			const Rect& rect = mRects[i];

			// vtx 1
			glColor4d( rect.mColor1.r, rect.mColor1.g, rect.mColor1.b, rect.mColor1.a );
			glVertex3d( rect.mX1, rect.mY1, 0.0 );

			// vtx 2
			glColor4d( rect.mColor2.r, rect.mColor2.g, rect.mColor2.b, rect.mColor2.a );
			glVertex3d( rect.mX2, rect.mY2, 0.0 );

			// vtx 3
			glColor4d( rect.mColor3.r, rect.mColor3.g, rect.mColor3.b, rect.mColor3.a );
			glVertex3d( rect.mX3, rect.mY3, 0.0 );

			// vtx 4
			glColor4d( rect.mColor4.r, rect.mColor4.g, rect.mColor4.b, rect.mColor4.a );
			glVertex3d( rect.mX4, rect.mY4, 0.0 );
		}
	glEnd();
//#endif

	glDisable( GL_BLEND );

	glPopAttrib();

	//const float render2DTime = time.GetTime();
	//LOG("numLines=%i, renderTime=%.3fms", numLines, render2DTime*1000);

	mNumDrawn2DRects += mRects.Size();
	mNumDrawCalls++;

	mRects.Clear(false);
}


void OpenGLWidgetCallback::RenderText(const char* text, float fontSize, const Core::Color& color, float posX, float posY, int alignment = OpenGLWidget::ALIGN_CENTER|OpenGLWidget::ALIGN_MIDDLE)
{
	mParent->RenderText( text, fontSize, color, mOffsetX+posX, mOffsetY+posY, alignment );

	mNumDrawnTexts++;
	mNumDrawCalls++;
}


void OpenGLWidgetCallback::RenderText(const char* text, float fontSize, const QColor& color, float posX, float posY, int alignment = OpenGLWidget::ALIGN_CENTER|OpenGLWidget::ALIGN_MIDDLE)
{
	RenderText( text, fontSize, FromQtColor(color), posX,  posY, alignment );
}


double OpenGLWidgetCallback::CalcTextWidth(const char* text)
{
	return mFontMetrics->width(text);
}


double OpenGLWidgetCallback::GetTextHeight()
{
	return mParent->GetDefaultFontSize();
}


void OpenGLWidgetCallback::RenderSimpleMesh(SimpleMesh* mesh, const Core::Color& color, const QMatrix4x4& worldTM)
{
	glPushMatrix();
	glMultMatrixf( worldTM.data() );

	glColor4f( color.r, color.g, color.b, color.a );
	glBegin( GL_TRIANGLES );
		uint32 index = 0;
		for (uint32 i=0; i<mesh->mNumIndices; ++i)
		{
			index = mesh->mIndices[i];
			glNormal3f( mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z );
			glVertex3f( mesh->mPositions[index].x, mesh->mPositions[index].y, mesh->mPositions[index].z );
		}
	glEnd();

	glPopMatrix();
}


void OpenGLWidgetCallback::RenderSphere(const Core::Vector3& position, float radius, const Core::Color& color)
{
	// setup the world space matrix of the sphere
	QMatrix4x4 sphereMatrix;
	sphereMatrix.translate( QVector3D(position.x, position.y, position.z) );
	sphereMatrix.scale( radius );

	// render the sphere
	RenderSimpleMesh( mSphereMesh, color, sphereMatrix);
}


void OpenGLWidgetCallback::InitSphereMesh()
{
	float radius = 1.0;
	uint32 numSegments = 16;

	uint32 i;

	// calculate the number of vertices and indices
	const uint32 numVertices=  (numSegments - 2) * numSegments + 2;
	uint32 numIndices		=  (numSegments - 3) * 6;
	numIndices				+= (numSegments - 3) * (numSegments - 1) * 6;
	numIndices				+= (numSegments - 1) * 3;
	numIndices				+= (numSegments - 1) * 3;
	numIndices				+= 6;

	// create our util mesh for the sphere
	mSphereMesh = new SimpleMesh(numVertices, numIndices, true);

	// fill the vertices
	for (i=1; i<numSegments-1; ++i)
	{
		const float y = (1.0f - (i / (float)(numSegments-1)) * 2.0f);
		const float r = Math::Sin( Math::ACos(y) ) * radius;

		for (uint32 j=0; j<numSegments; j++)
		{
			const float p = (j / (float)numSegments) * Math::pi * 2.0f;
			const float x = r * Math::Sin(p);
			const float z = r * Math::Cos(p);

			mSphereMesh->mPositions[(i-1) * numSegments + j] = Vector3(x,y * radius,z);
		}
	}

	// the highest and deepest vertices
	mSphereMesh->mPositions[(numSegments-2) * numSegments + 0]	= Vector3(0.0f,  radius, 0.0f);
	mSphereMesh->mPositions[(numSegments-2) * numSegments + 1]	= Vector3(0.0f, -radius, 0.0f);

	// calculate normals
	for (i=0; i<mSphereMesh->mNumVertices; ++i)
		mSphereMesh->mNormals[i] = mSphereMesh->mPositions[i].Normalized();

	// fill the indices
	uint32 c = 0;
	for (i=1; i<numSegments-2; ++i)
	{
		for( uint32 j = 0; j < numSegments - 1; j++ )
		{
			mSphereMesh->mIndices[c++] = (i-1) * numSegments + j;
			mSphereMesh->mIndices[c++] = (i-1) * numSegments + j + 1;
			mSphereMesh->mIndices[c++] = i * numSegments + j;

			mSphereMesh->mIndices[c++] = (i-1) * numSegments + j + 1;
			mSphereMesh->mIndices[c++] = i * numSegments + j + 1;	
			mSphereMesh->mIndices[c++] = i * numSegments + j;
		}

		mSphereMesh->mIndices[c++] = (i-1) * numSegments + numSegments - 1;
		mSphereMesh->mIndices[c++] = (i-1) * numSegments;
		mSphereMesh->mIndices[c++] = i * numSegments + numSegments - 1;

		mSphereMesh->mIndices[c++] = i * numSegments;	
		mSphereMesh->mIndices[c++] = (i-1) * numSegments;
		mSphereMesh->mIndices[c++] = i * numSegments + numSegments - 1;
	}

	// highest and deepest indices
	for (i=0; i<numSegments-1; ++i)
	{
		mSphereMesh->mIndices[c++] = i;
		mSphereMesh->mIndices[c++] = i + 1;
		mSphereMesh->mIndices[c++] = (numSegments-2) * numSegments;
	}

	mSphereMesh->mIndices[c++] = numSegments - 1;
	mSphereMesh->mIndices[c++] = 0;
	mSphereMesh->mIndices[c++] = (numSegments-2) * numSegments;

	for (i=0; i<numSegments-1; ++i)
	{
		mSphereMesh->mIndices[c++] = (numSegments-3) * numSegments + i;
		mSphereMesh->mIndices[c++] = (numSegments-3) * numSegments + i + 1;
		mSphereMesh->mIndices[c++] = (numSegments-2) * numSegments + 1;
	}

	mSphereMesh->mIndices[c++] = (numSegments-3) * numSegments + (numSegments-1);
	mSphereMesh->mIndices[c++] = (numSegments-3) * numSegments;
	mSphereMesh->mIndices[c++] = (numSegments-2) * numSegments + 1;
}


// render the current bounding box of the given actor instance
void OpenGLWidgetCallback::RenderAABB(const AABB& box, const Color& color)
{
	Vector3 min = box.GetMin();
	Vector3 max = box.GetMax();

	// generate our vertices
	Vector3 p[8];
	p[0].Set( min.x, min.y, min.z );
	p[1].Set( max.x, min.y, min.z );
	p[2].Set( max.x, min.y, max.z );
	p[3].Set( min.x, min.y, max.z );
	p[4].Set( min.x, max.y, min.z );
	p[5].Set( max.x, max.y, min.z );
	p[6].Set( max.x, max.y, max.z );
	p[7].Set( min.x, max.y, max.z );

	// OpenGL 1 version
	glColor3f( color.r, color.g, color.b);
	glBegin(GL_LINES);
	Vector3 a, b;

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);

		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);

		glVertex3f(p[3].x, p[3].y, p[3].z);
		glVertex3f(p[0].x, p[0].y, p[0].z);


		glVertex3f(p[4].x, p[4].y, p[4].z);
		glVertex3f(p[5].x, p[5].y, p[5].z);

		glVertex3f(p[5].x, p[5].y, p[5].z);
		glVertex3f(p[6].x, p[6].y, p[6].z);

		glVertex3f(p[6].x, p[6].y, p[6].z);
		glVertex3f(p[7].x, p[7].y, p[7].z);

		glVertex3f(p[7].x, p[7].y, p[7].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);


		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[5].x, p[5].y, p[5].z);

		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[6].x, p[6].y, p[6].z);

		glVertex3f(p[3].x, p[3].y, p[3].z);
		glVertex3f(p[7].x, p[7].y, p[7].z);

	glEnd();
}


// constructor
OpenGLWidgetCallback::SimpleMesh::SimpleMesh(uint32 numVertices, uint32 numIndices, bool hasNormals)
{
	CORE_ASSERT( numVertices > 0 && numIndices % 3 == 0 );

	// copy over the number of vertices and indices
	mNumVertices= numVertices;
	mNumIndices	= numIndices;

	// allocate the buffers
	mPositions	= new Vector3[numVertices];
	mIndices	= new uint32[numIndices];
	if (hasNormals == true)
		mNormals = new Vector3[numVertices];
}


// destructor
OpenGLWidgetCallback::SimpleMesh::~SimpleMesh()
{
	delete[] mPositions;
	delete[] mNormals;
	delete[] mIndices;
}
