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

#include "qeventdispatcher_cf_p.h"

#include <QtCore/qdebug.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qthread.h>
#include <QtCore/private/qcoreapplication_p.h>
#include <QtCore/private/qcore_unix_p.h>
#include <QtCore/private/qthread_p.h>

#include <limits>

#ifdef Q_OS_OSX
#  include <AppKit/NSApplication.h>
#elif defined(Q_OS_WATCHOS)
#  include <WatchKit/WatchKit.h>
#else
#  include <UIKit/UIApplication.h>
#endif

QT_USE_NAMESPACE

/*
    During scroll view panning, and possibly other gestures, UIKit will
    request a switch to UITrackingRunLoopMode via GSEventPushRunLoopMode,
    which records the new runloop mode and stops the current runloop.

    Unfortunately the runloop mode is just stored on an internal stack, used
    when UIKit itself is running the runloop, and is not available through e.g.
    CFRunLoopCopyCurrentMode, which only knows about the current running
    runloop mode, not the requested future runloop mode.

    To ensure that we pick up this new runloop mode and use it when calling
    CFRunLoopRunInMode from processEvents, we listen for the notification
    emitted by [UIApplication pushRunLoopMode:requester:].

    Without this workaround we end up always running in the default runloop
    mode, resulting in missing momentum-phases in UIScrollViews such as the
    emoji keyboard.
*/
@interface QT_MANGLE_NAMESPACE(RunLoopModeTracker) : NSObject
@end

QT_NAMESPACE_ALIAS_OBJC_CLASS(RunLoopModeTracker);

@implementation RunLoopModeTracker {
    QStack<CFStringRef> m_runLoopModes;
}

- (instancetype)init
{
    if ((self = [super init])) {
        m_runLoopModes.push(kCFRunLoopDefaultMode);

#if !defined(Q_OS_WATCHOS)
        if (!qt_apple_isApplicationExtension()) {
            [[NSNotificationCenter defaultCenter]
                addObserver:self selector:@selector(receivedNotification:)
                name:nil object:qt_apple_sharedApplication()];
        }
#endif
    }

    return self;
}

- (void)dealloc
{
    [NSNotificationCenter.defaultCenter removeObserver:self];

    [super dealloc];
}

static CFStringRef runLoopMode(NSDictionary *dictionary)
{
    for (NSString *key in dictionary) {
        if (CFStringHasSuffix((CFStringRef)key, CFSTR("RunLoopMode")))
            return (CFStringRef)dictionary[key];
    }

    return nil;
}

- (void)receivedNotification:(NSNotification *)notification
{
     if (CFStringHasSuffix((CFStringRef)notification.name, CFSTR("RunLoopModePushNotification"))) {
        if (CFStringRef mode = runLoopMode(notification.userInfo))
            m_runLoopModes.push(mode);
        else
            qCWarning(lcEventDispatcher) << "Encountered run loop push notification without run loop mode!";

     } else if (CFStringHasSuffix((CFStringRef)notification.name, CFSTR("RunLoopModePopNotification"))) {
        CFStringRef mode = runLoopMode(notification.userInfo);
        if (CFStringCompare(mode, self.currentMode, 0) == kCFCompareEqualTo)
            m_runLoopModes.pop();
        else
            qCWarning(lcEventDispatcher) << "Tried to pop run loop mode"
                << qPrintable(QString::fromCFString(mode)) << "that was never pushed!";

        Q_ASSERT(m_runLoopModes.size() >= 1);
     }
}

- (CFStringRef)currentMode
{
    return m_runLoopModes.top();
}

@end

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcEventDispatcher, "qt.eventdispatcher");
Q_LOGGING_CATEGORY(lcEventDispatcherTimers, "qt.eventdispatcher.timers");

class RunLoopDebugger : public QObject
{
    Q_OBJECT

    Q_ENUMS(Activity)
    Q_ENUMS(Result)

public:

    #define Q_MIRROR_ENUM(name) name = name

