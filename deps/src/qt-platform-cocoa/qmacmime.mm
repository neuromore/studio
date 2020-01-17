/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <ImageIO/ImageIO.h>

#include <QtCore/qsystemdetection.h>
#include <QtGui/qimage.h>

#if defined(Q_OS_OSX)
#import <AppKit/AppKit.h>
#else
#include <MobileCoreServices/MobileCoreServices.h>
#endif

#if defined(QT_PLATFORM_UIKIT)
#import <UIKit/UIKit.h>
#endif

#include "qmacmime_p.h"
#include "qguiapplication.h"
#include "private/qcore_mac_p.h"

QT_BEGIN_NAMESPACE

typedef QList<QMacInternalPasteboardMime*> MimeList;
Q_GLOBAL_STATIC(MimeList, globalMimeList)
Q_GLOBAL_STATIC(QStringList, globalDraggedTypesList)

void qt_mac_addToGlobalMimeList(QMacInternalPasteboardMime *macMime)
{
    // globalMimeList is in decreasing priority order. Recently added
    // converters take prioity over previously added converters: prepend
    // to the list.
    globalMimeList()->prepend(macMime);
}

void qt_mac_removeFromGlobalMimeList(QMacInternalPasteboardMime *macMime)
{
    if (!QGuiApplication::closingDown())
        globalMimeList()->removeAll(macMime);
}

/*!
    \fn void qRegisterDraggedTypes(const QStringList &types)
    \relates QMacPasteboardMime

    Registers the given \a types as custom pasteboard types.

    This function should be called to enable the Drag and Drop events
    for custom pasteboard types on Cocoa implementations. This is required
    in addition to a QMacPasteboardMime subclass implementation. By default
    drag and drop is enabled for all standard pasteboard types.

   \sa QMacPasteboardMime
*/
void qt_mac_registerDraggedTypes(const QStringList &types)
{
    (*globalDraggedTypesList()) += types;
}

const QStringList& qt_mac_enabledDraggedTypes()
{
    return (*globalDraggedTypesList());
}

/*****************************************************************************
  QDnD debug facilities
 *****************************************************************************/
//#define DEBUG_MIME_MAPS

/*!
  \class QMacPasteboardMime
  \brief The QMacPasteboardMime class converts between a MIME type and a
  \l{http://developer.apple.com/macosx/uniformtypeidentifiers.html}{Uniform
  Type Identifier (UTI)} format.
  \since 4.2

  \ingroup draganddrop
  \inmodule QtWidgets

  Qt's drag and drop and clipboard facilities use the MIME
  standard. On X11, this maps trivially to the Xdnd protocol. On
  Mac, although some applications use MIME to describe clipboard
  contents, it is more common to use Apple's UTI format.

  QMacPasteboardMime's role is to bridge the gap between MIME and UTI;
  By subclasses this class, one can extend Qt's drag and drop
  and clipboard handling to convert to and from unsupported, or proprietary, UTI formats.

  A subclass of QMacPasteboardMime will automatically be registered, and active, upon instantiation.

  Qt has predefined support for the following UTIs:
  \list
    \i public.utf8-plain-text - converts to "text/plain"
    \i public.utf16-plain-text - converts to "text/plain"
    \i public.text - converts to "text/plain"
    \i public.html - converts to "text/html"
    \i public.url - converts to "text/uri-list"
    \i public.file-url - converts to "text/uri-list"
    \i public.tiff - converts to "application/x-qt-image"
    \i public.vcard - converts to "text/plain"
    \i com.apple.traditional-mac-plain-text - converts to "text/plain"
    \i com.apple.pict - converts to "application/x-qt-image"
  \endlist

  When working with MIME data, Qt will interate through all instances of QMacPasteboardMime to
  find an instance that can convert to, or from, a specific MIME type. It will do this by calling
  canConvert() on each instance, starting with (and choosing) the last created instance first.
  The actual conversions will be done by using convertToMime() and convertFromMime().

  \note The API uses the term "flavor" in some cases. This is for backwards
  compatibility reasons, and should now be understood as UTIs.
*/

