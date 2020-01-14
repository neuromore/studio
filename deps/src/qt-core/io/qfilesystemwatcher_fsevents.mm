/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include <qplatformdefs.h>

#include "qdiriterator.h"
#include "qfilesystemwatcher.h"
#include "qfilesystemwatcher_fsevents_p.h"
#include "private/qcore_unix_p.h"
#include "kernel/qcore_mac_p.h"

#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qvarlengtharray.h>

#undef FSEVENT_DEBUG
#ifdef FSEVENT_DEBUG
#  define DEBUG if (true) qDebug
#else
#  define DEBUG if (false) qDebug
#endif

QT_BEGIN_NAMESPACE

static void callBackFunction(ConstFSEventStreamRef streamRef,
                             void *clientCallBackInfo,
                             size_t numEvents,
                             void *eventPaths,
                             const FSEventStreamEventFlags eventFlags[],
                             const FSEventStreamEventId eventIds[])
{
    QMacAutoReleasePool pool;

    char **paths = static_cast<char **>(eventPaths);
    QFseventsFileSystemWatcherEngine *engine = static_cast<QFseventsFileSystemWatcherEngine *>(clientCallBackInfo);
    engine->processEvent(streamRef, numEvents, paths, eventFlags, eventIds);
}

bool QFseventsFileSystemWatcherEngine::checkDir(DirsByName::iterator &it)
{
    bool needsRestart = false;

    QT_STATBUF st;
    const QString &name = it.key();
    Info &info = it->dirInfo;
    const int res = QT_STAT(QFile::encodeName(name), &st);
    if (res == -1) {
        needsRestart |= derefPath(info.watchedPath);
        emit emitDirectoryChanged(info.origPath, true);
        it = watchingState.watchedDirectories.erase(it);
    } else if (st.st_ctimespec != info.ctime || st.st_mode != info.mode) {
        info.ctime = st.st_ctimespec;
        info.mode = st.st_mode;
        emit emitDirectoryChanged(info.origPath, false);
        ++it;
    } else {
        bool dirChanged = false;
        InfoByName &entries = it->entries;
        // check known entries:
        for (InfoByName::iterator i = entries.begin(); i != entries.end(); ) {
            if (QT_STAT(QFile::encodeName(i.key()), &st) == -1) {
                // entry disappeared
                dirChanged = true;
                i = entries.erase(i);
            } else {
                if (i->ctime != st.st_ctimespec || i->mode != st.st_mode) {
                    // entry changed
                    dirChanged = true;
                    i->ctime = st.st_ctimespec;
                    i->mode = st.st_mode;
                }
                ++i;
            }
        }
        // check for new entries:
        QDirIterator dirIt(name);
        while (dirIt.hasNext()) {
            dirIt.next();
            QString entryName = dirIt.filePath();
            if (!entries.contains(entryName)) {
                dirChanged = true;
                QT_STATBUF st;
                if (QT_STAT(QFile::encodeName(entryName), &st) == -1)
                    continue;
                entries.insert(entryName, Info(QString(), st.st_ctimespec, st.st_mode, QString()));

            }
        }
        if (dirChanged)
            emit emitDirectoryChanged(info.origPath, false);
        ++it;
    }

    return needsRestart;
}

bool QFseventsFileSystemWatcherEngine::rescanDirs(const QString &path)
{
    bool needsRestart = false;

    for (DirsByName::iterator it = watchingState.watchedDirectories.begin();
            it != watchingState.watchedDirectories.end(); ) {
        if (it.key().startsWith(path))
            needsRestart |= checkDir(it);
        else
             ++it;
    }

    return needsRestart;
}

bool QFseventsFileSystemWatcherEngine::rescanFiles(InfoByName &filesInPath)
{
    bool needsRestart = false;

    for (InfoByName::iterator it = filesInPath.begin(); it != filesInPath.end(); ) {
        QT_STATBUF st;
        QString name = it.key();
        const int res = QT_STAT(QFile::encodeName(name), &st);
        if (res == -1) {
            needsRestart |= derefPath(it->watchedPath);
            emit emitFileChanged(it.value().origPath, true);
            it = filesInPath.erase(it);
            continue;
        } else if (st.st_ctimespec != it->ctime || st.st_mode != it->mode) {
            it->ctime = st.st_ctimespec;
            it->mode = st.st_mode;
            emit emitFileChanged(it.value().origPath, false);
        }

        ++it;
    }

    return needsRestart;
}

