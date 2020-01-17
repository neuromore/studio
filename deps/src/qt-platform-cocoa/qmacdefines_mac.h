/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/****************************************************************************
**
** Copyright (c) 2007-2008, Apple, Inc.
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**   * Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**   * Redistributions in binary form must reproduce the above copyright notice,
**     this list of conditions and the following disclaimer in the documentation
**     and/or other materials provided with the distribution.
**
**   * Neither the name of Apple, Inc. nor the names of its contributors
**     may be used to endorse or promote products derived from this software
**     without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

/*
 *  qmacdefines_mac_p.h
 *  All the defines you'll ever need for Qt/Mac :-)
 */

/* This is just many defines. Therefore it doesn't need things like:
QT_BEGIN_NAMESPACE


QT_END_NAMESPACE

Yes, it is an informative comment ;-)
*/

#include <QtCore/qglobal.h>

#ifdef __LP64__
typedef signed int OSStatus;
#else
typedef signed long OSStatus;
#endif

typedef struct OpaqueEventHandlerCallRef * EventHandlerCallRef;
typedef struct OpaqueEventRef * EventRef;
typedef struct OpaqueMenuRef * MenuRef;
typedef struct OpaquePasteboardRef* PasteboardRef;
typedef struct OpaqueRgnHandle * RgnHandle;
typedef const struct __HIShape *HIShapeRef;
typedef struct __HIShape *HIMutableShapeRef;
typedef struct CGRect CGRect;
typedef struct CGImage *CGImageRef;
typedef struct CGContext *CGContextRef;
typedef struct GDevice * GDPtr;
typedef GDPtr * GDHandle;
typedef struct OpaqueIconRef * IconRef;

#ifdef __OBJC__
typedef NSWindow* OSWindowRef;
typedef NSView *OSViewRef;
typedef NSMenu *OSMenuRef;
typedef NSEvent *OSEventRef;
#else
typedef void *OSWindowRef;
typedef void *OSViewRef;
typedef void *OSMenuRef;
typedef void *OSEventRef;
#endif

typedef PasteboardRef OSPasteboardRef;
typedef struct AEDesc AEDescList;
typedef AEDescList AERecord;
typedef AERecord AppleEvent;

#ifdef check
#undef check
#endif