/*! \enum QMacPasteboardMime::QMacPasteboardMimeType
    \internal
*/

/*!
  Constructs a new conversion object of type \a t, adding it to the
  globally accessed list of available convertors.
*/
QMacInternalPasteboardMime::QMacInternalPasteboardMime(char t) : type(t)
{
    qt_mac_addToGlobalMimeList(this);
}

/*!
  Destroys a conversion object, removing it from the global
  list of available convertors.
*/
QMacInternalPasteboardMime::~QMacInternalPasteboardMime()
{
    qt_mac_removeFromGlobalMimeList(this);
}

/*!
  Returns the item count for the given \a mimeData
*/
int QMacInternalPasteboardMime::count(QMimeData *mimeData)
{
    Q_UNUSED(mimeData);
    return 1;
}

class QMacPasteboardMimeAny : public QMacInternalPasteboardMime {
private:

public:
    QMacPasteboardMimeAny() : QMacInternalPasteboardMime(MIME_QT_CONVERTOR|MIME_ALL) {
    }
    ~QMacPasteboardMimeAny() {
    }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeAny::convertorName()
{
    return QLatin1String("Any-Mime");
}

QString QMacPasteboardMimeAny::flavorFor(const QString &mime)
{
    // do not handle the mime type name in the drag pasteboard
    if (mime == QLatin1String("application/x-qt-mime-type-name"))
        return QString();
    QString ret = QLatin1String("com.trolltech.anymime.") + mime;
    return ret.replace(QLatin1Char('/'), QLatin1String("--"));
}

QString QMacPasteboardMimeAny::mimeFor(QString flav)
{
    const QString any_prefix = QLatin1String("com.trolltech.anymime.");
    if (flav.size() > any_prefix.length() && flav.startsWith(any_prefix))
        return flav.mid(any_prefix.length()).replace(QLatin1String("--"), QLatin1String("/"));
    return QString();
}

bool QMacPasteboardMimeAny::canConvert(const QString &mime, QString flav)
{
    return mimeFor(flav) == mime;
}

QVariant QMacPasteboardMimeAny::convertToMime(const QString &mime, QList<QByteArray> data, QString)
{
    if (data.count() > 1)
        qWarning("QMacPasteboardMimeAny: Cannot handle multiple member data");
    QVariant ret;
    if (mime == QLatin1String("text/plain"))
        ret = QString::fromUtf8(data.first());
    else
        ret = data.first();
    return ret;
}

QList<QByteArray> QMacPasteboardMimeAny::convertFromMime(const QString &mime, QVariant data, QString)
{
    QList<QByteArray> ret;
    if (mime == QLatin1String("text/plain"))
        ret.append(data.toString().toUtf8());
    else
        ret.append(data.toByteArray());
    return ret;
}

class QMacPasteboardMimeTypeName : public QMacInternalPasteboardMime {
private:

public:
    QMacPasteboardMimeTypeName() : QMacInternalPasteboardMime(MIME_QT_CONVERTOR|MIME_ALL) {
    }
    ~QMacPasteboardMimeTypeName() {
    }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeTypeName::convertorName()
{
    return QLatin1String("Qt-Mime-Type");
}

QString QMacPasteboardMimeTypeName::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("application/x-qt-mime-type-name"))
        return QLatin1String("com.trolltech.qt.MimeTypeName");
    return QString();
}

QString QMacPasteboardMimeTypeName::mimeFor(QString)
{
    return QString();
}

bool QMacPasteboardMimeTypeName::canConvert(const QString &, QString)
{
    return false;
}

QVariant QMacPasteboardMimeTypeName::convertToMime(const QString &, QList<QByteArray>, QString)
{
    QVariant ret;
    return ret;
}

