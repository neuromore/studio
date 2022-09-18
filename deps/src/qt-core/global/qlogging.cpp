/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Olivier Goffart <ogoffart@woboq.com>
** Copyright (C) 2018 Intel Corporation.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qglobal_p.h"
#include "qlogging.h"
#include "qlogging_p.h"
#include "qlist.h"
#include "qbytearray.h"
#include "qstring.h"
#include "qvarlengtharray.h"
#include "qdebug.h"
#include "qmutex.h"
#include "qloggingcategory.h"
#ifndef QT_BOOTSTRAPPED
#include "qelapsedtimer.h"
#include "qdatetime.h"
#include "qcoreapplication.h"
#include "qthread.h"
#include "private/qloggingregistry_p.h"
#include "private/qcoreapplication_p.h"
#include "private/qsimd_p.h"
#include <qtcore_tracepoints_p.h>
#endif
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
#ifdef Q_CC_MSVC
#include <intrin.h>
#endif
#if QT_CONFIG(slog2)
#include <sys/slog2.h>
#endif
#if QT_HAS_INCLUDE(<paths.h>)
#include <paths.h>
#endif

#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif

#ifdef Q_OS_DARWIN
#include <QtCore/private/qcore_mac_p.h>
#endif

#if QT_CONFIG(journald)
# define SD_JOURNAL_SUPPRESS_LOCATION
# include <systemd/sd-journal.h>
# include <syslog.h>
#endif
#if QT_CONFIG(syslog)
# include <syslog.h>
#endif
#ifdef Q_OS_UNIX
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "private/qcore_unix_p.h"
#endif

#ifdef Q_OS_WASM
#include <emscripten/emscripten.h>
#endif

#if QT_CONFIG(regularexpression)
#  ifdef __UCLIBC__
#    if __UCLIBC_HAS_BACKTRACE__
#      define QLOGGING_HAVE_BACKTRACE
#    endif
#  elif (defined(__GLIBC__) && defined(__GLIBCXX__)) || (QT_HAS_INCLUDE(<cxxabi.h>) && QT_HAS_INCLUDE(<execinfo.h>))
#    define QLOGGING_HAVE_BACKTRACE
#  endif
#endif

#ifdef Q_OS_ANDROID
#ifdef QLOGGING_HAVE_BACKTRACE
#undef QLOGGING_HAVE_BACKTRACE
#endif
#endif

#if QT_CONFIG(slog2)
extern char *__progname;
#endif

#ifndef QT_BOOTSTRAPPED
#if defined(Q_OS_LINUX) && (defined(__GLIBC__) || QT_HAS_INCLUDE(<sys/syscall.h>))
#  include <sys/syscall.h>

# if defined(Q_OS_ANDROID) && !defined(SYS_gettid)
#  define SYS_gettid __NR_gettid
# endif

static long qt_gettid()
{
    // no error handling
    // this syscall has existed since Linux 2.4.11 and cannot fail
    return syscall(SYS_gettid);
}
#elif defined(Q_OS_DARWIN)
#  include <pthread.h>
static int qt_gettid()
{
    // no error handling: this call cannot fail
    __uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
}
#elif defined(Q_OS_FREEBSD_KERNEL) && defined(__FreeBSD_version) && __FreeBSD_version >= 900031
#  include <pthread_np.h>
static int qt_gettid()
{
    return pthread_getthreadid_np();
}
#else
static QT_PREPEND_NAMESPACE(qint64) qt_gettid()
{
    QT_USE_NAMESPACE
    return qintptr(QThread::currentThreadId());
}
#endif

#ifdef QLOGGING_HAVE_BACKTRACE
#  include <qregularexpression.h>
#  include <cxxabi.h>
#  include <execinfo.h>
#endif
#endif // !QT_BOOTSTRAPPED

#include <cstdlib>

#include <stdio.h>

QT_BEGIN_NAMESPACE

#if !defined(Q_CC_MSVC)
Q_NORETURN
#endif
static void qt_message_fatal(QtMsgType, const QMessageLogContext &context, const QString &message);
static void qt_message_print(QtMsgType, const QMessageLogContext &context, const QString &message);
static void qt_message_print(const QString &message);

static int checked_var_value(const char *varname)
{
    // qEnvironmentVariableIntValue returns 0 on both parsing failure and on
    // empty, but we need to distinguish between the two for backwards
    // compatibility reasons.
    QByteArray str = qgetenv(varname);
    if (str.isEmpty())
        return 0;

    bool ok;
    int value = str.toInt(&ok, 0);
    return ok ? value : 1;
}

static bool isFatal(QtMsgType msgType)
{
    if (msgType == QtFatalMsg)
        return true;

    if (msgType == QtCriticalMsg) {
        static QAtomicInt fatalCriticals = checked_var_value("QT_FATAL_CRITICALS");

        // it's fatal if the current value is exactly 1,
        // otherwise decrement if it's non-zero
        return fatalCriticals.load() && fatalCriticals.fetchAndAddRelaxed(-1) == 1;
    }

    if (msgType == QtWarningMsg || msgType == QtCriticalMsg) {
        static QAtomicInt fatalWarnings = checked_var_value("QT_FATAL_WARNINGS");

        // it's fatal if the current value is exactly 1,
        // otherwise decrement if it's non-zero
        return fatalWarnings.load() && fatalWarnings.fetchAndAddRelaxed(-1) == 1;
    }

    return false;
}

static bool isDefaultCategory(const char *category)
{
    return !category || strcmp(category, "default") == 0;
}

/*!
    Returns true if writing to \c stderr is supported.

    \internal
    \sa stderrHasConsoleAttached()
*/
static bool systemHasStderr()
{
#if defined(Q_OS_WINRT)
    return false; // WinRT has no stderr
#endif

    return true;
}

/*!
    Returns true if writing to \c stderr will end up in a console/terminal visible to the user.

    This is typically the case if the application was started from the command line.

    If the application is started without a controlling console/terminal, but the parent
    process reads \c stderr and presents it to the user in some other way, the parent process
    may override the detection in this function by setting the QT_ASSUME_STDERR_HAS_CONSOLE
    environment variable to \c 1.

    \note Qt Creator does not implement a pseudo TTY, nor does it launch apps with
    the override environment variable set, but it will read stderr and print it to
    the user, so in effect this function can not be used to conclude that stderr
    output will _not_ be visible to the user, as even if this function returns false,
    the output might still end up visible to the user. For this reason, we don't guard
    the stderr output in the default message handler with stderrHasConsoleAttached().

    \internal
    \sa systemHasStderr()
*/
static bool stderrHasConsoleAttached()
{
    static const bool stderrHasConsoleAttached = []() -> bool {
        if (!systemHasStderr())
            return false;

        if (qEnvironmentVariableIntValue("QT_LOGGING_TO_CONSOLE")) {
            fprintf(stderr, "warning: Environment variable QT_LOGGING_TO_CONSOLE is deprecated, use\n"
                            "QT_ASSUME_STDERR_HAS_CONSOLE and/or QT_FORCE_STDERR_LOGGING instead.\n");
            return true;
        }

        if (qEnvironmentVariableIntValue("QT_ASSUME_STDERR_HAS_CONSOLE"))
            return true;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
        return GetConsoleWindow();
#elif defined(Q_OS_UNIX)
#       ifndef _PATH_TTY
#       define _PATH_TTY "/dev/tty"
#       endif

        // If we can open /dev/tty, we have a controlling TTY
        int ttyDevice = -1;
        if ((ttyDevice = qt_safe_open(_PATH_TTY, O_RDONLY)) >= 0) {
            qt_safe_close(ttyDevice);
            return true;
        } else if (errno == ENOENT || errno == EPERM || errno == ENXIO) {
            // Fall back to isatty for some non-critical errors
            return isatty(STDERR_FILENO);
        } else {
            return false;
        }
#else
        return false; // No way to detect if stderr has a console attached
#endif
    }();

    return stderrHasConsoleAttached;
}