    enum Activity {
        Q_MIRROR_ENUM(kCFRunLoopEntry),
        Q_MIRROR_ENUM(kCFRunLoopBeforeTimers),
        Q_MIRROR_ENUM(kCFRunLoopBeforeSources),
        Q_MIRROR_ENUM(kCFRunLoopBeforeWaiting),
        Q_MIRROR_ENUM(kCFRunLoopAfterWaiting),
        Q_MIRROR_ENUM(kCFRunLoopExit)
    };

    enum Result {
        Q_MIRROR_ENUM(kCFRunLoopRunFinished),
        Q_MIRROR_ENUM(kCFRunLoopRunStopped),
        Q_MIRROR_ENUM(kCFRunLoopRunTimedOut),
        Q_MIRROR_ENUM(kCFRunLoopRunHandledSource)
    };
};

#define Q_ENUM_PRINTER(enumName) \
    static const char* qPrintable##enumName(int value) \
    { \
        return RunLoopDebugger::staticMetaObject.enumerator(RunLoopDebugger::staticMetaObject.indexOfEnumerator(#enumName)).valueToKey(value); \
    }

Q_ENUM_PRINTER(Activity);
Q_ENUM_PRINTER(Result);

QDebug operator<<(QDebug s, timespec tv)
{
    s << tv.tv_sec << "." << qSetFieldWidth(9) << qSetPadChar(QChar(48)) << tv.tv_nsec << reset;
    return s;
}

static const CFTimeInterval kCFTimeIntervalMinimum = 0;
static const CFTimeInterval kCFTimeIntervalDistantFuture = std::numeric_limits<CFTimeInterval>::max();

#pragma mark - Class definition

QEventDispatcherCoreFoundation::QEventDispatcherCoreFoundation(QObject *parent)
    : QAbstractEventDispatcher(parent)
    , m_processEvents(QEventLoop::EventLoopExec)
    , m_postedEventsRunLoopSource(this, &QEventDispatcherCoreFoundation::processPostedEvents)
    , m_runLoopActivityObserver(this, &QEventDispatcherCoreFoundation::handleRunLoopActivity, kCFRunLoopAllActivities)
    , m_runLoopModeTracker([[RunLoopModeTracker alloc] init])
    , m_runLoopTimer(0)
    , m_blockedRunLoopTimer(0)
    , m_overdueTimerScheduled(false)
{
}

void QEventDispatcherCoreFoundation::startingUp()
{
    // The following code must run on the event dispatcher thread, so that
    // CFRunLoopGetCurrent() returns the correct run loop.
    Q_ASSERT(QThread::currentThread() == thread());

    m_runLoop = QCFType<CFRunLoopRef>::constructFromGet(CFRunLoopGetCurrent());
    m_cfSocketNotifier.setHostEventDispatcher(this);
    m_postedEventsRunLoopSource.addToMode(kCFRunLoopCommonModes);
    m_runLoopActivityObserver.addToMode(kCFRunLoopCommonModes);
}

QEventDispatcherCoreFoundation::~QEventDispatcherCoreFoundation()
{
    invalidateTimer();
    qDeleteAll(m_timerInfoList);

    m_cfSocketNotifier.removeSocketNotifiers();
}

QEventLoop *QEventDispatcherCoreFoundation::currentEventLoop() const
{
    QEventLoop *eventLoop = QThreadData::current()->eventLoops.top();
    Q_ASSERT(eventLoop);
    return eventLoop;
}

/*!
    Processes all pending events that match \a flags until there are no
    more events to process. Returns \c true if pending events were handled;
    otherwise returns \c false.

    Note:

      - All events are considered equal. This function should process
        both system/native events (that we may or may not care about),
        as well as Qt-events (posted events and timers).

      - The function should not return until all queued/available events
        have been processed. If the WaitForMoreEvents is set, the
        function should wait only if there were no events ready,
        and _then_ process all newly queued/available events.

    These notes apply to other function in this class as well, such as
    hasPendingEvents().
*/
bool QEventDispatcherCoreFoundation::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    QT_APPLE_SCOPED_LOG_ACTIVITY(lcEventDispatcher().isDebugEnabled(), "processEvents");

    bool eventsProcessed = false;

    if (flags & (QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers))
        qCWarning(lcEventDispatcher) << "processEvents() flags" << flags << "not supported on iOS";

    qCDebug(lcEventDispatcher) << "Processing events with flags" << flags;

    if (m_blockedRunLoopTimer) {
        Q_ASSERT(m_blockedRunLoopTimer == m_runLoopTimer);

        qCDebug(lcEventDispatcher) << "Recursing from blocked timer" << m_blockedRunLoopTimer;
        m_runLoopTimer = 0; // Unset current timer to force creation of new timer
        updateTimers();
    }

    if (m_processEvents.deferredWakeUp) {
        // We may be processing events recursivly as a result of processing a posted event,
        // in which case we need to signal the run-loop source so that this iteration of
        // processEvents will take care of the newly posted events.
        m_postedEventsRunLoopSource.signal();
        m_processEvents.deferredWakeUp = false;

        qCDebug(lcEventDispatcher) << "Processed deferred wake-up";
    }

    // The documentation states that this signal is emitted after the event
    // loop returns from a function that could block, which is not the case
    // here, but all the other event dispatchers emit awake at the start of
    // processEvents, and the QEventLoop auto-test has an explicit check for
    // this behavior, so we assume it's for a good reason and do it as well.
    emit awake();

    ProcessEventsState previousState = m_processEvents;
    m_processEvents = ProcessEventsState(flags);

    bool returnAfterSingleSourceHandled = !(m_processEvents.flags & QEventLoop::EventLoopExec);

    Q_FOREVER {
        CFStringRef mode = [m_runLoopModeTracker currentMode];

        CFTimeInterval duration = (m_processEvents.flags & QEventLoop::WaitForMoreEvents) ?
            kCFTimeIntervalDistantFuture : kCFTimeIntervalMinimum;

        qCDebug(lcEventDispatcher) << "Calling CFRunLoopRunInMode =" << qPrintable(QString::fromCFString(mode))
            << "for" << duration << "ms, processing single source =" << returnAfterSingleSourceHandled;

        SInt32 result = CFRunLoopRunInMode(mode, duration, returnAfterSingleSourceHandled);

        qCDebug(lcEventDispatcher) << "result =" << qPrintableResult(result);

        eventsProcessed |= (result == kCFRunLoopRunHandledSource
                            || m_processEvents.processedPostedEvents
                            || m_processEvents.processedTimers);

        if (result == kCFRunLoopRunFinished) {
            // This should only happen at application shutdown, as the main runloop
            // will presumably always have sources registered.
            break;
        } else if (m_processEvents.wasInterrupted) {

            if (m_processEvents.flags & QEventLoop::EventLoopExec) {
                Q_ASSERT(result == kCFRunLoopRunStopped);

                // The runloop was potentially stopped (interrupted) by us, as a response to
                // a Qt event loop being asked to exit. We check that the topmost eventloop
                // is still supposed to keep going and return if not. Note that the runloop
                // might get stopped as a result of a non-top eventloop being asked to exit,
                // in which case we continue running the top event loop until that is asked
                // to exit, and then unwind back to the previous event loop which will break
                // immediately, since it has already been exited.

                if (!currentEventLoop()->isRunning()) {
                    qCDebug(lcEventDispatcher) << "Top level event loop was exited";
                    break;
                } else {
                    qCDebug(lcEventDispatcher) << "Top level event loop still running, making another pass";
                }
            } else {
                // We were called manually, through processEvents(), and should stop processing
                // events, even if we didn't finish processing all the queued events.
                qCDebug(lcEventDispatcher) << "Top level processEvents was interrupted";
                break;
            }
        }

        if (m_processEvents.flags & QEventLoop::EventLoopExec) {
            // We were called from QEventLoop's exec(), which blocks until the event
            // loop is asked to exit by calling processEvents repeatedly. Instead of
            // re-entering this method again and again from QEventLoop, we can block
            // here,  one lever closer to CFRunLoopRunInMode, by running the native
            // event loop again and again until we're interrupted by QEventLoop.
            continue;
        } else {
            // We were called 'manually', through processEvents()

            if (result == kCFRunLoopRunHandledSource) {
                // We processed one or more sources, but there might still be other
                // sources that did not get a chance to process events, so we need
                // to do another pass.

                // But we should only wait for more events the first time
                m_processEvents.flags &= ~QEventLoop::WaitForMoreEvents;
                continue;

            } else if (m_overdueTimerScheduled && !m_processEvents.processedTimers) {
                // CFRunLoopRunInMode does not guarantee that a scheduled timer with a fire
                // date in the past (overdue) will fire on the next run loop pass. The Qt
                // APIs on the other hand document eg. zero-interval timers to always be
                // handled after processing all available window-system events.
                qCDebug(lcEventDispatcher) << "Manually processing timers due to overdue timer";
                processTimers(0);
                eventsProcessed = true;
            }
        }

        break;
    }

    if (m_blockedRunLoopTimer) {
        invalidateTimer();
        m_runLoopTimer = m_blockedRunLoopTimer;
    }

    if (m_processEvents.deferredUpdateTimers)
        updateTimers();

    if (m_processEvents.deferredWakeUp) {
        m_postedEventsRunLoopSource.signal();
        qCDebug(lcEventDispatcher) << "Processed deferred wake-up";
    }

    bool wasInterrupted = m_processEvents.wasInterrupted;

    // Restore state of previous processEvents() call
    m_processEvents = previousState;

    if (wasInterrupted) {
        // The current processEvents run has been interrupted, but there may still be
        // others below it (eg, in the case of nested event loops). We need to trigger
        // another interrupt so that the parent processEvents call has a chance to check
        // if it should continue.
        qCDebug(lcEventDispatcher) << "Forwarding interrupt in case of nested processEvents";
        interrupt();
    }

    qCDebug(lcEventDispatcher) << "Returning with eventsProcessed =" << eventsProcessed;

    return eventsProcessed;
}

bool QEventDispatcherCoreFoundation::processPostedEvents()
{
    QT_APPLE_SCOPED_LOG_ACTIVITY(lcEventDispatcher().isDebugEnabled(), "processPostedEvents");

    if (m_processEvents.processedPostedEvents && !(m_processEvents.flags & QEventLoop::EventLoopExec)) {
        qCDebug(lcEventDispatcher) << "Already processed events this pass";
        return false;
    }

    m_processEvents.processedPostedEvents = true;

    qCDebug(lcEventDispatcher) << "Sending posted events for"
        << QEventLoop::ProcessEventsFlags(m_processEvents.flags.load());
    QCoreApplication::sendPostedEvents();

    return true;
}

void QEventDispatcherCoreFoundation::processTimers(CFRunLoopTimerRef timer)
{
    QT_APPLE_SCOPED_LOG_ACTIVITY(lcEventDispatcher().isDebugEnabled(), "processTimers");

    if (m_processEvents.processedTimers && !(m_processEvents.flags & QEventLoop::EventLoopExec)) {
        qCDebug(lcEventDispatcher) << "Already processed timers this pass";
        m_processEvents.deferredUpdateTimers = true;
        return;
    }

    qCDebug(lcEventDispatcher) << "CFRunLoopTimer" << timer << "fired, activating Qt timers";

    // Activating Qt timers might recurse into processEvents() if a timer-callback
    // brings up a new event-loop or tries to processes events manually. Although
    // a CFRunLoop can recurse inside its callbacks, a single CFRunLoopTimer can
    // not. So, for each recursion into processEvents() from a timer-callback we
    // need to set up a new timer-source. Instead of doing it preemtivly each
    // time we activate Qt timers, we set a flag here, and let processEvents()
    // decide whether or not it needs to bring up a new timer source.

    // We may have multiple recused timers, so keep track of the previous blocked timer
    CFRunLoopTimerRef previouslyBlockedRunLoopTimer = m_blockedRunLoopTimer;

    m_blockedRunLoopTimer = timer;
    m_timerInfoList.activateTimers();
    m_blockedRunLoopTimer = previouslyBlockedRunLoopTimer;
    m_processEvents.processedTimers = true;

    // Now that the timer source is unblocked we may need to schedule it again
    updateTimers();
}

Q_LOGGING_CATEGORY(lcEventDispatcherActivity, "qt.eventdispatcher.activity")

void QEventDispatcherCoreFoundation::handleRunLoopActivity(CFRunLoopActivity activity)
{
    qCDebug(lcEventDispatcherActivity) << "Runloop entered activity" << qPrintableActivity(activity);

    switch (activity) {
    case kCFRunLoopBeforeWaiting:
        if (m_processEvents.processedTimers
                && !(m_processEvents.flags & QEventLoop::EventLoopExec)
                && m_processEvents.flags & QEventLoop::WaitForMoreEvents) {
            // CoreFoundation does not treat a timer as a reason to exit CFRunLoopRunInMode
            // when asked to only process a single source, so we risk waiting a long time for
            // a 'proper' source to fire (typically a system source that we don't control).
            // To fix this we do an explicit interrupt after processing our timer, so that
            // processEvents() gets a chance to re-evaluate the state of things.
            interrupt();
        }
        emit aboutToBlock();
        break;
    case kCFRunLoopAfterWaiting:
        emit awake();
        break;
    case kCFRunLoopEntry:
    case kCFRunLoopBeforeTimers:
    case kCFRunLoopBeforeSources:
    case kCFRunLoopExit:
        break;
    default:
        Q_UNREACHABLE();
    }
}

bool QEventDispatcherCoreFoundation::hasPendingEvents()
{
    // There doesn't seem to be any API on iOS to peek into the other sources
    // to figure out if there are pending non-Qt events. As a workaround, we
    // assume that if the run-loop is currently blocking and waiting for a
    // source to signal then there are no system-events pending. If this
    // function is called from the main thread then the second clause
    // of the condition will always be true, as the run loop is
    // never waiting in that case. The function would be more aptly named
    // 'maybeHasPendingEvents' in our case.

    extern uint qGlobalPostedEventsCount();
    return qGlobalPostedEventsCount() || !CFRunLoopIsWaiting(m_runLoop);
}

void QEventDispatcherCoreFoundation::wakeUp()
{
    if (m_processEvents.processedPostedEvents && !(m_processEvents.flags & QEventLoop::EventLoopExec)) {
        // A manual processEvents call should only result in processing the events posted
        // up until then. Any newly posted events as result of processing existing posted
        // events should be handled in the next call to processEvents(). Since we're using
        // a run-loop source to process our posted events we need to prevent it from being
        // signaled as a result of posting new events, otherwise we end up in an infinite
        // loop. We do however need to signal the source at some point, so that the newly
        // posted event gets processed on the next processEvents() call, so we flag the
        // need to do a deferred wake-up.
        m_processEvents.deferredWakeUp = true;
        qCDebug(lcEventDispatcher) << "Already processed posted events, deferring wakeUp";
        return;
    }

    m_postedEventsRunLoopSource.signal();
    if (m_runLoop)
        CFRunLoopWakeUp(m_runLoop);

    qCDebug(lcEventDispatcher) << "Signaled posted event run-loop source";
}

void QEventDispatcherCoreFoundation::interrupt()
{
    qCDebug(lcEventDispatcher) << "Marking current processEvent as interrupted";
    m_processEvents.wasInterrupted = true;
    CFRunLoopStop(m_runLoop);
}

void QEventDispatcherCoreFoundation::flush()
{
    // X11 only.
}

#pragma mark - Socket notifiers

void QEventDispatcherCoreFoundation::registerSocketNotifier(QSocketNotifier *notifier)
{
    m_cfSocketNotifier.registerSocketNotifier(notifier);
}

void QEventDispatcherCoreFoundation::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    m_cfSocketNotifier.unregisterSocketNotifier(notifier);
}