QList<QByteArray> QMacPasteboardMimeTypeName::convertFromMime(const QString &, QVariant, QString)
{
    QList<QByteArray> ret;
    ret.append(QString(QLatin1String("x-qt-mime-type-name")).toUtf8());
    return ret;
}

class QMacPasteboardMimePlainTextFallback : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimePlainTextFallback() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimePlainTextFallback::convertorName()
{
    return QLatin1String("PlainText (public.text)");
}

QString QMacPasteboardMimePlainTextFallback::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("text/plain"))
        return QLatin1String("public.text");
    return QString();
}

QString QMacPasteboardMimePlainTextFallback::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.text"))
        return QLatin1String("text/plain");
    return QString();
}

bool QMacPasteboardMimePlainTextFallback::canConvert(const QString &mime, QString flav)
{
    return mime == mimeFor(flav);
}

QVariant QMacPasteboardMimePlainTextFallback::convertToMime(const QString &mimetype, QList<QByteArray> data, QString flavor)
{
    if (data.count() > 1)
        qWarning("QMacPasteboardMimePlainTextFallback: Cannot handle multiple member data");

    if (flavor == QLatin1String("public.text")) {
        // Note that public.text is documented by Apple to have an undefined encoding. From
        // testing it seems that utf8 is normally used, at least by Safari on iOS.
        const QByteArray &firstData = data.first();
        return QString(QCFString(CFStringCreateWithBytes(kCFAllocatorDefault,
                                             reinterpret_cast<const UInt8 *>(firstData.constData()),
                                             firstData.size(), kCFStringEncodingUTF8, false)));
    } else {
        qWarning("QMime::convertToMime: unhandled mimetype: %s", qPrintable(mimetype));
    }
    return QVariant();
}

QList<QByteArray> QMacPasteboardMimePlainTextFallback::convertFromMime(const QString &, QVariant data, QString flavor)
{
    QList<QByteArray> ret;
    QString string = data.toString();
    if (flavor == QLatin1String("public.text"))
        ret.append(string.toUtf8());
    return ret;
}

class QMacPasteboardMimeUnicodeText : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeUnicodeText() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeUnicodeText::convertorName()
{
    return QLatin1String("UnicodeText");
}

QString QMacPasteboardMimeUnicodeText::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("text/plain"))
        return QLatin1String("public.utf16-plain-text");
    int i = mime.indexOf(QLatin1String("charset="));
    if (i >= 0) {
        QString cs(mime.mid(i+8).toLower());
        i = cs.indexOf(QLatin1Char(';'));
        if (i>=0)
            cs = cs.left(i);
        if (cs == QLatin1String("system"))
            return QLatin1String("public.utf8-plain-text");
        else if (cs == QLatin1String("iso-10646-ucs-2")
                 || cs == QLatin1String("utf16"))
            return QLatin1String("public.utf16-plain-text");
    }
    return QString();
}

QString QMacPasteboardMimeUnicodeText::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.utf16-plain-text") || flav == QLatin1String("public.utf8-plain-text"))
        return QLatin1String("text/plain");
    return QString();
}

bool QMacPasteboardMimeUnicodeText::canConvert(const QString &mime, QString flav)
{
    return (mime == QLatin1String("text/plain")
            && (flav == QLatin1String("public.utf8-plain-text") || (flav == QLatin1String("public.utf16-plain-text"))));
}

QVariant QMacPasteboardMimeUnicodeText::convertToMime(const QString &mimetype, QList<QByteArray> data, QString flavor)
{
    if (data.count() > 1)
        qWarning("QMacPasteboardMimeUnicodeText: Cannot handle multiple member data");
    const QByteArray &firstData = data.first();
    // I can only handle two types (system and unicode) so deal with them that way
    QVariant ret;
    if (flavor == QLatin1String("public.utf8-plain-text")) {
        ret = QString::fromUtf8(firstData);
#if QT_CONFIG(textcodec)
    } else if (flavor == QLatin1String("public.utf16-plain-text")) {
        ret = QTextCodec::codecForName("UTF-16")->toUnicode(firstData);
#endif
    } else {
        qWarning("QMime::convertToMime: unhandled mimetype: %s", qPrintable(mimetype));
    }
    return ret;
}