namespace QtPrivate {

/*!
    Returns true if logging \c stderr should be ensured.

    This is normally the case if \c stderr has a console attached, but may be overridden
    by the user by setting the QT_FORCE_STDERR_LOGGING environment variable to \c 1.

    \internal
    \sa stderrHasConsoleAttached()
*/
bool shouldLogToStderr()
{
    static bool forceStderrLogging = qEnvironmentVariableIntValue("QT_FORCE_STDERR_LOGGING");
    return forceStderrLogging || stderrHasConsoleAttached();
}


} // QtPrivate

using namespace QtPrivate;

/*!
    \class QMessageLogContext
    \inmodule QtCore
    \brief The QMessageLogContext class provides additional information about a log message.
    \since 5.0

    The class provides information about the source code location a qDebug(), qInfo(), qWarning(),
    qCritical() or qFatal() message was generated.

    \note By default, this information is recorded only in debug builds. You can overwrite
    this explicitly by defining \c QT_MESSAGELOGCONTEXT or \c{QT_NO_MESSAGELOGCONTEXT}.

    \sa QMessageLogger, QtMessageHandler, qInstallMessageHandler()
*/

/*!
    \class QMessageLogger
    \inmodule QtCore
    \brief The QMessageLogger class generates log messages.
    \since 5.0

    QMessageLogger is used to generate messages for the Qt logging framework. Usually one uses
    it through qDebug(), qInfo(), qWarning(), qCritical, or qFatal() functions,
    which are actually macros: For example qDebug() expands to
    QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug()
    for debug builds, and QMessageLogger(0, 0, 0).debug() for release builds.

    One example of direct use is to forward errors that stem from a scripting language, e.g. QML:

    \snippet code/qlogging/qlogging.cpp 1

    \sa QMessageLogContext, qDebug(), qInfo(), qWarning(), qCritical(), qFatal()
*/

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_DEBUG) && defined(_CRT_ERROR)
static inline void convert_to_wchar_t_elided(wchar_t *d, size_t space, const char *s) Q_DECL_NOEXCEPT
{
    size_t len = qstrlen(s);
    if (len + 1 > space) {
        const size_t skip = len - space + 4; // 4 for "..." + '\0'
        s += skip;
        len -= skip;
        for (int i = 0; i < 3; ++i)
          *d++ = L'.';
    }
    while (len--)
        *d++ = *s++;
    *d++ = 0;
}
#endif

/*!
    \internal
*/
Q_NEVER_INLINE
static QString qt_message(QtMsgType msgType, const QMessageLogContext &context, const char *msg, va_list ap)
{
    QString buf = QString::vasprintf(msg, ap);
    qt_message_print(msgType, context, buf);
    return buf;
}

#undef qDebug
/*!
    Logs a debug message specified with format \a msg. Additional
    parameters, specified by \a msg, may be used.

    \sa qDebug()
*/
void QMessageLogger::debug(const char *msg, ...) const
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtDebugMsg, context, msg, ap);
    va_end(ap);

    if (isFatal(QtDebugMsg))
        qt_message_fatal(QtDebugMsg, context, message);
}


#undef qInfo
/*!
    Logs an informational message specified with format \a msg. Additional
    parameters, specified by \a msg, may be used.

    \sa qInfo()
    \since 5.5
*/
void QMessageLogger::info(const char *msg, ...) const
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtInfoMsg, context, msg, ap);
    va_end(ap);

    if (isFatal(QtInfoMsg))
        qt_message_fatal(QtInfoMsg, context, message);
}

/*!
    \typedef QMessageLogger::CategoryFunction

    This is a typedef for a pointer to a function with the following
    signature:

    \snippet code/qlogging/qlogging.cpp 2

    A function which this signature is generated by Q_DECLARE_LOGGING_CATEGORY,
    Q_LOGGING_CATEGORY.

    \since 5.3
*/

/*!
    Logs a debug message specified with format \a msg for the context \a cat.
    Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCDebug()
*/
void QMessageLogger::debug(const QLoggingCategory &cat, const char *msg, ...) const
{
    if (!cat.isDebugEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtDebugMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtDebugMsg))
        qt_message_fatal(QtDebugMsg, ctxt, message);
}

/*!
    Logs a debug message specified with format \a msg for the context returned
    by \a catFunc. Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCDebug()
*/
void QMessageLogger::debug(QMessageLogger::CategoryFunction catFunc,
                           const char *msg, ...) const
{
    const QLoggingCategory &cat = (*catFunc)();
    if (!cat.isDebugEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtDebugMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtDebugMsg))
        qt_message_fatal(QtDebugMsg, ctxt, message);
}

#ifndef QT_NO_DEBUG_STREAM

/*!
    Logs a debug message using a QDebug stream

    \sa qDebug(), QDebug
*/
QDebug QMessageLogger::debug() const
{
    QDebug dbg = QDebug(QtDebugMsg);
    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    return dbg;
}

/*!
    Logs a debug message into category \a cat using a QDebug stream.

    \since 5.3
    \sa qCDebug(), QDebug
*/
QDebug QMessageLogger::debug(const QLoggingCategory &cat) const
{
    QDebug dbg = QDebug(QtDebugMsg);
    if (!cat.isDebugEnabled())
        dbg.stream->message_output = false;

    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    return dbg;
}

/*!
    Logs a debug message into category returned by \a catFunc using a QDebug stream.

    \since 5.3
    \sa qCDebug(), QDebug
*/
QDebug QMessageLogger::debug(QMessageLogger::CategoryFunction catFunc) const
{
    return debug((*catFunc)());
}

/*!
    \internal

    Returns a QNoDebug object, which is used to ignore debugging output.

    \sa QNoDebug, qDebug()
*/
QNoDebug QMessageLogger::noDebug() const Q_DECL_NOTHROW
{
    return QNoDebug();
}

#endif

/*!
    Logs an informational message specified with format \a msg for the context \a cat.
    Additional parameters, specified by \a msg, may be used.

    \since 5.5
    \sa qCInfo()
*/
void QMessageLogger::info(const QLoggingCategory &cat, const char *msg, ...) const
{
    if (!cat.isInfoEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtInfoMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtInfoMsg))
        qt_message_fatal(QtInfoMsg, ctxt, message);
}

/*!
    Logs an informational message specified with format \a msg for the context returned
    by \a catFunc. Additional parameters, specified by \a msg, may be used.

    \since 5.5
    \sa qCInfo()
*/
void QMessageLogger::info(QMessageLogger::CategoryFunction catFunc,
                           const char *msg, ...) const
{
    const QLoggingCategory &cat = (*catFunc)();
    if (!cat.isInfoEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtInfoMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtInfoMsg))
        qt_message_fatal(QtInfoMsg, ctxt, message);
}

#ifndef QT_NO_DEBUG_STREAM

/*!
    Logs an informational message using a QDebug stream.

    \since 5.5
    \sa qInfo(), QDebug
*/
QDebug QMessageLogger::info() const
{
    QDebug dbg = QDebug(QtInfoMsg);
    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    return dbg;
}

/*!
    Logs an informational message into the category \a cat using a QDebug stream.

    \since 5.5
    \sa qCInfo(), QDebug
*/
QDebug QMessageLogger::info(const QLoggingCategory &cat) const
{
    QDebug dbg = QDebug(QtInfoMsg);
    if (!cat.isInfoEnabled())
        dbg.stream->message_output = false;

    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    return dbg;
}

/*!
    Logs an informational message into category returned by \a catFunc using a QDebug stream.

    \since 5.5
    \sa qCInfo(), QDebug
*/
QDebug QMessageLogger::info(QMessageLogger::CategoryFunction catFunc) const
{
    return info((*catFunc)());
}

#endif

#undef qWarning
/*!
    Logs a warning message specified with format \a msg. Additional
    parameters, specified by \a msg, may be used.

    \sa qWarning()
*/
void QMessageLogger::warning(const char *msg, ...) const
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtWarningMsg, context, msg, ap);
    va_end(ap);

    if (isFatal(QtWarningMsg))
        qt_message_fatal(QtWarningMsg, context, message);
}

/*!
    Logs a warning message specified with format \a msg for the context \a cat.
    Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCWarning()
*/
void QMessageLogger::warning(const QLoggingCategory &cat, const char *msg, ...) const
{
    if (!cat.isWarningEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtWarningMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtWarningMsg))
        qt_message_fatal(QtWarningMsg, ctxt, message);
}

