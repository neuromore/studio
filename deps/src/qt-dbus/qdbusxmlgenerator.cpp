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

#include <QtCore/qmetaobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>

#include "qdbusinterface_p.h"   // for ANNOTATION_NO_WAIT
#include "qdbusabstractadaptor_p.h" // for QCLASSINFO_DBUS_*
#include "qdbusconnection_p.h"  // for the flags
#include "qdbusmetatype_p.h"
#include "qdbusmetatype.h"
#include "qdbusutil_p.h"

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

extern Q_DBUS_EXPORT QString qDBusGenerateMetaObjectXml(QString interface, const QMetaObject *mo,
                                                       const QMetaObject *base, int flags);

static inline QString typeNameToXml(const char *typeName)
{
    // ### copied from qtextdocument.cpp
    // ### move this into Qt Core at some point
    const QLatin1String plain(typeName);
    QString rich;
    rich.reserve(int(plain.size() * 1.1));
    for (int i = 0; i < plain.size(); ++i) {
        if (plain.at(i) == QLatin1Char('<'))
            rich += QLatin1String("&lt;");
        else if (plain.at(i) == QLatin1Char('>'))
            rich += QLatin1String("&gt;");
        else if (plain.at(i) == QLatin1Char('&'))
            rich += QLatin1String("&amp;");
        else
            rich += plain.at(i);
    }
    return rich;
}

static inline QLatin1String accessAsString(bool read, bool write)
{
    if (read)
        return write ? QLatin1String("readwrite") : QLatin1String("read") ;
    else
        return write ? QLatin1String("write") : QLatin1String("") ;
}

// implement the D-Bus org.freedesktop.DBus.Introspectable interface
// we do that by analysing the metaObject of all the adaptor interfaces

