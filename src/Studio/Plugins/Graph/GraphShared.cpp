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
#include "GraphShared.h"
#include "GraphHelpers.h"
#include <QtBaseManager.h>
#include <ColorPalette.h>
#include <Core/Math.h>


using namespace Core;

// constructor
GraphShared::GraphShared()
{
	// colors
	mReadOnlyColor			= QColor(112,111,111);
	mBackgroundColor		= ColorPalette::Shared::GetDarkBackgroundQColor();
	mGridColor				= ColorPalette::Shared::GetDarkGridQColor();
	mSubGridColor			= ColorPalette::Shared::GetDarkGridQColor();

#ifdef SCREENSHOT_MODE
	// completely white background
	mBackgroundColor		= QColor(255, 255, 255);
	mGridColor				= QColor(255, 255, 255);
	mSubGridColor			= QColor(255, 255, 255);
#endif

	mActiveBorderColor		= QColor(120, 120, 120);
	mPortColor				= QColor(80, 80, 80);
	mConnectionBorderColor	= QColor(80, 80, 80);
	mSelectedColor			= QColor(66, 190, 247);
	mConnectionColor		= QColor(130,130,130);
	mHighlightFactor		= 160;

	// fonts and metrics
	mNodeHeaderFont.setBold( true );
	mNodeInfoFont.setBold( true );

	mNodePortMetrics		= new QFontMetrics(mNodePortFont);
	mNodeHeaderMetrics		= new QFontMetrics(mNodeHeaderFont);
	mNodeInfoMetrics		= new QFontMetrics(mNodeInfoFont);
	mNodeSubTitleMetrics	= new QFontMetrics(mNodeSubTitleFont);
	mStandardFontMetrics	= new QFontMetrics(mStandardFont);

	// animated error color
	mErrorBlinkOffset		= 0.0f;

	// animated dashed connection offset
	mConnectionDashOffset	= 0.0f;

	// create connection dash patterns
	QVector<qreal> dashPattern;
	dashPattern << 0.5 << 1;
	mConnectionDashPatterns.Add( dashPattern );

	// animate states
	mActiveStateAnimationOffset = 0.0;
	mActiveStateAnimationPhaseA = true;
	mAnimationScaleActive		= false;

	// set to invalid screen dpi so that the first SetScreenDPI actually updates everything
	mScreenDPI = -1.0;
}


// destructor
GraphShared::~GraphShared()
{
	// delete the font metrics
	delete mNodePortMetrics;
	delete mNodeHeaderMetrics;
	delete mNodeInfoMetrics;
	delete mNodeSubTitleMetrics;
}


// main update method
void GraphShared::Update(double timeDelta)
{
	// update the animated error timer
	mErrorBlinkOffset += timeDelta;
	if (mErrorBlinkOffset >= Math::twoPi)
		mErrorBlinkOffset = Math::FMod(mErrorBlinkOffset, Math::twoPi);

	// control the scroll speed of the dashed connections etc
	mConnectionDashOffset -= (15.0 * GetScreenScaling()) * timeDelta;

	// control the animation speed of the active state
	const double activeStateAnimSpeed = 1.0;
	mActiveStateAnimationOffset += activeStateAnimSpeed * timeDelta;
	if (mActiveStateAnimationOffset > 1.0)
	{
		mActiveStateAnimationPhaseA = !mActiveStateAnimationPhaseA;
		mActiveStateAnimationOffset = Math::FMod( mActiveStateAnimationOffset, 1.0 );
	}

	// scaling animation
	if (mAnimationScaleActive == true)
	{
		const float scaleAnimationSpeed = 20.0f;
		mAnimationScaleTime += scaleAnimationSpeed * timeDelta;
		
		mAnimationScaleCurrentStart = LinearInterpolate<float>( mAnimationScaleCurrentStart, mAnimationScaleTargetStart, scaleAnimationSpeed * timeDelta );
		mScale = EaseInOutInterpolate<float>( mAnimationScaleCurrentStart, mAnimationScaleTarget, mAnimationScaleTime );

		if (mAnimationScaleTime > 1.0f)
		{
			mScale					= mAnimationScaleTarget;
			mAnimationScaleActive	= false;
		}
	}

	// update the transform
	mTransform.reset();
	mTransform.translate( mScalePivot.x(), mScalePivot.y() );
	mTransform.scale( mScale, mScale );
	mTransform.translate( -mScalePivot.x() + mScrollOffset.x(), -mScalePivot.y() + mScrollOffset.y() );
}