/*!
    Logs a warning message specified with format \a msg for the context returned
    by \a catFunc. Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCWarning()
*/
void QMessageLogger::warning(QMessageLogger::CategoryFunction catFunc,
                             const char *msg, ...) const
{
    const QLoggingCategory &cat = (*catFunc)();
    if (!cat.isWarningEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtWarningMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtWarningMsg))
        qt_message_fatal(QtWarningMsg, ctxt, message);
}

#ifndef QT_NO_DEBUG_STREAM
/*!
    Logs a warning message using a QDebug stream

    \sa qWarning(), QDebug
*/
QDebug QMessageLogger::warning() const
{
    QDebug dbg = QDebug(QtWarningMsg);
    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    return dbg;
}

/*!
    Logs a warning message into category \a cat using a QDebug stream.

    \sa qCWarning(), QDebug
*/
QDebug QMessageLogger::warning(const QLoggingCategory &cat) const
{
    QDebug dbg = QDebug(QtWarningMsg);
    if (!cat.isWarningEnabled())
        dbg.stream->message_output = false;

    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    return dbg;
}

/*!
    Logs a warning message into category returned by \a catFunc using a QDebug stream.

    \since 5.3
    \sa qCWarning(), QDebug
*/
QDebug QMessageLogger::warning(QMessageLogger::CategoryFunction catFunc) const
{
    return warning((*catFunc)());
}

#endif

#undef qCritical

/*!
    Logs a critical message specified with format \a msg. Additional
    parameters, specified by \a msg, may be used.

    \sa qCritical()
*/
void QMessageLogger::critical(const char *msg, ...) const
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtCriticalMsg, context, msg, ap);
    va_end(ap);

    if (isFatal(QtCriticalMsg))
        qt_message_fatal(QtCriticalMsg, context, message);
}

/*!
    Logs a critical message specified with format \a msg for the context \a cat.
    Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCCritical()
*/
void QMessageLogger::critical(const QLoggingCategory &cat, const char *msg, ...) const
{
    if (!cat.isCriticalEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtCriticalMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtCriticalMsg))
        qt_message_fatal(QtCriticalMsg, ctxt, message);
}

/*!
    Logs a critical message specified with format \a msg for the context returned
    by \a catFunc. Additional parameters, specified by \a msg, may be used.

    \since 5.3
    \sa qCCritical()
*/
void QMessageLogger::critical(QMessageLogger::CategoryFunction catFunc,
                              const char *msg, ...) const
{
    const QLoggingCategory &cat = (*catFunc)();
    if (!cat.isCriticalEnabled())
        return;

    QMessageLogContext ctxt;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    va_list ap;
    va_start(ap, msg); // use variable arg list
    const QString message = qt_message(QtCriticalMsg, ctxt, msg, ap);
    va_end(ap);

    if (isFatal(QtCriticalMsg))
        qt_message_fatal(QtCriticalMsg, ctxt, message);
}

#ifndef QT_NO_DEBUG_STREAM
/*!
    Logs a critical message using a QDebug stream

    \sa qCritical(), QDebug
*/
QDebug QMessageLogger::critical() const
{
    QDebug dbg = QDebug(QtCriticalMsg);
    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    return dbg;
}

/*!
    Logs a critical message into category \a cat using a QDebug stream.

    \since 5.3
    \sa qCCritical(), QDebug
*/
QDebug QMessageLogger::critical(const QLoggingCategory &cat) const
{
    QDebug dbg = QDebug(QtCriticalMsg);
    if (!cat.isCriticalEnabled())
        dbg.stream->message_output = false;

    QMessageLogContext &ctxt = dbg.stream->context;
    ctxt.copy(context);
    ctxt.category = cat.categoryName();

    return dbg;
}

/*!
    Logs a critical message into category returned by \a catFunc using a QDebug stream.

    \since 5.3
    \sa qCCritical(), QDebug
*/
QDebug QMessageLogger::critical(QMessageLogger::CategoryFunction catFunc) const
{
    return critical((*catFunc)());
}

#endif

#undef qFatal
/*!
    Logs a fatal message specified with format \a msg. Additional
    parameters, specified by \a msg, may be used.

    \sa qFatal()
*/
void QMessageLogger::fatal(const char *msg, ...) const Q_DECL_NOTHROW
{
    QString message;

    va_list ap;
    va_start(ap, msg); // use variable arg list
    QT_TERMINATE_ON_EXCEPTION(message = qt_message(QtFatalMsg, context, msg, ap));
    va_end(ap);

    qt_message_fatal(QtFatalMsg, context, message);
}

/*!
    \internal
*/
Q_AUTOTEST_EXPORT QByteArray qCleanupFuncinfo(QByteArray info)
{
    // Strip the function info down to the base function name
    // note that this throws away the template definitions,
    // the parameter types (overloads) and any const/volatile qualifiers.

    if (info.isEmpty())
        return info;

    int pos;

    // Skip trailing [with XXX] for templates (gcc), but make
    // sure to not affect Objective-C message names.
    pos = info.size() - 1;
    if (info.endsWith(']') && !(info.startsWith('+') || info.startsWith('-'))) {
        while (--pos) {
            if (info.at(pos) == '[')
                info.truncate(pos);
        }
    }

    // operator names with '(', ')', '<', '>' in it
    static const char operator_call[] = "operator()";
    static const char operator_lessThan[] = "operator<";
    static const char operator_greaterThan[] = "operator>";
    static const char operator_lessThanEqual[] = "operator<=";
    static const char operator_greaterThanEqual[] = "operator>=";

    // canonize operator names
    info.replace("operator ", "operator");

    // remove argument list
    forever {
        int parencount = 0;
        pos = info.lastIndexOf(')');
        if (pos == -1) {
            // Don't know how to parse this function name
            return info;
        }

        // find the beginning of the argument list
        --pos;
        ++parencount;
        while (pos && parencount) {
            if (info.at(pos) == ')')
                ++parencount;
            else if (info.at(pos) == '(')
                --parencount;
            --pos;
        }
        if (parencount != 0)
            return info;

        info.truncate(++pos);

        if (info.at(pos - 1) == ')') {
            if (info.indexOf(operator_call) == pos - (int)strlen(operator_call))
                break;

            // this function returns a pointer to a function
            // and we matched the arguments of the return type's parameter list
            // try again
            info.remove(0, info.indexOf('('));
            info.chop(1);
            continue;
        } else {
            break;
        }
    }

    // find the beginning of the function name
    int parencount = 0;
    int templatecount = 0;
    --pos;

    // make sure special characters in operator names are kept
    if (pos > -1) {
        switch (info.at(pos)) {
        case ')':
            if (info.indexOf(operator_call) == pos - (int)strlen(operator_call) + 1)
                pos -= 2;
            break;
        case '<':
            if (info.indexOf(operator_lessThan) == pos - (int)strlen(operator_lessThan) + 1)
                --pos;
            break;
        case '>':
            if (info.indexOf(operator_greaterThan) == pos - (int)strlen(operator_greaterThan) + 1)
                --pos;
            break;
        case '=': {
            int operatorLength = (int)strlen(operator_lessThanEqual);
            if (info.indexOf(operator_lessThanEqual) == pos - operatorLength + 1)
                pos -= 2;
            else if (info.indexOf(operator_greaterThanEqual) == pos - operatorLength + 1)
                pos -= 2;
            break;
        }
        default:
            break;
        }
    }

    while (pos > -1) {
        if (parencount < 0 || templatecount < 0)
            return info;

        char c = info.at(pos);
        if (c == ')')
            ++parencount;
        else if (c == '(')
            --parencount;
        else if (c == '>')
            ++templatecount;
        else if (c == '<')
            --templatecount;
        else if (c == ' ' && templatecount == 0 && parencount == 0)
            break;

        --pos;
    }
    info = info.mid(pos + 1);

    // remove trailing '*', '&' that are part of the return argument
    while ((info.at(0) == '*')
           || (info.at(0) == '&'))
        info = info.mid(1);

    // we have the full function name now.
    // clean up the templates
    while ((pos = info.lastIndexOf('>')) != -1) {
        if (!info.contains('<'))
            break;

        // find the matching close
        int end = pos;
        templatecount = 1;
        --pos;
        while (pos && templatecount) {
            char c = info.at(pos);
            if (c == '>')
                ++templatecount;
            else if (c == '<')
                --templatecount;
            --pos;
        }
        ++pos;
        info.remove(pos, end - pos + 1);
    }

    return info;
}