QList<QByteArray> QMacPasteboardMimeUnicodeText::convertFromMime(const QString &, QVariant data, QString flavor)
{
    QList<QByteArray> ret;
    QString string = data.toString();
    if (flavor == QLatin1String("public.utf8-plain-text"))
        ret.append(string.toUtf8());
#if QT_CONFIG(textcodec)
    else if (flavor == QLatin1String("public.utf16-plain-text")) {
        QTextCodec::ConverterState state;
#if defined(Q_OS_MACOS)
        // Some applications such as Microsoft Excel, don't deal well with
        // a BOM present, so we follow the traditional approach of Qt on
        // macOS to not generate public.utf16-plain-text with a BOM.
        state.flags = QTextCodec::IgnoreHeader;
#else
        // Whereas iOS applications will fail to paste if we do _not_
        // include a BOM in the public.utf16-plain-text content, most
        // likely due to converting the data using NSUTF16StringEncoding
        // which assumes big-endian byte order if there is no BOM.
        state.flags = QTextCodec::DefaultConversion;
#endif
        ret.append(QTextCodec::codecForName("UTF-16")->fromUnicode(
            string.constData(), string.length(), &state));
    }
#endif
    return ret;
}

class QMacPasteboardMimeHTMLText : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeHTMLText() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeHTMLText::convertorName()
{
    return QLatin1String("HTML");
}

QString QMacPasteboardMimeHTMLText::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("text/html"))
        return QLatin1String("public.html");
    return QString();
}

QString QMacPasteboardMimeHTMLText::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.html"))
        return QLatin1String("text/html");
    return QString();
}

bool QMacPasteboardMimeHTMLText::canConvert(const QString &mime, QString flav)
{
    return flavorFor(mime) == flav;
}

QVariant QMacPasteboardMimeHTMLText::convertToMime(const QString &mimeType, QList<QByteArray> data, QString flavor)
{
    if (!canConvert(mimeType, flavor))
        return QVariant();
    if (data.count() > 1)
        qWarning("QMacPasteboardMimeHTMLText: Cannot handle multiple member data");
    return data.first();
}

QList<QByteArray> QMacPasteboardMimeHTMLText::convertFromMime(const QString &mime, QVariant data, QString flavor)
{
    QList<QByteArray> ret;
    if (!canConvert(mime, flavor))
        return ret;
    ret.append(data.toByteArray());
    return ret;
}

class QMacPasteboardMimeRtfText : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeRtfText() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeRtfText::convertorName()
{
    return QLatin1String("Rtf");
}

QString QMacPasteboardMimeRtfText::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("text/html"))
        return QLatin1String("public.rtf");
    return QString();
}

QString QMacPasteboardMimeRtfText::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.rtf"))
        return QLatin1String("text/html");
    return QString();
}

bool QMacPasteboardMimeRtfText::canConvert(const QString &mime, QString flav)
{
    return mime == mimeFor(flav);
}

QVariant QMacPasteboardMimeRtfText::convertToMime(const QString &mimeType, QList<QByteArray> data, QString flavor)
{
    if (!canConvert(mimeType, flavor))
        return QVariant();
    if (data.count() > 1)
        qWarning("QMacPasteboardMimeHTMLText: Cannot handle multiple member data");

    // Read RTF into to NSAttributedString, then convert the string to HTML
    NSAttributedString *string = [[NSAttributedString alloc] initWithData:data.at(0).toNSData()
            options:@{NSDocumentTypeDocumentAttribute: NSRTFTextDocumentType}
            documentAttributes:nil
            error:nil];

    NSError *error;
    NSRange range = NSMakeRange(0, [string length]);
    NSDictionary *dict = @{NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType};
    NSData *htmlData = [string dataFromRange:range documentAttributes:dict error:&error];
    return QByteArray::fromNSData(htmlData);
}