void GraphShared::ResetTransform()
{
	mScale			= 1.0f;
	mScrollOffset	= QPoint(0.0f, 0.0f);
	mScalePivot		= QPoint(0.0f, 0.0f);
}


// get the border and background color for a node port
void GraphShared::GetNodePortColors(bool isWidgetEnabled, const Port& nodePort, const QColor& borderColor, const QColor& headerBgColor, const QColor& textColor, QColor* outBrushColor, QColor* outPenColor, QColor* outTextColor, bool isNodeSelected, bool isPortHighlighted)
{
	if (isNodeSelected == true)
	{
		*outPenColor = headerBgColor.lighter(120);
		*outBrushColor	= headerBgColor;
	}
	else
	{
		if (isPortHighlighted == true || nodePort.HasConnection() == true)
		{
			*outPenColor = borderColor;
			*outBrushColor = borderColor;
		}
		else
		{
			*outPenColor = mPortColor.lighter(120);
			*outBrushColor	= mPortColor;
		}
	}

	*outTextColor = textColor;
	//if (isPortHighlighted == true)
	//	*outTextColor = textColor.darker(mHighlightFactor);

	if (isWidgetEnabled == false)
	{
		*outBrushColor	= mReadOnlyColor;
		*outPenColor	= mReadOnlyColor;
		*outTextColor	= mReadOnlyColor;
	}
}


void GraphShared::SetScale(float scale, bool animate)
{
	if (animate == false) 
	{ 
		mScale = scale; 
	} 
	else
	{
		if (mAnimationScaleActive == false) 
		{
			mAnimationScaleTime	= 0.0f;
			mAnimationScaleCurrentStart = mScale;
		}
		
		mAnimationScaleTargetStart = mScale;
		mAnimationScaleTarget	= scale;
		mAnimationScaleActive	= true;
	} 
}


void GraphShared::SetScreenInfo(float physicalScreenWidthInMillimeters, float physicalScreenHeightInMillimeters, float dpi)
{
	const float oldScreenDPI = mScreenDPI;
	mScreenDPI = dpi;

	//LogInfo( "Screen: DPI=%f, WidthInCm=%f, HeightInCm=%f", dpi, physicalScreenWidthInMillimeters*0.1, physicalScreenHeightInMillimeters*0.1 );

	const float screenWidthMeter = physicalScreenWidthInMillimeters * 0.001f;

	// 1920 width at 24' has a value of 48.0
	// 2560 width at 27' (iMac) has a value of 61.0
	// 5k iMac at 27' has a value of 130.0
	const float screenEvaluator = (screenWidthMeter * dpi);
	
	if (screenEvaluator <= 100.0f)
		mScreenScale = 1.0f;
	else
		mScreenScale = 2.0f;

	//LogDebug("RawScale=%f", mScreenScale);
	//mScreenScale = Core::Clamp( mScreenScale, 1.0f, 100.0f );
	//mScreenScale = Math::Floor( mScreenScale + 0.5f );
	//LogDebug("FinalScale=%f", mScreenScale);

	if (IsClose<float>(oldScreenDPI, dpi, Math::epsilon) == false)
	{
		LogInfo("Automatically scaled graph based on screen information ...");
		mNodeHeaderFont.setPixelSize( 11.0f * GetScreenScaling() );
		mNodePortFont.setPixelSize( 9.0f * GetScreenScaling() );
		mNodeInfoFont.setPixelSize( 10.0f * GetScreenScaling() );
		mNodeSubTitleFont.setPixelSize( 10.0f * GetScreenScaling() );
		mStandardFont.setPixelSize( GetQtBaseManager()->GetMainWindow()->GetDefaultFontSize() );

		delete mNodePortMetrics;
		delete mNodeHeaderMetrics;
		delete mNodeInfoMetrics;
		delete mNodeSubTitleMetrics;
		delete mStandardFontMetrics;

		mNodePortMetrics		= new QFontMetrics(mNodePortFont);
		mNodeHeaderMetrics		= new QFontMetrics(mNodeHeaderFont);
		mNodeInfoMetrics		= new QFontMetrics(mNodeInfoFont);
		mNodeSubTitleMetrics	= new QFontMetrics(mNodeSubTitleFont);
		mStandardFontMetrics	= new QFontMetrics(mStandardFont);
	}
}