// tokens as recognized in QT_MESSAGE_PATTERN
static const char categoryTokenC[] = "%{category}";
static const char typeTokenC[] = "%{type}";
static const char messageTokenC[] = "%{message}";
static const char fileTokenC[] = "%{file}";
static const char lineTokenC[] = "%{line}";
static const char functionTokenC[] = "%{function}";
static const char pidTokenC[] = "%{pid}";
static const char appnameTokenC[] = "%{appname}";
static const char threadidTokenC[] = "%{threadid}";
static const char qthreadptrTokenC[] = "%{qthreadptr}";
static const char timeTokenC[] = "%{time"; //not a typo: this command has arguments
static const char backtraceTokenC[] = "%{backtrace"; //ditto
static const char ifCategoryTokenC[] = "%{if-category}";
static const char ifDebugTokenC[] = "%{if-debug}";
static const char ifInfoTokenC[] = "%{if-info}";
static const char ifWarningTokenC[] = "%{if-warning}";
static const char ifCriticalTokenC[] = "%{if-critical}";
static const char ifFatalTokenC[] = "%{if-fatal}";
static const char endifTokenC[] = "%{endif}";
static const char emptyTokenC[] = "";

static const char defaultPattern[] = "%{if-category}%{category}: %{endif}%{message}";


struct QMessagePattern {
    QMessagePattern();
    ~QMessagePattern();

    void setPattern(const QString &pattern);

    // 0 terminated arrays of literal tokens / literal or placeholder tokens
    const char **literals;
    const char **tokens;
    QList<QString> timeArgs;   // timeFormats in sequence of %{time
#ifndef QT_BOOTSTRAPPED
    QElapsedTimer timer;
#endif
#ifdef QLOGGING_HAVE_BACKTRACE
    struct BacktraceParams {
        QString backtraceSeparator;
        int backtraceDepth;
    };
    QVector<BacktraceParams> backtraceArgs; // backtrace argumens in sequence of %{backtrace
#endif

    bool fromEnvironment;
    static QBasicMutex mutex;
};
#ifdef QLOGGING_HAVE_BACKTRACE
Q_DECLARE_TYPEINFO(QMessagePattern::BacktraceParams, Q_MOVABLE_TYPE);
#endif

QBasicMutex QMessagePattern::mutex;

QMessagePattern::QMessagePattern()
    : literals(0)
    , tokens(0)
    , fromEnvironment(false)
{
#ifndef QT_BOOTSTRAPPED
    timer.start();
#endif
    const QString envPattern = QString::fromLocal8Bit(qgetenv("QT_MESSAGE_PATTERN"));
    if (envPattern.isEmpty()) {
        setPattern(QLatin1String(defaultPattern));
    } else {
        setPattern(envPattern);
        fromEnvironment = true;
    }
}

QMessagePattern::~QMessagePattern()
{
    for (int i = 0; literals[i]; ++i)
        delete [] literals[i];
    delete [] literals;
    literals = 0;
    delete [] tokens;
    tokens = 0;
}

void QMessagePattern::setPattern(const QString &pattern)
{
    if (literals) {
        for (int i = 0; literals[i]; ++i)
            delete [] literals[i];
        delete [] literals;
    }
    delete [] tokens;
    timeArgs.clear();
#ifdef QLOGGING_HAVE_BACKTRACE
    backtraceArgs.clear();
#endif

    // scanner
    QList<QString> lexemes;
    QString lexeme;
    bool inPlaceholder = false;
    for (int i = 0; i < pattern.size(); ++i) {
        const QChar c = pattern.at(i);
        if ((c == QLatin1Char('%'))
                && !inPlaceholder) {
            if ((i + 1 < pattern.size())
                    && pattern.at(i + 1) == QLatin1Char('{')) {
                // beginning of placeholder
                if (!lexeme.isEmpty()) {
                    lexemes.append(lexeme);
                    lexeme.clear();
                }
                inPlaceholder = true;
            }
        }

        lexeme.append(c);

        if ((c == QLatin1Char('}') && inPlaceholder)) {
            // end of placeholder
            lexemes.append(lexeme);
            lexeme.clear();
            inPlaceholder = false;
        }
    }
    if (!lexeme.isEmpty())
        lexemes.append(lexeme);

    // tokenizer
    QVarLengthArray<const char*> literalsVar;
    tokens = new const char*[lexemes.size() + 1];
    tokens[lexemes.size()] = 0;

    bool nestedIfError = false;
    bool inIf = false;
    QString error;

    for (int i = 0; i < lexemes.size(); ++i) {
        const QString lexeme = lexemes.at(i);
        if (lexeme.startsWith(QLatin1String("%{"))
                && lexeme.endsWith(QLatin1Char('}'))) {
            // placeholder
            if (lexeme == QLatin1String(typeTokenC)) {
                tokens[i] = typeTokenC;
            } else if (lexeme == QLatin1String(categoryTokenC))
                tokens[i] = categoryTokenC;
            else if (lexeme == QLatin1String(messageTokenC))
                tokens[i] = messageTokenC;
            else if (lexeme == QLatin1String(fileTokenC))
                tokens[i] = fileTokenC;
            else if (lexeme == QLatin1String(lineTokenC))
                tokens[i] = lineTokenC;
            else if (lexeme == QLatin1String(functionTokenC))
                tokens[i] = functionTokenC;
            else if (lexeme == QLatin1String(pidTokenC))
                tokens[i] = pidTokenC;
            else if (lexeme == QLatin1String(appnameTokenC))
                tokens[i] = appnameTokenC;
            else if (lexeme == QLatin1String(threadidTokenC))
                tokens[i] = threadidTokenC;
            else if (lexeme == QLatin1String(qthreadptrTokenC))
                tokens[i] = qthreadptrTokenC;
            else if (lexeme.startsWith(QLatin1String(timeTokenC))) {
                tokens[i] = timeTokenC;
                int spaceIdx = lexeme.indexOf(QChar::fromLatin1(' '));
                if (spaceIdx > 0)
                    timeArgs.append(lexeme.mid(spaceIdx + 1, lexeme.length() - spaceIdx - 2));
                else
                    timeArgs.append(QString());
            } else if (lexeme.startsWith(QLatin1String(backtraceTokenC))) {
#ifdef QLOGGING_HAVE_BACKTRACE
                tokens[i] = backtraceTokenC;
                QString backtraceSeparator = QStringLiteral("|");
                int backtraceDepth = 5;
                static const QRegularExpression depthRx(QStringLiteral(" depth=(?|\"([^\"]*)\"|([^ }]*))"));
                static const QRegularExpression separatorRx(QStringLiteral(" separator=(?|\"([^\"]*)\"|([^ }]*))"));
                QRegularExpressionMatch m = depthRx.match(lexeme);
                if (m.hasMatch()) {
                    int depth = m.capturedRef(1).toInt();
                    if (depth <= 0)
                        error += QLatin1String("QT_MESSAGE_PATTERN: %{backtrace} depth must be a number greater than 0\n");
                    else
                        backtraceDepth = depth;
                }
                m = separatorRx.match(lexeme);
                if (m.hasMatch())
                    backtraceSeparator = m.captured(1);
                BacktraceParams backtraceParams;
                backtraceParams.backtraceDepth = backtraceDepth;
                backtraceParams.backtraceSeparator = backtraceSeparator;
                backtraceArgs.append(backtraceParams);
#else
                error += QLatin1String("QT_MESSAGE_PATTERN: %{backtrace} is not supported by this Qt build\n");
                tokens[i] = "";
#endif
            }

#define IF_TOKEN(LEVEL) \
            else if (lexeme == QLatin1String(LEVEL)) { \
                if (inIf) \
                    nestedIfError = true; \
                tokens[i] = LEVEL; \
                inIf = true; \
            }
            IF_TOKEN(ifCategoryTokenC)
            IF_TOKEN(ifDebugTokenC)
            IF_TOKEN(ifInfoTokenC)
            IF_TOKEN(ifWarningTokenC)
            IF_TOKEN(ifCriticalTokenC)
            IF_TOKEN(ifFatalTokenC)
#undef IF_TOKEN
            else if (lexeme == QLatin1String(endifTokenC)) {
                tokens[i] = endifTokenC;
                if (!inIf && !nestedIfError)
                    error += QLatin1String("QT_MESSAGE_PATTERN: %{endif} without an %{if-*}\n");
                inIf = false;
            } else {
                tokens[i] = emptyTokenC;
                error += QStringLiteral("QT_MESSAGE_PATTERN: Unknown placeholder %1\n")
                        .arg(lexeme);
            }
        } else {
            char *literal = new char[lexeme.size() + 1];
            strncpy(literal, lexeme.toLatin1().constData(), lexeme.size());
            literal[lexeme.size()] = '\0';
            literalsVar.append(literal);
            tokens[i] = literal;
        }
    }
    if (nestedIfError)
        error += QLatin1String("QT_MESSAGE_PATTERN: %{if-*} cannot be nested\n");
    else if (inIf)
        error += QLatin1String("QT_MESSAGE_PATTERN: missing %{endif}\n");

    if (!error.isEmpty())
        qt_message_print(error);

    literals = new const char*[literalsVar.size() + 1];
    literals[literalsVar.size()] = 0;
    memcpy(literals, literalsVar.constData(), literalsVar.size() * sizeof(const char*));
}

