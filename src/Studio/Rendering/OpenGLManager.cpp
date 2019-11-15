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
#include "OpenGLManager.h"
#include <QtBaseManager.h>
#include <QScreen>
#include <QSurface>


using namespace Core;

// constructor
OpenGLManager::OpenGLManager()
{
	LogDetailedInfo("Constructing OpenGL manager ...");

	// OpenGL surface format definition
    mDefaultSurfaceFormat.setSamples(4);
	mDefaultSurfaceFormat.setSwapInterval(0); // disables vsync
	//mDefaultSurfaceFormat.setMajorVersion(3);
	//mDefaultSurfaceFormat.setMinorVersion(2);
	mDefaultSurfaceFormat.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
	mDefaultSurfaceFormat.setRenderableType( QSurfaceFormat::OpenGL );
#ifdef OPENGL_DEBUG_MESSAGES
	mDefaultSurfaceFormat.setOption(QSurfaceFormat::DebugContext);
#endif

	// set the default surface format
    QSurfaceFormat::setDefaultFormat( mDefaultSurfaceFormat );
}


// destructor
OpenGLManager::~OpenGLManager()
{
	LogDetailedInfo("Destructing OpenGL manager ...");

	// get rid of the OpenGL items
	const uint32 numItems = mOpenGLItems.Size();
	for (uint32 i=0; i<numItems; ++i)
		delete mOpenGLItems[i];
	mOpenGLItems.Clear();
}


void OpenGLManager::RegisterOpenGLWidget(QOpenGLWidget* widget, OpenGLWidget* glWidget)
{
	OpenGLItem* newItem = new OpenGLItem(widget, glWidget);
	newItem->Log();

	// add a OpenGL item for the given widget
	mOpenGLItems.Add( newItem );
}


void OpenGLManager::UnregisterOpenGLWidget(QOpenGLWidget* widget)
{
	const uint32 itemIndex = FindOpenGLItemIndex(widget);
	if (itemIndex != CORE_INVALIDINDEX32)
	{
		delete mOpenGLItems[itemIndex];
		mOpenGLItems.Remove(itemIndex);
	}
}


