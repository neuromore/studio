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

#ifndef __NEUROMORE_OPENGLWIDGETCALLBACK_H
#define __NEUROMORE_OPENGLWIDGETCALLBACK_H

#include "../Config.h"
#include <Core/Timer.h>
#include <Core/Array.h>
#include <Core/AABB.h>
#include <DSP/Channel.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>


// forward declaration
class OpenGLWidget;

class OpenGLWidgetCallback
{
	public:
		// constructor & destructor
		OpenGLWidgetCallback(OpenGLWidget* parent);
		virtual ~OpenGLWidgetCallback();

		virtual void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)			{ SetOffset(x, y); }
		virtual void RenderTimeline(double x, double y, double width, double height)									{ SetOffset(x, y); }

		// the offset in case we render multiple viewports within a single widget
		void SetOffset(float offsetX, float offsetY)																	{ mOffsetX = offsetX; mOffsetY = offsetY; }

		// line rendering
		void AddLine(double x1, double y1, const Core::Color& color1, double x2, double y2, const Core::Color& color2);
		void RenderLines(double lineWidth=1.0);


		struct Rect
		{
			double				mX1;
			double				mY1;
			Core::Color		mColor1;

			double				mX2;
			double				mY2;
			Core::Color		mColor2;

			double				mX3;
			double				mY3;
			Core::Color		mColor3;

			double				mX4;
			double				mY4;
			Core::Color		mColor4;
		};

		// rect rendering
		void AddRect(const Rect& rect);
		void AddRect(double x, double y, double width, double height, const Core::Color& color);
		void RenderRects();


		// text rendering
		void RenderText(const char* text, float fontSize, const Core::Color& color, float posX, float posY, int alignment);
		void RenderText(const char* text, float fontSize, const QColor& color, float posX, float posY, int alignment);

		double CalcTextWidth(const char* text);
		double GetTextHeight();
		const QFont& GetFont() const															{ return *mFont; }
		const QFontMetrics& GetFontMetrics() const												{ return *mFontMetrics; }


		// performance statistics
		uint32 GetNumDrawn2DLines() const		{ return mNumDrawn2DLines; }
		uint32 GetNumDrawn2DRects() const		{ return mNumDrawn2DRects; }
		uint32 GetNumDrawnTexts() const			{ return mNumDrawnTexts; }
		uint32 GetNumDrawCalls() const			{ return mNumDrawCalls; }

		// 3D

		// sphere rendering
		struct SimpleMesh
		{
			SimpleMesh(uint32 numVertices, uint32 numIndices, bool hasNormals);
			~SimpleMesh();

			Core::Vector3*		mPositions;		
			uint32*				mIndices;		
			Core::Vector3*		mNormals;		
			uint32				mNumVertices;	
			uint32				mNumIndices;	
		};

		void RenderSimpleMesh(SimpleMesh* mesh, const Core::Color& color, const QMatrix4x4& worldTM);
		void RenderSphere(const Core::Vector3& position, float radius, const Core::Color& color);

		// AABB rendering
		void RenderAABB(const Core::AABB& box, const Core::Color& color);

		void ResetPerformanceStatistics();

		OpenGLWidget* GetOpenGLWidget() const													{ return mParent; }

		void SetPainter(QPainter* painter)														{ mPainter = painter; }
		QPainter* GetPainter()																	{ return mPainter; }

	protected:
		struct Line2D
		{
			double				x1;	/**< The x position of the first vertex. */
			double				y1;	/**< The y position of the first vertex. */
			Core::Color		color1;
			double				x2;	/**< The x position of the second vertex. */
			double				y2;	/**< The y position of the second vertex. */
			Core::Color		color2;
		};

		QPainter*					mPainter;

		Core::Array<Line2D>			mLines;
		Core::Array<Rect>			mRects;

		// sphere
		void InitSphereMesh();
		SimpleMesh*					mSphereMesh;

		// text rendering
		QFont*						mFont;
		QFontMetrics*				mFontMetrics;

		OpenGLWidget*				mParent;
		float						mOffsetX;
		float						mOffsetY;

		// performance statistics
		uint32						mNumDrawn2DLines;
		uint32						mNumDrawn2DRects;
		uint32						mNumDrawnTexts;
		uint32						mNumDrawCalls;
};


#endif
