/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusextratypes.h"
#include "qdbusutil_p.h"

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

void QDBusObjectPath::doCheck()
{
    if (!QDBusUtil::isValidObjectPath(m_path)) {
        qWarning("QDBusObjectPath: invalid path \"%s\"", qPrintable(m_path));
        m_path.clear();
    }
}

void QDBusSignature::doCheck()
{
    if (!QDBusUtil::isValidSignature(m_signature)) {
        qWarning("QDBusSignature: invalid signature \"%s\"", qPrintable(m_signature));
        m_signature.clear();
    }
}

/*!
    \class QDBusVariant
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusVariant class enables the programmer to identify
    the variant type provided by the D-Bus typesystem.

    A D-Bus function that takes an integer, a D-Bus variant and a string as parameters
    can be called with the following argument list (see QDBusMessage::setArguments()):

    \snippet qdbusextratypes/qdbusextratypes.cpp 0

    When a D-Bus function returns a D-Bus variant, it can be retrieved as follows:

    \snippet qdbusextratypes/qdbusextratypes.cpp 1

    The QVariant within a QDBusVariant is required to distinguish between a normal
    D-Bus value and a value within a D-Bus variant.

    \sa {The Qt D-Bus Type System}
*/

/*!
    \fn QDBusVariant::QDBusVariant()

    Constructs a new D-Bus variant.
*/

/*!
    \fn QDBusVariant::QDBusVariant(const QVariant &variant)

    Constructs a new D-Bus variant from the given Qt \a variant.

    \sa setVariant()
*/

/*!
    \fn QVariant QDBusVariant::variant() const

    Returns this D-Bus variant as a QVariant object.

    \sa setVariant()
*/

/*!
    \fn void QDBusVariant::setVariant(const QVariant &variant)

    Assigns the value of the given Qt \a variant to this D-Bus variant.

    \sa variant()
*/

/*!
    \class QDBusObjectPath
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusObjectPath class enables the programmer to
    identify the OBJECT_PATH type provided by the D-Bus typesystem.

    \sa {The Qt D-Bus Type System}
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath()

    Constructs a new object path.
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(const char *path)

    Constructs a new object path from the given \a path.

    \sa setPath()
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(QLatin1String path)

    Constructs a new object path from the given \a path.
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(const QString &path)

    Constructs a new object path from the given \a path.
*/

/*!
    \fn QString QDBusObjectPath::path() const

    Returns this object path.

    \sa setPath()
*/

/*!
    \fn void QDBusObjectPath::setPath(const QString &path)

    Assigns the value of the given \a path to this object path.

    \sa path()
*/

/*!
    \class QDBusSignature
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusSignature class enables the programmer to
    identify the SIGNATURE type provided by the D-Bus typesystem.

    \sa {The Qt D-Bus Type System}
*/

/*!
    \fn QDBusSignature::QDBusSignature()

    Constructs a new signature.

    \sa setSignature()
*/

/*!
    \fn QDBusSignature::QDBusSignature(const char *signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QDBusSignature::QDBusSignature(QLatin1String signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QDBusSignature::QDBusSignature(const QString &signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QString QDBusSignature::signature() const

    Returns this signature.

    \sa setSignature()
*/

/*!
    \fn void QDBusSignature::setSignature(const QString &signature)

    Assigns the value of the given \a signature to this signature.
    \sa signature()
*/

/*!
    \fn void QDBusObjectPath::swap(QDBusObjectPath &other)

    Swaps this QDBusObjectPath instance with \a other.
*/

/*!
    \fn void QDBusSignature::swap(QDBusSignature &other)

    Swaps this QDBusSignature instance with \a other.
*/

/*!
    \fn void QDBusVariant::swap(QDBusVariant &other)

    Swaps this QDBusVariant instance with \a other.
*/

QT_END_NAMESPACE

#endif // QT_NO_DBUS