#pragma mark - Timers

void QEventDispatcherCoreFoundation::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object)
{
    qCDebug(lcEventDispatcherTimers) << "Registering timer with id =" << timerId << "interval =" << interval
        << "type =" << timerType << "object =" << object;

    Q_ASSERT(timerId > 0 && interval >= 0 && object);
    Q_ASSERT(object->thread() == thread() && thread() == QThread::currentThread());

    m_timerInfoList.registerTimer(timerId, interval, timerType, object);
    updateTimers();
}

bool QEventDispatcherCoreFoundation::unregisterTimer(int timerId)
{
    Q_ASSERT(timerId > 0);
    Q_ASSERT(thread() == QThread::currentThread());

    bool returnValue = m_timerInfoList.unregisterTimer(timerId);

    qCDebug(lcEventDispatcherTimers) << "Unegistered timer with id =" << timerId << "Timers left:" << m_timerInfoList.size();

    updateTimers();
    return returnValue;
}

bool QEventDispatcherCoreFoundation::unregisterTimers(QObject *object)
{
    Q_ASSERT(object && object->thread() == thread() && thread() == QThread::currentThread());

    bool returnValue = m_timerInfoList.unregisterTimers(object);

    qCDebug(lcEventDispatcherTimers) << "Unegistered timers for object =" << object << "Timers left:" << m_timerInfoList.size();

    updateTimers();
    return returnValue;
}