QList<QByteArray> QMacPasteboardMimeRtfText::convertFromMime(const QString &mime, QVariant data, QString flavor)
{
    QList<QByteArray> ret;
    if (!canConvert(mime, flavor))
        return ret;

    NSAttributedString *string = [[NSAttributedString alloc] initWithData:data.toByteArray().toNSData()
            options:@{NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType}
            documentAttributes:nil
            error:nil];

    NSError *error;
    NSRange range = NSMakeRange(0, [string length]);
    NSDictionary *dict = @{NSDocumentTypeDocumentAttribute: NSRTFTextDocumentType};
    NSData *rtfData = [string dataFromRange:range documentAttributes:dict error:&error];
    ret << QByteArray::fromNSData(rtfData);
    return ret;
}

class QMacPasteboardMimeFileUri : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeFileUri() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
    int count(QMimeData *mimeData);
};

QString QMacPasteboardMimeFileUri::convertorName()
{
    return QLatin1String("FileURL");
}

QString QMacPasteboardMimeFileUri::flavorFor(const QString &mime)
{
    if (mime == QLatin1String("text/uri-list"))
        return QLatin1String("public.file-url");
    return QString();
}

QString QMacPasteboardMimeFileUri::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.file-url"))
        return QLatin1String("text/uri-list");
    return QString();
}

bool QMacPasteboardMimeFileUri::canConvert(const QString &mime, QString flav)
{
    return mime == QLatin1String("text/uri-list") && flav == QLatin1String("public.file-url");
}

QVariant QMacPasteboardMimeFileUri::convertToMime(const QString &mime, QList<QByteArray> data, QString flav)
{
    if (!canConvert(mime, flav))
        return QVariant();
    QList<QVariant> ret;
    for (int i = 0; i < data.size(); ++i) {
        const QByteArray &a = data.at(i);
        NSString *urlString = [[[NSString alloc] initWithBytesNoCopy:(void *)a.data() length:a.size()
                                                 encoding:NSUTF8StringEncoding freeWhenDone:NO] autorelease];
        NSURL *nsurl = [NSURL URLWithString:urlString];
        QUrl url;
        // OS X 10.10 sends file references instead of file paths
        if ([nsurl isFileReferenceURL]) {
            url = QUrl::fromNSURL([nsurl filePathURL]);
        } else {
            url = QUrl::fromNSURL(nsurl);
        }

        if (url.host().toLower() == QLatin1String("localhost"))
            url.setHost(QString());

        url.setPath(url.path().normalized(QString::NormalizationForm_C));
        ret.append(url);
    }
    return QVariant(ret);
}

QList<QByteArray> QMacPasteboardMimeFileUri::convertFromMime(const QString &mime, QVariant data, QString flav)
{
    QList<QByteArray> ret;
    if (!canConvert(mime, flav))
        return ret;
    QList<QVariant> urls = data.toList();
    for (int i = 0; i < urls.size(); ++i) {
        QUrl url = urls.at(i).toUrl();
        if (url.scheme().isEmpty())
            url.setScheme(QLatin1String("file"));
        if (url.scheme() == QLatin1String("file")) {
            if (url.host().isEmpty())
                url.setHost(QLatin1String("localhost"));
            url.setPath(url.path().normalized(QString::NormalizationForm_D));
        }
        ret.append(url.toEncoded());
    }
    return ret;
}

int QMacPasteboardMimeFileUri::count(QMimeData *mimeData)
{
    return mimeData->urls().count();
}

class QMacPasteboardMimeUrl : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeUrl() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeUrl::convertorName()
{
    return QLatin1String("URL");
}

QString QMacPasteboardMimeUrl::flavorFor(const QString &mime)
{
    if (mime.startsWith(QLatin1String("text/uri-list")))
        return QLatin1String("public.url");
    return QString();
}

