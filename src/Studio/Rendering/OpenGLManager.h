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

#ifndef __NEUROMORE_OPENGLMANAGER_H
#define __NEUROMORE_OPENGLMANAGER_H

// include required headers
#include "../Config.h"
#include <QObject>
#include <QTimer>
#include <QMainWindow>
#include <Core/Timer.h>
#include "OpenGLWidget.h"
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>


#ifdef CORE_DEBUG
	// TODO: Crashes when closing in the following case: Load classifier, undock graph window, maximize graph window, close graph window, close Studio
	//#define OPENGL_DEBUG_MESSAGES
#endif


class OpenGLItem : public QObject
{
	Q_OBJECT
	public:
		OpenGLItem(QOpenGLWidget* widget, OpenGLWidget* glWidget);
		virtual ~OpenGLItem();

		QOpenGLWidget* GetBaseWidget() const			{ return mWidget; }
		OpenGLWidget* GetOpenGLWidget() const			{ return mOpenGLWidget; }
		QOpenGLContext* GetContext() const				{ return mWidget->context(); }

		void Log() const;
	
	public slots:
		void OnOpenGLDebugMessageLogged(const QOpenGLDebugMessage& message);
		
	private:
#ifdef OPENGL_DEBUG_MESSAGES
		QOpenGLDebugLogger* mOpenGLDebugLogger;
#endif
		QOpenGLWidget*		mWidget;
		OpenGLWidget*		mOpenGLWidget;
};


class OpenGLManager : public QObject
{
	Q_OBJECT
	public:
		OpenGLManager();
		virtual ~OpenGLManager();

		void RegisterOpenGLWidget(QOpenGLWidget* widget, OpenGLWidget* glWidget=NULL);
		void UnregisterOpenGLWidget(QOpenGLWidget* widget);

		const QSurfaceFormat& GetDefaultSurfaceFormat() const					{ return mDefaultSurfaceFormat; }

		static void LogOpenGLContext(QOpenGLContext* context);
		static void LogSurfaceFormat(const QSurfaceFormat& format);

		void OnScheduleAsyncReInit(OpenGLWidget* widget);

	private:
		uint32 FindOpenGLItemIndex(QOpenGLWidget* widget);

		Core::Array<OpenGLItem*>	mOpenGLItems;
		QSurfaceFormat				mDefaultSurfaceFormat;
};


#endif