#if defined(QLOGGING_HAVE_BACKTRACE) && !defined(QT_BOOTSTRAPPED)
// make sure the function has "Message" in the name so the function is removed

#if ((defined(Q_CC_GNU) && defined(QT_COMPILER_SUPPORTS_SIMD_ALWAYS)) || QT_HAS_ATTRIBUTE(optimize)) \
    && !defined(Q_CC_INTEL) && !defined(Q_CC_CLANG)
// force skipping the frame pointer, to save the backtrace() function some work
__attribute__((optimize("omit-frame-pointer")))
#endif
static QStringList backtraceFramesForLogMessage(int frameCount)
{
    QStringList result;
    if (frameCount == 0)
        return result;

    // The results of backtrace_symbols looks like this:
    //    /lib/libc.so.6(__libc_start_main+0xf3) [0x4a937413]
    // The offset and function name are optional.
    // This regexp tries to extract the library name (without the path) and the function name.
    // This code is protected by QMessagePattern::mutex so it is thread safe on all compilers
    static const QRegularExpression rx(QStringLiteral("^(?:[^(]*/)?([^(/]+)\\(([^+]*)(?:[\\+[a-f0-9x]*)?\\) \\[[a-f0-9x]*\\]$"));

    QVarLengthArray<void*, 32> buffer(8 + frameCount);
    int n = backtrace(buffer.data(), buffer.size());
    if (n > 0) {
        int numberPrinted = 0;
        for (int i = 0; i < n && numberPrinted < frameCount; ++i) {
            QScopedPointer<char*, QScopedPointerPodDeleter> strings(backtrace_symbols(buffer.data() + i, 1));
            QString trace = QString::fromLatin1(strings.data()[0]);
            QRegularExpressionMatch m = rx.match(trace);
            if (m.hasMatch()) {
                QString library = m.captured(1);
                QString function = m.captured(2);

                // skip the trace from QtCore that are because of the qDebug itself
                if (!numberPrinted && library.contains(QLatin1String("Qt5Core"))
                        && (function.isEmpty() || function.contains(QLatin1String("Message"), Qt::CaseInsensitive)
                            || function.contains(QLatin1String("QDebug")))) {
                    continue;
                }

                if (function.startsWith(QLatin1String("_Z"))) {
                    QScopedPointer<char, QScopedPointerPodDeleter> demangled(
                                abi::__cxa_demangle(function.toUtf8(), 0, 0, 0));
                    if (demangled)
                        function = QString::fromUtf8(qCleanupFuncinfo(demangled.data()));
                }

                if (function.isEmpty()) {
                    result.append(QLatin1Char('?') + library + QLatin1Char('?'));
                } else {
                    result.append(function);
                }
            } else {
                if (numberPrinted == 0) {
                    // innermost, unknown frames are usually the logging framework itself
                    continue;
                }
                result.append(QStringLiteral("???"));
            }
            numberPrinted++;
        }
    }
    return result;
}

static QString formatBacktraceForLogMessage(const QMessagePattern::BacktraceParams backtraceParams,
                                            const char *function)
{
    QString backtraceSeparator = backtraceParams.backtraceSeparator;
    int backtraceDepth = backtraceParams.backtraceDepth;

    QStringList frames = backtraceFramesForLogMessage(backtraceDepth);
    if (frames.isEmpty())
        return QString();

    // if the first frame is unknown, replace it with the context function
    if (function && frames.at(0).startsWith(QLatin1Char('?')))
        frames[0] = QString::fromUtf8(qCleanupFuncinfo(function));

    return frames.join(backtraceSeparator);
}
#endif // QLOGGING_HAVE_BACKTRACE && !QT_BOOTSTRAPPED

Q_GLOBAL_STATIC(QMessagePattern, qMessagePattern)

/*!
    \relates <QtGlobal>
    \since 5.4

    Generates a formatted string out of the \a type, \a context, \a str arguments.

    qFormatLogMessage returns a QString that is formatted according to the current message pattern.
    It can be used by custom message handlers to format output similar to Qt's default message
    handler.

    The function is thread-safe.

    \sa qInstallMessageHandler(), qSetMessagePattern()
 */