QString QMacPasteboardMimeUrl::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.url"))
        return QLatin1String("text/uri-list");
    return QString();
}

bool QMacPasteboardMimeUrl::canConvert(const QString &mime, QString flav)
{
    return flav == QLatin1String("public.url")
            && mime == QLatin1String("text/uri-list");
}

QVariant QMacPasteboardMimeUrl::convertToMime(const QString &mime, QList<QByteArray> data, QString flav)
{
    if (!canConvert(mime, flav))
        return QVariant();

    QList<QVariant> ret;
    for (int i=0; i<data.size(); ++i) {
        QUrl url = QUrl::fromEncoded(data.at(i));
        if (url.host().toLower() == QLatin1String("localhost"))
            url.setHost(QString());
        url.setPath(url.path().normalized(QString::NormalizationForm_C));
        ret.append(url);
    }
    return QVariant(ret);
}

QList<QByteArray> QMacPasteboardMimeUrl::convertFromMime(const QString &mime, QVariant data, QString flav)
{
    QList<QByteArray> ret;
    if (!canConvert(mime, flav))
        return ret;

    QList<QVariant> urls = data.toList();
    for (int i=0; i<urls.size(); ++i) {
        QUrl url = urls.at(i).toUrl();
        if (url.scheme().isEmpty())
            url.setScheme(QLatin1String("file"));
        if (url.scheme() == QLatin1String("file")) {
            if (url.host().isEmpty())
                url.setHost(QLatin1String("localhost"));
            url.setPath(url.path().normalized(QString::NormalizationForm_D));
        }
        ret.append(url.toEncoded());
    }
    return ret;
}

class QMacPasteboardMimeVCard : public QMacInternalPasteboardMime
{
public:
    QMacPasteboardMimeVCard() : QMacInternalPasteboardMime(MIME_ALL){ }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeVCard::convertorName()
{
    return QLatin1String("VCard");
}

bool QMacPasteboardMimeVCard::canConvert(const QString &mime, QString flav)
{
    return mimeFor(flav) == mime;
}

QString QMacPasteboardMimeVCard::flavorFor(const QString &mime)
{
    if (mime.startsWith(QLatin1String("text/vcard")))
        return QLatin1String("public.vcard");
    return QString();
}

QString QMacPasteboardMimeVCard::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.vcard"))
        return QLatin1String("text/vcard");
    return QString();
}

QVariant QMacPasteboardMimeVCard::convertToMime(const QString &mime, QList<QByteArray> data, QString)
{
    QByteArray cards;
    if (mime == QLatin1String("text/vcard")) {
        for (int i=0; i<data.size(); ++i)
            cards += data[i];
    }
    return QVariant(cards);
}

QList<QByteArray> QMacPasteboardMimeVCard::convertFromMime(const QString &mime, QVariant data, QString)
{
    QList<QByteArray> ret;
    if (mime == QLatin1String("text/vcard"))
        ret.append(data.toString().toUtf8());
    return ret;
}

extern QImage qt_mac_toQImage(CGImageRef image);
extern CGImageRef qt_mac_toCGImage(const QImage &qImage);

class QMacPasteboardMimeTiff : public QMacInternalPasteboardMime {
public:
    QMacPasteboardMimeTiff() : QMacInternalPasteboardMime(MIME_ALL) { }
    QString convertorName();