bool QFseventsFileSystemWatcherEngine::rescanFiles(const QString &path)
{
    bool needsRestart = false;

    for (FilesByPath::iterator i = watchingState.watchedFiles.begin();
            i != watchingState.watchedFiles.end(); ) {
        if (i.key().startsWith(path)) {
            needsRestart |= rescanFiles(i.value());
            if (i.value().isEmpty()) {
                i = watchingState.watchedFiles.erase(i);
                continue;
            }
        }

        ++i;
    }

    return needsRestart;
}

void QFseventsFileSystemWatcherEngine::processEvent(ConstFSEventStreamRef streamRef,
                                                    size_t numEvents,
                                                    char **eventPaths,
                                                    const FSEventStreamEventFlags eventFlags[],
                                                    const FSEventStreamEventId eventIds[])
{
#if defined(Q_OS_OSX)
    Q_UNUSED(streamRef);

    bool needsRestart = false;

    QMutexLocker locker(&lock);

    for (size_t i = 0; i < numEvents; ++i) {
        FSEventStreamEventFlags eFlags = eventFlags[i];
        DEBUG("Change %llu in %s, flags %x", eventIds[i], eventPaths[i], (unsigned int)eFlags);

        if (eFlags & kFSEventStreamEventFlagEventIdsWrapped) {
            DEBUG("\tthe event ids wrapped");
            lastReceivedEvent = 0;
        }
        lastReceivedEvent = qMax(lastReceivedEvent, eventIds[i]);

        QString path = QFile::decodeName(eventPaths[i]);
        if (path.endsWith(QDir::separator()))
            path = path.mid(0, path.size() - 1);

        if (eFlags & kFSEventStreamEventFlagMustScanSubDirs) {
            DEBUG("\tmust rescan directory because of coalesced events");
            if (eFlags & kFSEventStreamEventFlagUserDropped)
                DEBUG("\t\t... user dropped.");
            if (eFlags & kFSEventStreamEventFlagKernelDropped)
                DEBUG("\t\t... kernel dropped.");
            needsRestart |= rescanDirs(path);
            needsRestart |= rescanFiles(path);
            continue;
        }

        if (eFlags & kFSEventStreamEventFlagRootChanged) {
            // re-check everything:
            DirsByName::iterator dirIt = watchingState.watchedDirectories.find(path);
            if (dirIt != watchingState.watchedDirectories.end())
                needsRestart |= checkDir(dirIt);
            needsRestart |= rescanFiles(path);
            continue;
        }

        if ((eFlags & kFSEventStreamEventFlagItemIsDir) && (eFlags & kFSEventStreamEventFlagItemRemoved))
            needsRestart |= rescanDirs(path);

        // check watched directories:
        DirsByName::iterator dirIt = watchingState.watchedDirectories.find(path);
        if (dirIt != watchingState.watchedDirectories.end())
            needsRestart |= checkDir(dirIt);

        // check watched files:
        FilesByPath::iterator pIt = watchingState.watchedFiles.find(path);
        if (pIt != watchingState.watchedFiles.end())
            needsRestart |= rescanFiles(pIt.value());
    }

    if (needsRestart)
        emit scheduleStreamRestart();
#else
    Q_UNUSED(streamRef);
    Q_UNUSED(numEvents);
    Q_UNUSED(eventPaths);
    Q_UNUSED(eventFlags);
    Q_UNUSED(eventIds);
#endif
}

void QFseventsFileSystemWatcherEngine::doEmitFileChanged(const QString &path, bool removed)
{
    DEBUG() << "emitting fileChanged for" << path << "with removed =" << removed;
    emit fileChanged(path, removed);
}

void QFseventsFileSystemWatcherEngine::doEmitDirectoryChanged(const QString &path, bool removed)
{
    DEBUG() << "emitting directoryChanged for" << path << "with removed =" << removed;
    emit directoryChanged(path, removed);
}

bool QFseventsFileSystemWatcherEngine::restartStream()
{
    QMutexLocker locker(&lock);
    stopStream();
    return startStream();
}

QFseventsFileSystemWatcherEngine *QFseventsFileSystemWatcherEngine::create(QObject *parent)
{
    return new QFseventsFileSystemWatcherEngine(parent);
}