QString qFormatLogMessage(QtMsgType type, const QMessageLogContext &context, const QString &str)
{
    QString message;

    QMutexLocker lock(&QMessagePattern::mutex);

    QMessagePattern *pattern = qMessagePattern();
    if (!pattern) {
        // after destruction of static QMessagePattern instance
        message.append(str);
        return message;
    }

    bool skip = false;

#ifndef QT_BOOTSTRAPPED
    int timeArgsIdx = 0;
#ifdef QLOGGING_HAVE_BACKTRACE
    int backtraceArgsIdx = 0;
#endif
#endif

    // we do not convert file, function, line literals to local encoding due to overhead
    for (int i = 0; pattern->tokens[i] != 0; ++i) {
        const char *token = pattern->tokens[i];
        if (token == endifTokenC) {
            skip = false;
        } else if (skip) {
            // we skip adding messages, but we have to iterate over
            // timeArgsIdx and backtraceArgsIdx anyway
#ifndef QT_BOOTSTRAPPED
            if (token == timeTokenC)
                timeArgsIdx++;
#ifdef QLOGGING_HAVE_BACKTRACE
            else if (token == backtraceTokenC)
                backtraceArgsIdx++;
#endif
#endif
        } else if (token == messageTokenC) {
            message.append(str);
        } else if (token == categoryTokenC) {
            message.append(QLatin1String(context.category));
        } else if (token == typeTokenC) {
            switch (type) {
            case QtDebugMsg:   message.append(QLatin1String("debug")); break;
            case QtInfoMsg:    message.append(QLatin1String("info")); break;
            case QtWarningMsg: message.append(QLatin1String("warning")); break;
            case QtCriticalMsg:message.append(QLatin1String("critical")); break;
            case QtFatalMsg:   message.append(QLatin1String("fatal")); break;
            }
        } else if (token == fileTokenC) {
            if (context.file)
                message.append(QLatin1String(context.file));
            else
                message.append(QLatin1String("unknown"));
        } else if (token == lineTokenC) {
            message.append(QString::number(context.line));
        } else if (token == functionTokenC) {
            if (context.function)
                message.append(QString::fromLatin1(qCleanupFuncinfo(context.function)));
            else
                message.append(QLatin1String("unknown"));
#ifndef QT_BOOTSTRAPPED
        } else if (token == pidTokenC) {
            message.append(QString::number(QCoreApplication::applicationPid()));
        } else if (token == appnameTokenC) {
            message.append(QCoreApplication::applicationName());
        } else if (token == threadidTokenC) {
            // print the TID as decimal
            message.append(QString::number(qt_gettid()));
        } else if (token == qthreadptrTokenC) {
            message.append(QLatin1String("0x"));
            message.append(QString::number(qlonglong(QThread::currentThread()->currentThread()), 16));
#ifdef QLOGGING_HAVE_BACKTRACE
        } else if (token == backtraceTokenC) {
            QMessagePattern::BacktraceParams backtraceParams = pattern->backtraceArgs.at(backtraceArgsIdx);
            backtraceArgsIdx++;
            message.append(formatBacktraceForLogMessage(backtraceParams, context.function));
#endif
        } else if (token == timeTokenC) {
            QString timeFormat = pattern->timeArgs.at(timeArgsIdx);
            timeArgsIdx++;
            if (timeFormat == QLatin1String("process")) {
                    quint64 ms = pattern->timer.elapsed();
                    message.append(QString::asprintf("%6d.%03d", uint(ms / 1000), uint(ms % 1000)));
            } else if (timeFormat ==  QLatin1String("boot")) {
                // just print the milliseconds since the elapsed timer reference
                // like the Linux kernel does
                QElapsedTimer now;
                now.start();
                uint ms = now.msecsSinceReference();
                message.append(QString::asprintf("%6d.%03d", uint(ms / 1000), uint(ms % 1000)));
#if QT_CONFIG(datestring)
            } else if (timeFormat.isEmpty()) {
                    message.append(QDateTime::currentDateTime().toString(Qt::ISODate));
            } else {
                message.append(QDateTime::currentDateTime().toString(timeFormat));
#endif // QT_CONFIG(datestring)
            }
#endif // !QT_BOOTSTRAPPED
        } else if (token == ifCategoryTokenC) {
            if (isDefaultCategory(context.category))
                skip = true;
#define HANDLE_IF_TOKEN(LEVEL)  \
        } else if (token == if##LEVEL##TokenC) { \
            skip = type != Qt##LEVEL##Msg;
        HANDLE_IF_TOKEN(Debug)
        HANDLE_IF_TOKEN(Info)
        HANDLE_IF_TOKEN(Warning)
        HANDLE_IF_TOKEN(Critical)
        HANDLE_IF_TOKEN(Fatal)
#undef HANDLE_IF_TOKEN
        } else {
            message.append(QLatin1String(token));
        }
    }
    return message;
}

#if !QT_DEPRECATED_SINCE(5, 0)
// make sure they're defined to be exported
typedef void (*QtMsgHandler)(QtMsgType, const char *);
Q_CORE_EXPORT QtMsgHandler qInstallMsgHandler(QtMsgHandler);
#endif

static void qDefaultMsgHandler(QtMsgType type, const char *buf);
static void qDefaultMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &buf);

// pointer to QtMsgHandler debug handler (without context)
static QBasicAtomicPointer<void (QtMsgType, const char*)> msgHandler = Q_BASIC_ATOMIC_INITIALIZER(qDefaultMsgHandler);
// pointer to QtMessageHandler debug handler (with context)
static QBasicAtomicPointer<void (QtMsgType, const QMessageLogContext &, const QString &)> messageHandler = Q_BASIC_ATOMIC_INITIALIZER(qDefaultMessageHandler);

// ------------------------ Alternate logging sinks -------------------------

#if defined(QT_BOOTSTRAPPED)
    // Boostrapped tools always print to stderr, so no need for alternate sinks
#else

#if QT_CONFIG(slog2)
#ifndef QT_LOG_CODE
#define QT_LOG_CODE 9000
#endif

static bool slog2_default_handler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);
    formattedMessage.append(QLatin1Char('\n'));
    if (slog2_set_default_buffer((slog2_buffer_t)-1) == 0) {
        slog2_buffer_set_config_t buffer_config;
        slog2_buffer_t buffer_handle;

        buffer_config.buffer_set_name = __progname;
        buffer_config.num_buffers = 1;
        buffer_config.verbosity_level = SLOG2_DEBUG1;
        buffer_config.buffer_config[0].buffer_name = "default";
        buffer_config.buffer_config[0].num_pages = 8;

        if (slog2_register(&buffer_config, &buffer_handle, 0) == -1) {
            fprintf(stderr, "Error registering slogger2 buffer!\n");
            fprintf(stderr, "%s", formattedMessage.toLocal8Bit().constData());
            fflush(stderr);
            return false;
        }

        // Set as the default buffer
        slog2_set_default_buffer(buffer_handle);
    }
    int severity;
    //Determines the severity level
    switch (type) {
    case QtDebugMsg:
        severity = SLOG2_DEBUG1;
        break;
    case QtInfoMsg:
        severity = SLOG2_INFO;
        break;
    case QtWarningMsg:
        severity = SLOG2_NOTICE;
        break;
    case QtCriticalMsg:
        severity = SLOG2_WARNING;
        break;
    case QtFatalMsg:
        severity = SLOG2_ERROR;
        break;
    }
    //writes to the slog2 buffer
    slog2c(NULL, QT_LOG_CODE, severity, formattedMessage.toLocal8Bit().constData());

    return true; // Prevent further output to stderr
}
#endif // slog2

#if QT_CONFIG(journald)
static bool systemd_default_message_handler(QtMsgType type,
                                            const QMessageLogContext &context,
                                            const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);

    int priority = LOG_INFO; // Informational
    switch (type) {
    case QtDebugMsg:
        priority = LOG_DEBUG; // Debug-level messages
        break;
    case QtInfoMsg:
        priority = LOG_INFO; // Informational conditions
        break;
    case QtWarningMsg:
        priority = LOG_WARNING; // Warning conditions
        break;
    case QtCriticalMsg:
        priority = LOG_CRIT; // Critical conditions
        break;
    case QtFatalMsg:
        priority = LOG_ALERT; // Action must be taken immediately
        break;
    }

    sd_journal_send("MESSAGE=%s",     formattedMessage.toUtf8().constData(),
                    "PRIORITY=%i",    priority,
                    "CODE_FUNC=%s",   context.function ? context.function : "unknown",
                    "CODE_LINE=%d",   context.line,
                    "CODE_FILE=%s",   context.file ? context.file : "unknown",
                    "QT_CATEGORY=%s", context.category ? context.category : "unknown",
                    NULL);

    return true; // Prevent further output to stderr
}
#endif

#if QT_CONFIG(syslog)
static bool syslog_default_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);

    int priority = LOG_INFO; // Informational
    switch (type) {
    case QtDebugMsg:
        priority = LOG_DEBUG; // Debug-level messages
        break;
    case QtInfoMsg:
        priority = LOG_INFO; // Informational conditions
        break;
    case QtWarningMsg:
        priority = LOG_WARNING; // Warning conditions
        break;
    case QtCriticalMsg:
        priority = LOG_CRIT; // Critical conditions
        break;
    case QtFatalMsg:
        priority = LOG_ALERT; // Action must be taken immediately
        break;
    }

    syslog(priority, "%s", formattedMessage.toUtf8().constData());

    return true; // Prevent further output to stderr
}
#endif

#if defined(Q_OS_ANDROID) && !defined(Q_OS_ANDROID_EMBEDDED)
static bool android_default_message_handler(QtMsgType type,
                                  const QMessageLogContext &context,
                                  const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);

    android_LogPriority priority = ANDROID_LOG_DEBUG;
    switch (type) {
    case QtDebugMsg: priority = ANDROID_LOG_DEBUG; break;
    case QtInfoMsg: priority = ANDROID_LOG_INFO; break;
    case QtWarningMsg: priority = ANDROID_LOG_WARN; break;
    case QtCriticalMsg: priority = ANDROID_LOG_ERROR; break;
    case QtFatalMsg: priority = ANDROID_LOG_FATAL; break;
    };

    __android_log_print(priority, qPrintable(QCoreApplication::applicationName()), "%s\n", qPrintable(formattedMessage));

    return true; // Prevent further output to stderr
}
#endif //Q_OS_ANDROID