static QString generateInterfaceXml(const QMetaObject *mo, int flags, int methodOffset, int propOffset)
{
    QString retval;

    // start with properties:
    if (flags & (QDBusConnection::ExportScriptableProperties |
                 QDBusConnection::ExportNonScriptableProperties)) {
        for (int i = propOffset; i < mo->propertyCount(); ++i) {

            QMetaProperty mp = mo->property(i);

            if (!((mp.isScriptable() && (flags & QDBusConnection::ExportScriptableProperties)) ||
                  (!mp.isScriptable() && (flags & QDBusConnection::ExportNonScriptableProperties))))
                continue;

            int typeId = mp.userType();
            if (!typeId)
                continue;
            const char *signature = QDBusMetaType::typeToSignature(typeId);
            if (!signature)
                continue;

            retval += QString::fromLatin1("    <property name=\"%1\" type=\"%2\" access=\"%3\"")
                      .arg(QLatin1String(mp.name()),
                           QLatin1String(signature),
                           accessAsString(mp.isReadable(), mp.isWritable()));

            if (QDBusMetaType::signatureToType(signature) == QVariant::Invalid) {
                const char *typeName = QMetaType::typeName(typeId);
                retval += QString::fromLatin1(">\n      <annotation name=\"org.qtproject.QtDBus.QtTypeName\" value=\"%3\"/>\n    </property>\n")
                          .arg(typeNameToXml(typeName));
            } else {
                retval += QLatin1String("/>\n");
            }
        }
    }

    // now add methods:
    for (int i = methodOffset; i < mo->methodCount(); ++i) {
        QMetaMethod mm = mo->method(i);

        bool isSignal;
        if (mm.methodType() == QMetaMethod::Signal)
            // adding a signal
            isSignal = true;
        else if (mm.access() == QMetaMethod::Public && (mm.methodType() == QMetaMethod::Slot || mm.methodType() == QMetaMethod::Method))
            isSignal = false;
        else
            continue;           // neither signal nor public slot

        if (isSignal && !(flags & (QDBusConnection::ExportScriptableSignals |
                                   QDBusConnection::ExportNonScriptableSignals)))
            continue;           // we're not exporting any signals
        if (!isSignal && (!(flags & (QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportNonScriptableSlots)) &&
                          !(flags & (QDBusConnection::ExportScriptableInvokables | QDBusConnection::ExportNonScriptableInvokables))))
            continue;           // we're not exporting any slots or invokables

        // we want to skip non-scriptable stuff as early as possible to avoid bogus warning
        // for methods that are not being exported at all
        bool isScriptable = mm.attributes() & QMetaMethod::Scriptable;
        if (!isScriptable && !(flags & (isSignal ? QDBusConnection::ExportNonScriptableSignals : QDBusConnection::ExportNonScriptableInvokables | QDBusConnection::ExportNonScriptableSlots)))
            continue;

        QString xml = QString::asprintf("    <%s name=\"%s\">\n",
                                        isSignal ? "signal" : "method", mm.name().constData());

        // check the return type first
        int typeId = mm.returnType();
        if (typeId != QMetaType::UnknownType && typeId != QMetaType::Void) {
            const char *typeName = QDBusMetaType::typeToSignature(typeId);
            if (typeName) {
                xml += QString::fromLatin1("      <arg type=\"%1\" direction=\"out\"/>\n")
                       .arg(typeNameToXml(typeName));

                // do we need to describe this argument?
                if (QDBusMetaType::signatureToType(typeName) == QVariant::Invalid)
                    xml += QString::fromLatin1("      <annotation name=\"org.qtproject.QtDBus.QtTypeName.Out0\" value=\"%1\"/>\n")
                        .arg(typeNameToXml(QMetaType::typeName(typeId)));
            } else {
                qWarning() << "Unsupported return type" << typeId << QMetaType::typeName(typeId) << "in method" << mm.name();
                continue;
            }
        }
        else if (typeId == QMetaType::UnknownType) {
            qWarning() << "Invalid return type in method" << mm.name();
            continue;           // wasn't a valid type
        }

        QList<QByteArray> names = mm.parameterNames();
        QVector<int> types;
        QString errorMsg;
        int inputCount = qDBusParametersForMethod(mm, types, errorMsg);
        if (inputCount == -1) {
            qWarning() << "Skipped method" << mm.name() << ":" << qPrintable(errorMsg);
            continue;           // invalid form
        }
        if (isSignal && inputCount + 1 != types.count())
            continue;           // signal with output arguments?
        if (isSignal && types.at(inputCount) == QDBusMetaTypeId::message())
            continue;           // signal with QDBusMessage argument?
        if (isSignal && mm.attributes() & QMetaMethod::Cloned)
            continue;           // cloned signal?

        int j;
        for (j = 1; j < types.count(); ++j) {
            // input parameter for a slot or output for a signal
            if (types.at(j) == QDBusMetaTypeId::message()) {
                isScriptable = true;
                continue;
            }

            QString name;
            if (!names.at(j - 1).isEmpty())
                name = QString::fromLatin1("name=\"%1\" ").arg(QLatin1String(names.at(j - 1)));

            bool isOutput = isSignal || j > inputCount;

            const char *signature = QDBusMetaType::typeToSignature(types.at(j));
            xml += QString::asprintf("      <arg %lstype=\"%s\" direction=\"%s\"/>\n",
                                     qUtf16Printable(name), signature, isOutput ? "out" : "in");

            // do we need to describe this argument?
            if (QDBusMetaType::signatureToType(signature) == QVariant::Invalid) {
                const char *typeName = QMetaType::typeName(types.at(j));
                xml += QString::fromLatin1("      <annotation name=\"org.qtproject.QtDBus.QtTypeName.%1%2\" value=\"%3\"/>\n")
                       .arg(isOutput ? QLatin1String("Out") : QLatin1String("In"))
                       .arg(isOutput && !isSignal ? j - inputCount : j - 1)
                       .arg(typeNameToXml(typeName));
            }
        }

        int wantedMask;
        if (isScriptable)
            wantedMask = isSignal ? QDBusConnection::ExportScriptableSignals
                                  : QDBusConnection::ExportScriptableSlots;
        else
            wantedMask = isSignal ? QDBusConnection::ExportNonScriptableSignals
                                  : QDBusConnection::ExportNonScriptableSlots;
        if ((flags & wantedMask) != wantedMask)
            continue;

        if (qDBusCheckAsyncTag(mm.tag()))
            // add the no-reply annotation
            xml += QLatin1String("      <annotation name=\"" ANNOTATION_NO_WAIT "\""
                                 " value=\"true\"/>\n");

        retval += xml;
        retval += QString::fromLatin1("    </%1>\n")
                  .arg(isSignal ? QLatin1String("signal") : QLatin1String("method"));
    }

    return retval;
}