    QString flavorFor(const QString &mime);
    QString mimeFor(QString flav);
    bool canConvert(const QString &mime, QString flav);
    QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
    QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

QString QMacPasteboardMimeTiff::convertorName()
{
    return QLatin1String("Tiff");
}

QString QMacPasteboardMimeTiff::flavorFor(const QString &mime)
{
    if (mime.startsWith(QLatin1String("application/x-qt-image")))
        return QLatin1String("public.tiff");
    return QString();
}

QString QMacPasteboardMimeTiff::mimeFor(QString flav)
{
    if (flav == QLatin1String("public.tiff"))
        return QLatin1String("application/x-qt-image");
    return QString();
}

bool QMacPasteboardMimeTiff::canConvert(const QString &mime, QString flav)
{
    return flav == QLatin1String("public.tiff") && mime == QLatin1String("application/x-qt-image");
}

QVariant QMacPasteboardMimeTiff::convertToMime(const QString &mime, QList<QByteArray> data, QString flav)
{
    if (data.count() > 1)
        qWarning("QMacPasteboardMimeTiff: Cannot handle multiple member data");

    if (!canConvert(mime, flav))
        return QVariant();

    QCFType<CFDataRef> tiffData = data.first().toRawCFData();
    QCFType<CGImageSourceRef> imageSource = CGImageSourceCreateWithData(tiffData, 0);

    if (QCFType<CGImageRef> image = CGImageSourceCreateImageAtIndex(imageSource, 0, 0))
        return QVariant(qt_mac_toQImage(image));

    return QVariant();
}

QList<QByteArray> QMacPasteboardMimeTiff::convertFromMime(const QString &mime, QVariant variant, QString flav)
{
    if (!canConvert(mime, flav))
        return QList<QByteArray>();

    QCFType<CFMutableDataRef> data = CFDataCreateMutable(0, 0);
    QCFType<CGImageDestinationRef> imageDestination = CGImageDestinationCreateWithData(data, kUTTypeTIFF, 1, 0);

    if (!imageDestination)
        return QList<QByteArray>();

    QImage img = qvariant_cast<QImage>(variant);
    NSDictionary *props = @{
        static_cast<NSString *>(kCGImagePropertyPixelWidth): @(img.width()),
        static_cast<NSString *>(kCGImagePropertyPixelHeight): @(img.height())
    };

    CGImageDestinationAddImage(imageDestination, qt_mac_toCGImage(img), static_cast<CFDictionaryRef>(props));
    CGImageDestinationFinalize(imageDestination);

    return QList<QByteArray>() << QByteArray::fromCFData(data);
}

/*!
  \internal

  This is an internal function.
*/
void QMacInternalPasteboardMime::initializeMimeTypes()
{
    if (globalMimeList()->isEmpty()) {
        // Create QMacPasteboardMimeAny first to put it at the end of globalMimeList
        // with lowest priority. (the constructor prepends to the list)
        new QMacPasteboardMimeAny;

        //standard types that we wrap
        new QMacPasteboardMimeTiff;
        new QMacPasteboardMimePlainTextFallback;
        new QMacPasteboardMimeUnicodeText;
        new QMacPasteboardMimeRtfText;
        new QMacPasteboardMimeHTMLText;
        new QMacPasteboardMimeFileUri;
        new QMacPasteboardMimeUrl;
        new QMacPasteboardMimeTypeName;
        new QMacPasteboardMimeVCard;
    }
}

/*!
  \internal
*/
void QMacInternalPasteboardMime::destroyMimeTypes()
{
    MimeList *mimes = globalMimeList();
    while (!mimes->isEmpty())
        delete mimes->takeFirst();
}

/*!
  Returns the most-recently created QMacPasteboardMime of type \a t that can convert
  between the \a mime and \a flav formats.  Returns 0 if no such convertor
  exists.
*/
QMacInternalPasteboardMime*
QMacInternalPasteboardMime::convertor(uchar t, const QString &mime, QString flav)
{
    MimeList *mimes = globalMimeList();
    for (MimeList::const_iterator it = mimes->constBegin(); it != mimes->constEnd(); ++it) {
#ifdef DEBUG_MIME_MAPS
        qDebug("QMacPasteboardMime::convertor: seeing if %s (%d) can convert %s to %d[%c%c%c%c] [%d]",
               (*it)->convertorName().toLatin1().constData(),
               (*it)->type & t, mime.toLatin1().constData(),
               flav, (flav >> 24) & 0xFF, (flav >> 16) & 0xFF, (flav >> 8) & 0xFF, (flav) & 0xFF,
               (*it)->canConvert(mime,flav));
        for (int i = 0; i < (*it)->countFlavors(); ++i) {
            int f = (*it)->flavor(i);
            qDebug("  %d) %d[%c%c%c%c] [%s]", i, f,
                   (f >> 24) & 0xFF, (f >> 16) & 0xFF, (f >> 8) & 0xFF, (f) & 0xFF,
                   (*it)->convertorName().toLatin1().constData());
        }
#endif
        if (((*it)->type & t) && (*it)->canConvert(mime, flav))
            return (*it);
    }
    return 0;
}
/*!
  Returns a MIME type of type \a t for \a flav, or 0 if none exists.
*/
QString QMacInternalPasteboardMime::flavorToMime(uchar t, QString flav)
{
    MimeList *mimes = globalMimeList();
    for (MimeList::const_iterator it = mimes->constBegin(); it != mimes->constEnd(); ++it) {
#ifdef DEBUG_MIME_MAPS
        qDebug("QMacMIme::flavorToMime: attempting %s (%d) for flavor %d[%c%c%c%c] [%s]",
               (*it)->convertorName().toLatin1().constData(),
               (*it)->type & t, flav, (flav >> 24) & 0xFF, (flav >> 16) & 0xFF, (flav >> 8) & 0xFF, (flav) & 0xFF,
               (*it)->mimeFor(flav).toLatin1().constData());

#endif
        if ((*it)->type & t) {
            QString mimeType = (*it)->mimeFor(flav);
            if (!mimeType.isNull())
                return mimeType;
        }
    }
    return QString();
}

/*!
  Returns a list of all currently defined QMacPasteboardMime objects of type \a t.
*/
QList<QMacInternalPasteboardMime*> QMacInternalPasteboardMime::all(uchar t)
{
    MimeList ret;
    MimeList *mimes = globalMimeList();
    for (MimeList::const_iterator it = mimes->constBegin(); it != mimes->constEnd(); ++it) {
        if ((*it)->type & t)
            ret.append((*it));
    }
    return ret;
}


/*!
  \fn QString QMacPasteboardMime::convertorName()

  Returns a name for the convertor.

  All subclasses must reimplement this pure virtual function.
*/

/*!
  \fn bool QMacPasteboardMime::canConvert(const QString &mime, QString flav)

  Returns \c true if the convertor can convert (both ways) between
  \a mime and \a flav; otherwise returns \c false.

  All subclasses must reimplement this pure virtual function.
*/

/*!
  \fn QString QMacPasteboardMime::mimeFor(QString flav)

  Returns the MIME UTI used for Mac flavor \a flav, or 0 if this
  convertor does not support \a flav.

  All subclasses must reimplement this pure virtual function.
*/

/*!
  \fn QString QMacPasteboardMime::flavorFor(const QString &mime)

  Returns the Mac UTI used for MIME type \a mime, or 0 if this
  convertor does not support \a mime.

  All subclasses must reimplement this pure virtual function.
*/

/*!
    \fn QVariant QMacPasteboardMime::convertToMime(const QString &mime, QList<QByteArray> data, QString flav)

    Returns \a data converted from Mac UTI \a flav to MIME type \a
    mime.

    Note that Mac flavors must all be self-terminating. The input \a
    data may contain trailing data.

    All subclasses must reimplement this pure virtual function.
*/

/*!
  \fn QList<QByteArray> QMacPasteboardMime::convertFromMime(const QString &mime, QVariant data, QString flav)

  Returns \a data converted from MIME type \a mime
    to Mac UTI \a flav.

  Note that Mac flavors must all be self-terminating.  The return
  value may contain trailing data.

  All subclasses must reimplement this pure virtual function.
*/

QT_END_NAMESPACE