#ifdef Q_OS_WIN
static bool win_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);
    formattedMessage.append(QLatin1Char('\n'));
    OutputDebugString(reinterpret_cast<const wchar_t *>(formattedMessage.utf16()));

    return true; // Prevent further output to stderr
}
#endif

#ifdef Q_OS_WASM
static bool wasm_default_message_handler(QtMsgType type,
                                  const QMessageLogContext &context,
                                  const QString &message)
{
    if (shouldLogToStderr())
        return false; // Leave logging up to stderr handler

    QString formattedMessage = qFormatLogMessage(type, context, message);
    int emOutputFlags = (EM_LOG_CONSOLE | EM_LOG_DEMANGLE);
    QByteArray localMsg = message.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        break;
    case QtInfoMsg:
        break;
    case QtWarningMsg:
        emOutputFlags |= EM_LOG_WARN;
        break;
    case QtCriticalMsg:
        emOutputFlags |= EM_LOG_ERROR;
        break;
    case QtFatalMsg:
        emOutputFlags |= EM_LOG_ERROR;
    }
    emscripten_log(emOutputFlags, "%s\n", qPrintable(formattedMessage));

    return true; // Prevent further output to stderr
}
#endif

#endif // Bootstrap check

// --------------------------------------------------------------------------

static void stderr_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString formattedMessage = qFormatLogMessage(type, context, message);

    // print nothing if message pattern didn't apply / was empty.
    // (still print empty lines, e.g. because message itself was empty)
    if (formattedMessage.isNull())
        return;

    fprintf(stderr, "%s\n", formattedMessage.toLocal8Bit().constData());
    fflush(stderr);
}

/*!
    \internal
*/
static void qDefaultMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                   const QString &message)
{
    bool handledStderr = false;

    // A message sink logs the message to a structured or unstructured destination,
    // optionally formatting the message if the latter, and returns true if the sink
    // handled stderr output as well, which will shortcut our default stderr output.
    // In the future, if we allow multiple/dynamic sinks, this will be iterating
    // a list of sinks.

#if !defined(QT_BOOTSTRAPPED)
# if defined(Q_OS_WIN)
    handledStderr |= win_message_handler(type, context, message);
# elif QT_CONFIG(slog2)
    handledStderr |= slog2_default_handler(type, context, message);
# elif QT_CONFIG(journald)
    handledStderr |= systemd_default_message_handler(type, context, message);
# elif QT_CONFIG(syslog)
    handledStderr |= syslog_default_message_handler(type, context, message);
# elif defined(Q_OS_ANDROID) && !defined(Q_OS_ANDROID_EMBEDDED)
    handledStderr |= android_default_message_handler(type, context, message);
# elif defined(QT_USE_APPLE_UNIFIED_LOGGING)
    handledStderr |= AppleUnifiedLogger::messageHandler(type, context, message);
# elif defined Q_OS_WASM
    handledStderr |= wasm_default_message_handler(type, context, message);
# endif
#endif

    if (!handledStderr)
        stderr_message_handler(type, context, message);
}

/*!
    \internal
*/
static void qDefaultMsgHandler(QtMsgType type, const char *buf)
{
    QMessageLogContext emptyContext;
    qDefaultMessageHandler(type, emptyContext, QString::fromLocal8Bit(buf));
}

#if defined(Q_COMPILER_THREAD_LOCAL)

static thread_local bool msgHandlerGrabbed = false;

static bool grabMessageHandler()
{
    if (msgHandlerGrabbed)
        return false;

    msgHandlerGrabbed = true;
    return true;
}

static void ungrabMessageHandler()
{
    msgHandlerGrabbed = false;
}

#else
static bool grabMessageHandler() { return true; }
static void ungrabMessageHandler() { }
#endif // (Q_COMPILER_THREAD_LOCAL)

static void qt_message_print(QtMsgType msgType, const QMessageLogContext &context, const QString &message)
{
#ifndef QT_BOOTSTRAPPED
    Q_TRACE(qt_message_print, msgType, context.category, context.function, context.file, context.line, message);

    // qDebug, qWarning, ... macros do not check whether category is enabled
    if (isDefaultCategory(context.category)) {
        if (QLoggingCategory *defaultCategory = QLoggingCategory::defaultCategory()) {
            if (!defaultCategory->isEnabled(msgType))
                return;
        }
    }
#endif

    // prevent recursion in case the message handler generates messages
    // itself, e.g. by using Qt API
    if (grabMessageHandler()) {
        // prefer new message handler over the old one
        if (msgHandler.load() == qDefaultMsgHandler
                || messageHandler.load() != qDefaultMessageHandler) {
            (*messageHandler.load())(msgType, context, message);
        } else {
            (*msgHandler.load())(msgType, message.toLocal8Bit().constData());
        }
        ungrabMessageHandler();
    } else {
        fprintf(stderr, "%s\n", message.toLocal8Bit().constData());
    }
}

static void qt_message_print(const QString &message)
{
#if defined(Q_OS_WINRT)
    OutputDebugString(reinterpret_cast<const wchar_t*>(message.utf16()));
    return;
#elif defined(Q_OS_WIN) && !defined(QT_BOOTSTRAPPED)
    if (!shouldLogToStderr()) {
        OutputDebugString(reinterpret_cast<const wchar_t*>(message.utf16()));
        return;
    }
#endif
    fprintf(stderr, "%s", message.toLocal8Bit().constData());
    fflush(stderr);
}

static void qt_message_fatal(QtMsgType, const QMessageLogContext &context, const QString &message)
{
#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_DEBUG) && defined(_CRT_ERROR)
    wchar_t contextFileL[256];
    // we probably should let the compiler do this for us, by declaring QMessageLogContext::file to
    // be const wchar_t * in the first place, but the #ifdefery above is very complex  and we
    // wouldn't be able to change it later on...
    convert_to_wchar_t_elided(contextFileL, sizeof contextFileL / sizeof *contextFileL,
                              context.file);
    // get the current report mode
    int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
    _CrtSetReportMode(_CRT_ERROR, reportMode);

    int ret = _CrtDbgReportW(_CRT_ERROR, contextFileL, context.line, _CRT_WIDE(QT_VERSION_STR),
                             reinterpret_cast<const wchar_t *>(message.utf16()));
    if ((ret == 0) && (reportMode & _CRTDBG_MODE_WNDW))
        return; // ignore
    else if (ret == 1)
        _CrtDbgBreak();
#else
    Q_UNUSED(context);
    Q_UNUSED(message);
#endif

#ifdef Q_OS_WIN
    // std::abort() in the MSVC runtime will call _exit(3) if the abort
    // behavior is _WRITE_ABORT_MSG - see also _set_abort_behavior(). This is
    // the default for a debug-mode build of the runtime. Worse, MinGW's
    // std::abort() implementation (in msvcrt.dll) is basically a call to
    // _exit(3) too. Unfortunately, _exit() and _Exit() *do* run the static
    // destructors of objects in DLLs, a violation of the C++ standard (see
    // [support.start.term]). So we bypass std::abort() and directly
    // terminate the application.

#  if defined(Q_CC_MSVC) && !defined(Q_CC_INTEL)
    if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
#  else
    RaiseFailFastException(nullptr, nullptr, 0);
#  endif

    // Fallback
    TerminateProcess(GetCurrentProcess(), STATUS_FATAL_APP_EXIT);

    // Tell the compiler the application has stopped.
    Q_UNREACHABLE_IMPL();
#else // !Q_OS_WIN
    std::abort();
#endif
}


/*!
    \internal
*/
void qt_message_output(QtMsgType msgType, const QMessageLogContext &context, const QString &message)
{
    qt_message_print(msgType, context, message);
    if (isFatal(msgType))
        qt_message_fatal(msgType, context, message);
}

void qErrnoWarning(const char *msg, ...)
{
    // qt_error_string() will allocate anyway, so we don't have
    // to be careful here (like we do in plain qWarning())
    va_list ap;
    va_start(ap, msg);
    QString buf = QString::vasprintf(msg, ap);
    va_end(ap);

    buf += QLatin1String(" (") + qt_error_string(-1) + QLatin1Char(')');
    QMessageLogContext context;
    qt_message_output(QtCriticalMsg, context, buf);
}