QFseventsFileSystemWatcherEngine::QFseventsFileSystemWatcherEngine(QObject *parent)
    : QFileSystemWatcherEngine(parent)
    , stream(0)
    , lastReceivedEvent(kFSEventStreamEventIdSinceNow)
{

    // We cannot use signal-to-signal queued connections, because the
    // QSignalSpy cannot spot signals fired from other/alien threads.
    connect(this, SIGNAL(emitDirectoryChanged(QString,bool)),
            this, SLOT(doEmitDirectoryChanged(QString,bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(emitFileChanged(QString,bool)),
            this, SLOT(doEmitFileChanged(QString,bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(scheduleStreamRestart()),
            this, SLOT(restartStream()), Qt::QueuedConnection);

    queue = dispatch_queue_create("org.qt-project.QFseventsFileSystemWatcherEngine", NULL);
}

QFseventsFileSystemWatcherEngine::~QFseventsFileSystemWatcherEngine()
{
    QMacAutoReleasePool pool;

    // Stop the stream in case we have to wait for the lock below to be acquired.
    if (stream)
        FSEventStreamStop(stream);

    // The assumption with the locking strategy is that this class cannot and will not be subclassed!
    QMutexLocker locker(&lock);

    stopStream(true);
    dispatch_release(queue);
}

QStringList QFseventsFileSystemWatcherEngine::addPaths(const QStringList &paths,
                                                       QStringList *files,
                                                       QStringList *directories)
{
    QMacAutoReleasePool pool;

    if (stream) {
        DEBUG("Flushing, last id is %llu", FSEventStreamGetLatestEventId(stream));
        FSEventStreamFlushSync(stream);
    }

    QMutexLocker locker(&lock);

    bool wasRunning = stream != nullptr;
    bool needsRestart = false;

    WatchingState oldState = watchingState;
    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString origPath = it.next().normalized(QString::NormalizationForm_C);
        QString realPath = origPath;
        if (realPath.endsWith(QDir::separator()))
            realPath = realPath.mid(0, realPath.size() - 1);
        QString watchedPath, parentPath;

        realPath = QFileInfo(realPath).canonicalFilePath();
        QFileInfo fi(realPath);
        if (realPath.isEmpty())
            continue;

        QT_STATBUF st;
        if (QT_STAT(QFile::encodeName(realPath), &st) == -1)
            continue;

        const bool isDir = S_ISDIR(st.st_mode);
        if (isDir) {
            if (watchingState.watchedDirectories.contains(realPath))
                continue;
            directories->append(origPath);
            watchedPath = realPath;
            it.remove();
        } else {
            if (files->contains(origPath))
                continue;
            files->append(origPath);
            it.remove();

            watchedPath = fi.path();
            parentPath = watchedPath;
        }

        for (PathRefCounts::const_iterator i = watchingState.watchedPaths.begin(),
                ei = watchingState.watchedPaths.end(); i != ei; ++i) {
            if (watchedPath.startsWith(i.key() % QDir::separator())) {
                watchedPath = i.key();
                break;
            }
        }

        PathRefCounts::iterator it = watchingState.watchedPaths.find(watchedPath);
        if (it == watchingState.watchedPaths.end()) {
            needsRestart = true;
            watchingState.watchedPaths.insert(watchedPath, 1);
            DEBUG("Adding '%s' to watchedPaths", qPrintable(watchedPath));
        } else {
            ++it.value();
        }

        Info info(origPath, st.st_ctimespec, st.st_mode, watchedPath);
        if (isDir) {
            DirInfo dirInfo;
            dirInfo.dirInfo = info;
            dirInfo.entries = scanForDirEntries(realPath);
            watchingState.watchedDirectories.insert(realPath, dirInfo);
            DEBUG("-- Also adding '%s' to watchedDirectories", qPrintable(realPath));
        } else {
            watchingState.watchedFiles[parentPath].insert(realPath, info);
            DEBUG("-- Also adding '%s' to watchedFiles", qPrintable(realPath));
        }
    }

    if (needsRestart) {
        stopStream();
        if (!startStream()) {
            // ok, something went wrong, let's try to restore the previous state
            watchingState = qMove(oldState);
            // and because we don't know which path caused the issue (if any), fail on all of them
            p = paths;

            if (wasRunning)
                startStream();
        }
    }

    return p;
}

QStringList QFseventsFileSystemWatcherEngine::removePaths(const QStringList &paths,
                                                          QStringList *files,
                                                          QStringList *directories)
{
    QMacAutoReleasePool pool;

    QMutexLocker locker(&lock);

    bool needsRestart = false;

    WatchingState oldState = watchingState;
    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString origPath = it.next();
        QString realPath = origPath;
        if (realPath.endsWith(QDir::separator()))
            realPath = realPath.mid(0, realPath.size() - 1);

        QFileInfo fi(realPath);
        realPath = fi.canonicalFilePath();

        if (fi.isDir()) {
            DirsByName::iterator dirIt = watchingState.watchedDirectories.find(realPath);
            if (dirIt != watchingState.watchedDirectories.end()) {
                needsRestart |= derefPath(dirIt->dirInfo.watchedPath);
                watchingState.watchedDirectories.erase(dirIt);
                directories->removeAll(origPath);
                it.remove();
                DEBUG("Removed directory '%s'", qPrintable(realPath));
            }
        } else {
            QFileInfo fi(realPath);
            QString parentPath = fi.path();
            FilesByPath::iterator pIt = watchingState.watchedFiles.find(parentPath);
            if (pIt != watchingState.watchedFiles.end()) {
                InfoByName &filesInDir = pIt.value();
                InfoByName::iterator fIt = filesInDir.find(realPath);
                if (fIt != filesInDir.end()) {
                    needsRestart |= derefPath(fIt->watchedPath);
                    filesInDir.erase(fIt);
                    if (filesInDir.isEmpty())
                        watchingState.watchedFiles.erase(pIt);
                    files->removeAll(origPath);
                    it.remove();
                    DEBUG("Removed file '%s'", qPrintable(realPath));
                }
            }
        }
    }

    locker.unlock();

    if (needsRestart) {
        if (!restartStream()) {
            watchingState = qMove(oldState);
            startStream();
        }
    }

    return p;
}

// Returns false if FSEventStream* calls failed for some mysterious reason, true if things got a
// thumbs-up.
bool QFseventsFileSystemWatcherEngine::startStream()
{
    Q_ASSERT(stream == 0);
    if (stream) // Ok, this really shouldn't happen, esp. not after the assert. But let's be nice in release mode and still handle it.
        stopStream();

    QMacAutoReleasePool pool;

    if (watchingState.watchedPaths.isEmpty())
        return true; // we succeeded in doing nothing

    DEBUG() << "Starting stream with paths" << watchingState.watchedPaths.keys();

    NSMutableArray<NSString *> *pathsToWatch = [NSMutableArray<NSString *> arrayWithCapacity:watchingState.watchedPaths.size()];
    for (PathRefCounts::const_iterator i = watchingState.watchedPaths.begin(), ei = watchingState.watchedPaths.end(); i != ei; ++i)
        [pathsToWatch addObject:i.key().toNSString()];

    struct FSEventStreamContext callBackInfo = {
        0,
        this,
        NULL,
        NULL,
        NULL
    };
    const CFAbsoluteTime latency = .5; // in seconds

    // Never start with kFSEventStreamEventIdSinceNow, because this will generate an invalid
    // soft-assert in FSEventStreamFlushSync in CarbonCore when no event occurred.
    if (lastReceivedEvent == kFSEventStreamEventIdSinceNow)
        lastReceivedEvent = FSEventsGetCurrentEventId();
    stream = FSEventStreamCreate(NULL,
                                 &callBackFunction,
                                 &callBackInfo,
                                 reinterpret_cast<CFArrayRef>(pathsToWatch),
                                 lastReceivedEvent,
                                 latency,
                                 FSEventStreamCreateFlags(0));

    if (!stream) { // nope, no way to know what went wrong, so just fail
        DEBUG() << "Failed to create stream!";
        return false;
    }

    FSEventStreamSetDispatchQueue(stream, queue);

    if (FSEventStreamStart(stream)) {
        DEBUG() << "Stream started successfully with sinceWhen =" << lastReceivedEvent;
        return true;
    } else { // again, no way to know what went wrong, so just clean up and fail
        DEBUG() << "Stream failed to start!";
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
        stream = 0;
        return false;
    }
}

void QFseventsFileSystemWatcherEngine::stopStream(bool isStopped)
{
    QMacAutoReleasePool pool;
    if (stream) {
        if (!isStopped)
            FSEventStreamStop(stream);
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
        stream = 0;
        DEBUG() << "Stream stopped. Last event ID:" << lastReceivedEvent;
    }
}

QFseventsFileSystemWatcherEngine::InfoByName QFseventsFileSystemWatcherEngine::scanForDirEntries(const QString &path)
{
    InfoByName entries;

    QDirIterator it(path);
    while (it.hasNext()) {
        it.next();
        QString entryName = it.filePath();
        QT_STATBUF st;
        if (QT_STAT(QFile::encodeName(entryName), &st) == -1)
            continue;
        entries.insert(entryName, Info(QString(), st.st_ctimespec, st.st_mode, QString()));
    }

    return entries;
}

bool QFseventsFileSystemWatcherEngine::derefPath(const QString &watchedPath)
{
    PathRefCounts::iterator it = watchingState.watchedPaths.find(watchedPath);
    if (it != watchingState.watchedPaths.end() && --it.value() < 1) {
        watchingState.watchedPaths.erase(it);
        DEBUG("Removing '%s' from watchedPaths.", qPrintable(watchedPath));
        return true;
    }

    return false;
}

QT_END_NAMESPACE