QList<QAbstractEventDispatcher::TimerInfo> QEventDispatcherCoreFoundation::registeredTimers(QObject *object) const
{
    Q_ASSERT(object);
    return m_timerInfoList.registeredTimers(object);
}

int QEventDispatcherCoreFoundation::remainingTime(int timerId)
{
    Q_ASSERT(timerId > 0);
    return m_timerInfoList.timerRemainingTime(timerId);
}

static double timespecToSeconds(const timespec &spec)
{
    static double nanosecondsPerSecond = 1.0 * 1000 * 1000 * 1000;
    return spec.tv_sec + (spec.tv_nsec / nanosecondsPerSecond);
}

void QEventDispatcherCoreFoundation::updateTimers()
{
    if (m_timerInfoList.size() > 0) {
        // We have Qt timers registered, so create or reschedule CF timer to match

        timespec tv = { -1, -1 };
        CFAbsoluteTime timeToFire = m_timerInfoList.timerWait(tv) ?
            // We have a timer ready to fire right now, or some time in the future
            CFAbsoluteTimeGetCurrent() + timespecToSeconds(tv)
            // We have timers, but they are all currently blocked by callbacks
            : kCFTimeIntervalDistantFuture;

        if (!m_runLoopTimer) {
            m_runLoopTimer = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault,
                timeToFire, kCFTimeIntervalDistantFuture, 0, 0, ^(CFRunLoopTimerRef timer) {
                processTimers(timer);
            });

            CFRunLoopAddTimer(m_runLoop, m_runLoopTimer, kCFRunLoopCommonModes);
            qCDebug(lcEventDispatcherTimers) << "Created new CFRunLoopTimer" << m_runLoopTimer;

        } else {
            CFRunLoopTimerSetNextFireDate(m_runLoopTimer, timeToFire);
            qCDebug(lcEventDispatcherTimers) << "Re-scheduled CFRunLoopTimer" << m_runLoopTimer;
        }

        m_overdueTimerScheduled = !timespecToSeconds(tv);

        qCDebug(lcEventDispatcherTimers) << "Next timeout in" << tv << "seconds";

    } else {
        // No Qt timers are registered, so make sure we're not running any CF timers
        invalidateTimer();

        m_overdueTimerScheduled = false;
    }
}

void QEventDispatcherCoreFoundation::invalidateTimer()
{
    if (!m_runLoopTimer || (m_runLoopTimer == m_blockedRunLoopTimer))
        return;

    CFRunLoopTimerInvalidate(m_runLoopTimer);
    qCDebug(lcEventDispatcherTimers) << "Invalidated CFRunLoopTimer" << m_runLoopTimer;

    CFRelease(m_runLoopTimer);
    m_runLoopTimer = 0;
}

#include "qeventdispatcher_cf.moc"
#include "moc_qeventdispatcher_cf_p.cpp"

QT_END_NAMESPACE