void qErrnoWarning(int code, const char *msg, ...)
{
    // qt_error_string() will allocate anyway, so we don't have
    // to be careful here (like we do in plain qWarning())
    va_list ap;
    va_start(ap, msg);
    QString buf = QString::vasprintf(msg, ap);
    va_end(ap);

    buf += QLatin1String(" (") + qt_error_string(code) + QLatin1Char(')');
    QMessageLogContext context;
    qt_message_output(QtCriticalMsg, context, buf);
}

/*!
    \typedef QtMsgHandler
    \relates <QtGlobal>
    \deprecated

    This is a typedef for a pointer to a function with the following
    signature:

    \snippet code/src_corelib_global_qglobal.cpp 7

    This typedef is deprecated, you should use QtMessageHandler instead.
    \sa QtMsgType, QtMessageHandler, qInstallMsgHandler(), qInstallMessageHandler()
*/

/*!
    \typedef QtMessageHandler
    \relates <QtGlobal>
    \since 5.0

    This is a typedef for a pointer to a function with the following
    signature:

    \snippet code/src_corelib_global_qglobal.cpp 49

    \sa QtMsgType, qInstallMessageHandler()
*/

/*!
    \fn QtMessageHandler qInstallMessageHandler(QtMessageHandler handler)
    \relates <QtGlobal>
    \since 5.0

    Installs a Qt message \a handler which has been defined
    previously. Returns a pointer to the previous message handler.

    The message handler is a function that prints out debug messages,
    warnings, critical and fatal error messages. The Qt library (debug
    mode) contains hundreds of warning messages that are printed
    when internal errors (usually invalid function arguments)
    occur. Qt built in release mode also contains such warnings unless
    QT_NO_WARNING_OUTPUT and/or QT_NO_DEBUG_OUTPUT have been set during
    compilation. If you implement your own message handler, you get total
    control of these messages.

    The default message handler prints the message to the standard
    output under X11 or to the debugger under Windows. If it is a
    fatal message, the application aborts immediately.

    Only one message handler can be defined, since this is usually
    done on an application-wide basis to control debug output.

    To restore the message handler, call \c qInstallMessageHandler(0).

    Example:

    \snippet code/src_corelib_global_qglobal.cpp 23

    \sa QtMessageHandler, QtMsgType, qDebug(), qInfo(), qWarning(), qCritical(), qFatal(),
    {Debugging Techniques}
*/

/*!
    \fn QtMsgHandler qInstallMsgHandler(QtMsgHandler handler)
    \relates <QtGlobal>
    \deprecated

    Installs a Qt message \a handler which has been defined
    previously. This method is deprecated, use qInstallMessageHandler
    instead.
    \sa QtMsgHandler, qInstallMessageHandler()
*/
/*!
    \fn void qSetMessagePattern(const QString &pattern)
    \relates <QtGlobal>
    \since 5.0

    \brief Changes the output of the default message handler.

    Allows to tweak the output of qDebug(), qInfo(), qWarning(), qCritical(),
    and qFatal(). The category logging output of qCDebug(), qCInfo(),
    qCWarning(), and qCCritical() is formatted, too.

    Following placeholders are supported:

    \table
    \header \li Placeholder \li Description
    \row \li \c %{appname} \li QCoreApplication::applicationName()
    \row \li \c %{category} \li Logging category
    \row \li \c %{file} \li Path to source file
    \row \li \c %{function} \li Function
    \row \li \c %{line} \li Line in source file
    \row \li \c %{message} \li The actual message
    \row \li \c %{pid} \li QCoreApplication::applicationPid()
    \row \li \c %{threadid} \li The system-wide ID of current thread (if it can be obtained)
    \row \li \c %{qthreadptr} \li A pointer to the current QThread (result of QThread::currentThread())
    \row \li \c %{type} \li "debug", "warning", "critical" or "fatal"
    \row \li \c %{time process} \li time of the message, in seconds since the process started (the token "process" is literal)
    \row \li \c %{time boot} \li the time of the message, in seconds since the system boot if that
        can be determined (the token "boot" is literal). If the time since boot could not be obtained,
        the output is indeterminate (see QElapsedTimer::msecsSinceReference()).
    \row \li \c %{time [format]} \li system time when the message occurred, formatted by
        passing the \c format to \l QDateTime::toString(). If the format is
        not specified, the format of Qt::ISODate is used.
    \row \li \c{%{backtrace [depth=N] [separator="..."]}} \li A backtrace with the number of frames
        specified by the optional \c depth parameter (defaults to 5), and separated by the optional
        \c separator parameter (defaults to "|").
        This expansion is available only on some platforms (currently only platfoms using glibc).
        Names are only known for exported functions. If you want to see the name of every function
        in your application, use \c{QMAKE_LFLAGS += -rdynamic}.
        When reading backtraces, take into account that frames might be missing due to inlining or
        tail call optimization.
    \endtable

    You can also use conditionals on the type of the message using \c %{if-debug}, \c %{if-info}
    \c %{if-warning}, \c %{if-critical} or \c %{if-fatal} followed by an \c %{endif}.
    What is inside the \c %{if-*} and \c %{endif} will only be printed if the type matches.

    Finally, text inside \c %{if-category} ... \c %{endif} is only printed if the category
    is not the default one.

    Example:
    \snippet code/src_corelib_global_qlogging.cpp 0

    The default \a pattern is "%{if-category}%{category}: %{endif}%{message}".

    The \a pattern can also be changed at runtime by setting the QT_MESSAGE_PATTERN
    environment variable; if both \l qSetMessagePattern() is called and QT_MESSAGE_PATTERN is
    set, the environment variable takes precedence.

    \note The message pattern only applies to unstructured logging, such as the default
    \c stderr output. Structured logging such as systemd will record the message as is,
    along with as much structured information as can be captured.

    Custom message handlers can use qFormatLogMessage() to take \a pattern into account.

    \sa qInstallMessageHandler(), {Debugging Techniques}, {QLoggingCategory}
 */

QtMessageHandler qInstallMessageHandler(QtMessageHandler h)
{
    if (!h)
        h = qDefaultMessageHandler;
    //set 'h' and return old message handler
    return messageHandler.fetchAndStoreRelaxed(h);
}

QtMsgHandler qInstallMsgHandler(QtMsgHandler h)
{
    if (!h)
        h = qDefaultMsgHandler;
    //set 'h' and return old message handler
    return msgHandler.fetchAndStoreRelaxed(h);
}

void qSetMessagePattern(const QString &pattern)
{
    QMutexLocker lock(&QMessagePattern::mutex);

    if (!qMessagePattern()->fromEnvironment)
        qMessagePattern()->setPattern(pattern);
}


/*!
    Copies context information from \a logContext into this QMessageLogContext
    \internal
*/
void QMessageLogContext::copy(const QMessageLogContext &logContext)
{
    this->category = logContext.category;
    this->file = logContext.file;
    this->line = logContext.line;
    this->function = logContext.function;
}

/*!
    \fn QMessageLogger::QMessageLogger()

    Constructs a default QMessageLogger. See the other constructors to specify
    context information.
*/

/*!
    \fn QMessageLogger::QMessageLogger(const char *file, int line, const char *function)

    Constructs a QMessageLogger to record log messages for \a file at \a line
    in \a function. The is equivalent to QMessageLogger(file, line, function, "default")
*/
/*!
    \fn QMessageLogger::QMessageLogger(const char *file, int line, const char *function, const char *category)

    Constructs a QMessageLogger to record \a category messages for \a file at \a line
    in \a function.
*/

/*!
    \fn void QMessageLogger::noDebug(const char *, ...) const
    \internal

    Ignores logging output

    \sa QNoDebug, qDebug()
*/

/*!
    \fn QMessageLogContext::QMessageLogContext()
    \internal

    Constructs a QMessageLogContext
*/

/*!
    \fn QMessageLogContext::QMessageLogContext(const char *fileName, int lineNumber, const char *functionName, const char *categoryName)
    \internal

    Constructs a QMessageLogContext with for file \a fileName at line
    \a lineNumber, in function \a functionName, and category \a categoryName.
*/

QT_END_NAMESPACE