uint32 OpenGLManager::FindOpenGLItemIndex(QOpenGLWidget* widget)
{
	// get the number of OpenGL items and iterate through them
	const uint32 numItems = mOpenGLItems.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		if (mOpenGLItems[i]->GetBaseWidget() == widget)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// log surface format information
void OpenGLManager::LogSurfaceFormat(const QSurfaceFormat& format)
{
	LogInfo( "Surface format information:" );

	// renderable type
	switch (format.renderableType())
	{
		case QSurfaceFormat::DefaultRenderableType:	{ LogInfo( "   Rendering Backend: Default (unspecified rendering method)" ); break;}
		case QSurfaceFormat::OpenGL:				{ LogInfo( "   Rendering Backend: OpenGL" ); break;}
		case QSurfaceFormat::OpenGLES:				{ LogInfo( "   Rendering Backend: OpenGL ES" ); break;}
		case QSurfaceFormat::OpenVG:				{ LogInfo( "   Rendering Backend: Open Vector Graphics" ); break;}
	}

	// version
	LogInfo( "   Version: v%i.%i", format.majorVersion(), format.minorVersion() );

	// context profile
	switch (format.profile())
	{
		case QSurfaceFormat::NoProfile:				{ LogInfo( "   Context Profile: No Profile (OpenGL version is lower than 3.2)" ); break;}
		case QSurfaceFormat::CoreProfile:			{ LogInfo( "   Context Profile: Core Profile (Functionality deprecated in OpenGL version 3.0 is not available)" ); break;}
		case QSurfaceFormat::CompatibilityProfile:	{ LogInfo( "   Context Profile: Compatibility Profile (Functionality from earlier OpenGL versions is available)" ); break;}
	}

	// format option
	if (format.options() & QSurfaceFormat::StereoBuffers)		LogInfo( "   Options: Stereo Buffers" );
	if (format.options() & QSurfaceFormat::DebugContext)		LogInfo( "   Options: Debug Context (with extra debugging information)" );
	if (format.options() & QSurfaceFormat::DeprecatedFunctions)	LogInfo( "   Options: Deprecated Function Support" );

	// color buffer
	LogInfo("   Color Buffer Size (r, g, b): (%i, %i, %i)", format.redBufferSize(), format.blueBufferSize(), format.greenBufferSize() );

	// alpha buffer
	if (format.hasAlpha() == true)
	{
		LogDetailedInfo( "   Has Alpha Buffer: Yes" );
		LogDetailedInfo( "   Alpha Buffer Size: %i", format.alphaBufferSize() );
	}
	else
		LogDetailedInfo( "   Has Alpha Buffer: No" );

	// depth buffer
	LogDetailedInfo( "   Depth Buffer Size: %i", format.depthBufferSize() );

	// stencil buffer
	LogDetailedInfo( "   Stencil Buffer Size: %i", format.stencilBufferSize() );

	// swap behavior
	switch (format.swapBehavior())
	{
		case QSurfaceFormat::DefaultSwapBehavior:	{ LogInfo( "   Swap Behavior: Default" ); break;}
		case QSurfaceFormat::SingleBuffer:			{ LogInfo( "   Swap Behavior: Single Buffering (might flicker)" ); break;}
		case QSurfaceFormat::DoubleBuffer:			{ LogInfo( "   Swap Behavior: Double Buffering" ); break;}
		case QSurfaceFormat::TripleBuffer:			{ LogInfo( "   Swap Behavior: Triple Buffering" ); break;}
	}

	// vsync and buffer swapping interval
	LogInfo( "   Swap Interval: %i", format.swapInterval() );

	// multi sampling
	LogInfo( "   Num Multisamples: %i", format.swapInterval() );

	// stereo buffering (what's that? same as double buffering!?)
	LogDetailedInfo( "   Stereo Buffering Enabled: %s", format.stereo() ? "Yes" : "No" );
}


// log OpenGL context information
void OpenGLManager::LogOpenGLContext(QOpenGLContext* context)
{
	LogInfo( "OpenGL Context:" );

	LogInfo( "   Is Valid: %s", context->isValid() ? "Yes" : "No" );
	LogInfo( "   Is OpenGL ES: %s", context->isOpenGLES() ? "Yes" : "No" );

	// share context
	QOpenGLContext*	shareContext = context->shareContext();
	if (shareContext == NULL)
		LogInfo( "   Share Context: None" );
	else
	{
		LogInfo( "   Share Context: 0x%08x", shareContext );
	}

	// share group
	QOpenGLContextGroup* shareGroup = context->shareGroup();
	if (shareGroup == NULL)
		LogInfo( "   Share Group: None" );
	else
	{
		LogInfo( "   Share Group: 0x%08x", shareGroup );
	}

	// surface
	//QSurface* surface = context->surface();
	//LogInfo( "   Surface: Size=(%i, %i), OpenGLSupport=%s", surface->size().width(), surface->size().height(), surface->supportsOpenGL() ? "Yes" : "No" );

	// screen
	QScreen* screen = context->screen();
	LogDetailedInfo( "   Screen: Size=(%i, %i), PixelRatio=%f", screen->size().width(), screen->size().height(), screen->devicePixelRatio() );

	// extensions
	//LogDebug( "   Extensions:" );
	//QList<QByteArray> extensions = context->extensions().toList();
	//const int numExtensions = extensions.count();
	//for (int i=0; i<numExtensions; ++i)
	//	LogDebug( "      %s", extensions[i].data() );
}


void OpenGLManager::OnScheduleAsyncReInit(OpenGLWidget* widget)
{
	const uint32 numItems = mOpenGLItems.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		if (mOpenGLItems[i]->GetOpenGLWidget() == NULL)
			continue;

		mOpenGLItems[i]->GetOpenGLWidget()->ScheduleAsyncReInit();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
OpenGLItem::OpenGLItem(QOpenGLWidget* widget, OpenGLWidget* glWidget)
{
	mWidget = widget;
	mOpenGLWidget = glWidget;

#ifdef OPENGL_DEBUG_MESSAGES
	mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
	connect( mOpenGLDebugLogger, SIGNAL(messageLogged(const QOpenGLDebugMessage&)), this, SLOT(OnOpenGLDebugMessageLogged(const QOpenGLDebugMessage&)), Qt::DirectConnection );
	if (mOpenGLDebugLogger->initialize() == true)
	{
		mOpenGLDebugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
		mOpenGLDebugLogger->enableMessages();
	}
#endif
}


// destructor
OpenGLItem::~OpenGLItem()
{
#ifdef OPENGL_DEBUG_MESSAGES
	// TODO: Crashes when closing in the following case: Load classifier, undock graph window, maximize graph window, close graph window, close Studio
	mOpenGLDebugLogger->deleteLater();
#endif
}


// log OpenGL debug message
void OpenGLItem::OnOpenGLDebugMessageLogged(const QOpenGLDebugMessage& message)
{
#ifdef OPENGL_DEBUG_MESSAGES
	LogDebug( FromQtString(message.message()).AsChar() );
#endif
}


// log the OpenGL context information
void OpenGLItem::Log() const
{
	OpenGLManager::LogOpenGLContext( GetContext() );
	OpenGLManager::LogSurfaceFormat( mWidget->format() );
}