QString qDBusGenerateMetaObjectXml(QString interface, const QMetaObject *mo,
                                   const QMetaObject *base, int flags)
{
    if (interface.isEmpty())
        // generate the interface name from the meta object
        interface = qDBusInterfaceFromMetaObject(mo);

    QString xml;
    int idx = mo->indexOfClassInfo(QCLASSINFO_DBUS_INTROSPECTION);
    if (idx >= mo->classInfoOffset())
        return QString::fromUtf8(mo->classInfo(idx).value());
    else
        xml = generateInterfaceXml(mo, flags, base->methodCount(), base->propertyCount());

    if (xml.isEmpty())
        return QString();       // don't add an empty interface
    return QString::fromLatin1("  <interface name=\"%1\">\n%2  </interface>\n")
        .arg(interface, xml);
}
#if 0
QString qDBusGenerateMetaObjectXml(QString interface, const QMetaObject *mo, const QMetaObject *base,
                                   int flags)
{
    if (interface.isEmpty()) {
        // generate the interface name from the meta object
        int idx = mo->indexOfClassInfo(QCLASSINFO_DBUS_INTERFACE);
        if (idx >= mo->classInfoOffset()) {
            interface = QLatin1String(mo->classInfo(idx).value());
        } else {
            interface = QLatin1String(mo->className());
            interface.replace(QLatin1String("::"), QLatin1String("."));

            if (interface.startsWith(QLatin1String("QDBus"))) {
                interface.prepend(QLatin1String("org.qtproject.QtDBus."));
            } else if (interface.startsWith(QLatin1Char('Q')) &&
                       interface.length() >= 2 && interface.at(1).isUpper()) {
                // assume it's Qt
                interface.prepend(QLatin1String("org.qtproject.Qt."));
            } else if (!QCoreApplication::instance()||
                       QCoreApplication::instance()->applicationName().isEmpty()) {
                interface.prepend(QLatin1String("local."));
            } else {
                interface.prepend(QLatin1Char('.')).prepend(QCoreApplication::instance()->applicationName());
                QStringList domainName =
                    QCoreApplication::instance()->organizationDomain().split(QLatin1Char('.'),
                                                                             QString::SkipEmptyParts);
                if (domainName.isEmpty())
                    interface.prepend(QLatin1String("local."));
                else
                    for (int i = 0; i < domainName.count(); ++i)
                        interface.prepend(QLatin1Char('.')).prepend(domainName.at(i));
            }
        }
    }

    QString xml;
    int idx = mo->indexOfClassInfo(QCLASSINFO_DBUS_INTROSPECTION);
    if (idx >= mo->classInfoOffset())
        return QString::fromUtf8(mo->classInfo(idx).value());
    else
        xml = generateInterfaceXml(mo, flags, base->methodCount(), base->propertyCount());

    if (xml.isEmpty())
        return QString();       // don't add an empty interface
    return QString::fromLatin1("  <interface name=\"%1\">\n%2  </interface>\n")
        .arg(interface, xml);
}

#endif

QT_END_NAMESPACE

#endif // QT_NO_DBUS
