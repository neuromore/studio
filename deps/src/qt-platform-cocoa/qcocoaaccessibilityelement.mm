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
#include "qcocoaaccessibilityelement.h"
#include "qcocoaaccessibility.h"
#include "qcocoahelpers.h"
#include "qcocoawindow.h"
#include "qcocoascreen.h"
#include "private/qaccessiblecache_p.h"
#include <QtAccessibilitySupport/private/qaccessiblebridgeutils_p.h>
#include <QtGui/qaccessible.h>

#import <AppKit/NSAccessibility.h>

QT_USE_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

static void convertLineOffset(QAccessibleTextInterface *text, int *line, int *offset, NSUInteger *start = 0, NSUInteger *end = 0)
{
    Q_ASSERT(*line == -1 || *offset == -1);
    Q_ASSERT(*line != -1 || *offset != -1);
    Q_ASSERT(*offset <= text->characterCount());

    int curLine = -1;
    int curStart = 0, curEnd = 0;

    do {
        curStart = curEnd;
        text->textAtOffset(curStart, QAccessible::LineBoundary, &curStart, &curEnd);
        // If the text is empty then we just return
        if (curStart == -1 || curEnd == -1) {
            if (start)
                *start = 0;
            if (end)
                *end = 0;
            return;
        }
        ++curLine;
        {
            // check for a case where a single word longer than the text edit's width and gets wrapped
            // in the middle of the word; in this case curEnd will be an offset belonging to the next line
            // and therefore nextEnd will not be equal to curEnd
            int nextStart;
            int nextEnd;
            text->textAtOffset(curEnd, QAccessible::LineBoundary, &nextStart, &nextEnd);
            if (nextEnd == curEnd)
                ++curEnd;
        }
    } while ((*line == -1 || curLine < *line) && (*offset == -1 || (curEnd <= *offset)) && curEnd <= text->characterCount());

    curEnd = qMin(curEnd, text->characterCount());

    if (*line == -1)
        *line = curLine;
    if (*offset == -1)
        *offset = curStart;

    Q_ASSERT(curStart >= 0);
    Q_ASSERT(curEnd >= 0);
    if (start)
        *start = curStart;
    if (end)
        *end = curEnd;
}

@implementation QMacAccessibilityElement {
    QAccessible::Id axid;
}

- (instancetype)initWithId:(QAccessible::Id)anId
{
    Q_ASSERT((int)anId < 0);
    self = [super init];
    if (self) {
        axid = anId;
    }

    return self;
}

+ (instancetype)elementWithId:(QAccessible::Id)anId
{
    Q_ASSERT(anId);
    if (!anId)
        return nil;

    QAccessibleCache *cache = QAccessibleCache::instance();

    QMacAccessibilityElement *element = cache->elementForId(anId);
    if (!element) {
        QAccessibleInterface *iface = QAccessible::accessibleInterface(anId);
        Q_ASSERT(iface);
        if (!iface || !iface->isValid())
            return nil;
        element = [[self alloc] initWithId:anId];
        cache->insertElement(anId, element);
    }
    return element;
}

- (void)invalidate {
    axid = 0;
    NSAccessibilityPostNotification(self, NSAccessibilityUIElementDestroyedNotification);
    [self release];
}

- (void)dealloc {
    [super dealloc];
}

- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[QMacAccessibilityElement class]]) {
        QMacAccessibilityElement *other = object;
        return other->axid == axid;
    } else {
        return NO;
    }
}

- (NSUInteger)hash {
    return axid;
}

//
// accessibility protocol
//

// attributes

+ (id) lineNumberForIndex: (int)index forText:(const QString &)text
{
    QStringRef textBefore = QStringRef(&text, 0, index);
    int newlines = textBefore.count(QLatin1Char('\n'));
    return @(newlines);
}

- (BOOL) accessibilityNotifiesWhenDestroyed {
    return YES;
}

- (NSArray<NSString *> *)accessibilityAttributeNames {
    static NSArray<NSString *> *defaultAttributes = [@[
        NSAccessibilityRoleAttribute,
        NSAccessibilityRoleDescriptionAttribute,
        NSAccessibilitySubroleAttribute,
        NSAccessibilityChildrenAttribute,
        NSAccessibilityFocusedAttribute,
        NSAccessibilityParentAttribute,
        NSAccessibilityWindowAttribute,
        NSAccessibilityTopLevelUIElementAttribute,
        NSAccessibilityPositionAttribute,
        NSAccessibilitySizeAttribute,
        NSAccessibilityTitleAttribute,
        NSAccessibilityDescriptionAttribute,
        NSAccessibilityEnabledAttribute
    ] retain];

    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return defaultAttributes;

    NSMutableArray<NSString *> *attributes = [[NSMutableArray<NSString *> alloc] initWithCapacity:defaultAttributes.count];
    [attributes addObjectsFromArray:defaultAttributes];

    if (QCocoaAccessible::hasValueAttribute(iface)) {
        [attributes addObject:NSAccessibilityValueAttribute];
    }

    if (iface->textInterface()) {
        [attributes addObjectsFromArray:@[
            NSAccessibilityNumberOfCharactersAttribute,
            NSAccessibilitySelectedTextAttribute,
            NSAccessibilitySelectedTextRangeAttribute,
            NSAccessibilityVisibleCharacterRangeAttribute,
            NSAccessibilityInsertionPointLineNumberAttribute
        ]];

// TODO: multi-selection: NSAccessibilitySelectedTextRangesAttribute,
    }

    if (iface->valueInterface()) {
        [attributes addObjectsFromArray:@[
            NSAccessibilityMinValueAttribute,
            NSAccessibilityMaxValueAttribute
        ]];
    }

    return [attributes autorelease];
}

- (id)parentElement {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return nil;

    // macOS expects that the hierarchy is:
    // App -> Window -> Children
    // We don't actually have the window reflected properly in QAccessibility.
    // Check if the parent is the application and then instead return the native window.

    if (QAccessibleInterface *parent = iface->parent()) {
        if (parent->role() != QAccessible::Application) {
            QAccessible::Id parentId = QAccessible::uniqueId(parent);
            return [QMacAccessibilityElement elementWithId: parentId];
        }
    }

    if (QWindow *window = iface->window()) {
        QPlatformWindow *platformWindow = window->handle();
        if (platformWindow) {
            QCocoaWindow *win = static_cast<QCocoaWindow*>(platformWindow);
            return qnsview_cast(win->view());
        }
    }
    return nil;
}


- (id) minValueAttribute:(QAccessibleInterface*)iface {
    if (QAccessibleValueInterface *val = iface->valueInterface())
        return @(val->minimumValue().toDouble());
    return nil;
}

- (id) maxValueAttribute:(QAccessibleInterface*)iface {
    if (QAccessibleValueInterface *val = iface->valueInterface())
        return @(val->maximumValue().toDouble());
    return nil;
}

- (id)accessibilityAttributeValue:(NSString *)attribute {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid()) {
        qWarning() << "Called attribute on invalid object: " << axid;
        return nil;
    }

    if ([attribute isEqualToString:NSAccessibilityRoleAttribute]) {
        return QCocoaAccessible::macRole(iface);
    } else if ([attribute isEqualToString:NSAccessibilitySubroleAttribute]) {
        return QCocoaAccessible::macSubrole(iface);
    } else if ([attribute isEqualToString:NSAccessibilityRoleDescriptionAttribute]) {
        return NSAccessibilityRoleDescription(QCocoaAccessible::macRole(iface),
            [self accessibilityAttributeValue:NSAccessibilitySubroleAttribute]);
    } else if ([attribute isEqualToString:NSAccessibilityChildrenAttribute]) {
        return QCocoaAccessible::unignoredChildren(iface);
    } else if ([attribute isEqualToString:NSAccessibilityFocusedAttribute]) {
        // Just check if the app thinks we're focused.
        id focusedElement = [NSApp accessibilityAttributeValue:NSAccessibilityFocusedUIElementAttribute];
        return @([focusedElement isEqual:self]);
    } else if ([attribute isEqualToString:NSAccessibilityParentAttribute]) {
        return NSAccessibilityUnignoredAncestor([self parentElement]);
    } else if ([attribute isEqualToString:NSAccessibilityWindowAttribute]) {
        // We're in the same window as our parent.
        return [[self parentElement] accessibilityAttributeValue:NSAccessibilityWindowAttribute];
    } else if ([attribute isEqualToString:NSAccessibilityTopLevelUIElementAttribute]) {
        // We're in the same top level element as our parent.
        return [[self parentElement] accessibilityAttributeValue:NSAccessibilityTopLevelUIElementAttribute];
    } else if ([attribute isEqualToString:NSAccessibilityPositionAttribute]) {
        // The position in points of the element's lower-left corner in screen-relative coordinates
        QPointF qtPosition = QRectF(iface->rect()).bottomLeft();
        return [NSValue valueWithPoint:QCocoaScreen::mapToNative(qtPosition)];
    } else if ([attribute isEqualToString:NSAccessibilitySizeAttribute]) {
        QSize qtSize = iface->rect().size();
        return [NSValue valueWithSize: NSMakeSize(qtSize.width(), qtSize.height())];
    } else if ([attribute isEqualToString:NSAccessibilityTitleAttribute]) {
        if (iface->role() == QAccessible::StaticText)
            return nil;
        return iface->text(QAccessible::Name).toNSString();
    } else if ([attribute isEqualToString:NSAccessibilityDescriptionAttribute]) {
        return iface->text(QAccessible::Description).toNSString();
    } else if ([attribute isEqualToString:NSAccessibilityEnabledAttribute]) {
        return @(!iface->state().disabled);
    } else if ([attribute isEqualToString:NSAccessibilityValueAttribute]) {
        // VoiceOver asks for the value attribute for all elements. Return nil
        // if we don't want the element to have a value attribute.
        if (!QCocoaAccessible::hasValueAttribute(iface))
            return nil;

        return QCocoaAccessible::getValueAttribute(iface);

    } else if ([attribute isEqualToString:NSAccessibilityNumberOfCharactersAttribute]) {
        if (QAccessibleTextInterface *text = iface->textInterface())
            return @(text->characterCount());
        return nil;
    } else if ([attribute isEqualToString:NSAccessibilitySelectedTextAttribute]) {
        if (QAccessibleTextInterface *text = iface->textInterface()) {
            int start = 0;
            int end = 0;
            text->selection(0, &start, &end);
            return text->text(start, end).toNSString();
        }
        return nil;
    } else if ([attribute isEqualToString:NSAccessibilitySelectedTextRangeAttribute]) {
        if (QAccessibleTextInterface *text = iface->textInterface()) {
            int start = 0;
            int end = 0;
            if (text->selectionCount() > 0) {
                text->selection(0, &start, &end);
            } else {
                start = text->cursorPosition();
                end = start;
            }
            return [NSValue valueWithRange:NSMakeRange(quint32(start), quint32(end - start))];
        }
        return [NSValue valueWithRange: NSMakeRange(0, 0)];
    } else if ([attribute isEqualToString:NSAccessibilityVisibleCharacterRangeAttribute]) {
        // FIXME This is not correct and may impact performance for big texts
        if (QAccessibleTextInterface *text = iface->textInterface())
            return [NSValue valueWithRange: NSMakeRange(0, text->characterCount())];
        return [NSValue valueWithRange: NSMakeRange(0, iface->text(QAccessible::Name).length())];
    } else if ([attribute isEqualToString:NSAccessibilityInsertionPointLineNumberAttribute]) {
        if (QAccessibleTextInterface *text = iface->textInterface()) {
            int line = 0; // true for all single line edits
            if (iface->state().multiLine) {
                int position = text->cursorPosition();
                convertLineOffset(text, &line, &position);
            }
            return @(line);
        }
        return nil;
    } else if ([attribute isEqualToString:NSAccessibilityMinValueAttribute]) {
        return [self minValueAttribute:iface];
    } else if ([attribute isEqualToString:NSAccessibilityMaxValueAttribute]) {
        return [self maxValueAttribute:iface];
    }

    return nil;
}

- (NSArray *)accessibilityParameterizedAttributeNames {

    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid()) {
        qWarning() << "Called attribute on invalid object: " << axid;
        return nil;
    }

    if (iface->textInterface()) {
        return @[
            NSAccessibilityStringForRangeParameterizedAttribute,
            NSAccessibilityLineForIndexParameterizedAttribute,
            NSAccessibilityRangeForLineParameterizedAttribute,
            NSAccessibilityRangeForPositionParameterizedAttribute,
//          NSAccessibilityRangeForIndexParameterizedAttribute,
            NSAccessibilityBoundsForRangeParameterizedAttribute,
//          NSAccessibilityRTFForRangeParameterizedAttribute,
            NSAccessibilityStyleRangeForIndexParameterizedAttribute,
            NSAccessibilityAttributedStringForRangeParameterizedAttribute
        ];
    }

    return nil;
}

- (id)accessibilityAttributeValue:(NSString *)attribute forParameter:(id)parameter {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid()) {
        qWarning() << "Called attribute on invalid object: " << axid;
        return nil;
    }

    if (!iface->textInterface())
        return nil;

    if ([attribute isEqualToString: NSAccessibilityStringForRangeParameterizedAttribute]) {
        NSRange range = [parameter rangeValue];
        QString text = iface->textInterface()->text(range.location, range.location + range.length);
        return text.toNSString();
    }
    if ([attribute isEqualToString: NSAccessibilityLineForIndexParameterizedAttribute]) {
        int index = [parameter intValue];
        if (index < 0 || index > iface->textInterface()->characterCount())
            return nil;
        int line = -1;
        convertLineOffset(iface->textInterface(), &line, &index);
        return @(line);
    }
    if ([attribute isEqualToString: NSAccessibilityRangeForLineParameterizedAttribute]) {
        int line = [parameter intValue];
        if (line < 0)
            return nil;
        int lineOffset = -1;
        NSUInteger startOffset = 0;
        NSUInteger endOffset = 0;
        convertLineOffset(iface->textInterface(), &line, &lineOffset, &startOffset, &endOffset);
        return [NSValue valueWithRange:NSMakeRange(startOffset, endOffset - startOffset)];
    }
    if ([attribute isEqualToString: NSAccessibilityBoundsForRangeParameterizedAttribute]) {
        NSRange range = [parameter rangeValue];
        QRect firstRect = iface->textInterface()->characterRect(range.location);
        QRectF rect;
        if (range.length > 0) {
            NSUInteger position = range.location + range.length - 1;
            if (position > range.location && iface->textInterface()->text(position, position + 1) == QStringLiteral("\n"))
                --position;
            QRect lastRect = iface->textInterface()->characterRect(position);
            rect = firstRect.united(lastRect);
        } else {
            rect = firstRect;
            rect.setWidth(1);
        }
        return [NSValue valueWithRect:QCocoaScreen::mapToNative(rect)];
    }
    if ([attribute isEqualToString: NSAccessibilityAttributedStringForRangeParameterizedAttribute]) {
        NSRange range = [parameter rangeValue];
        QString text = iface->textInterface()->text(range.location, range.location + range.length);
        return [[NSAttributedString alloc] initWithString:text.toNSString()];
    } else if ([attribute isEqualToString: NSAccessibilityRangeForPositionParameterizedAttribute]) {
        QPoint point = QCocoaScreen::mapFromNative([parameter pointValue]).toPoint();
        int offset = iface->textInterface()->offsetAtPoint(point);
        return [NSValue valueWithRange:NSMakeRange(static_cast<NSUInteger>(offset), 1)];
    } else if ([attribute isEqualToString: NSAccessibilityStyleRangeForIndexParameterizedAttribute]) {
        int start = 0;
        int end = 0;
        iface->textInterface()->attributes([parameter intValue], &start, &end);
        return [NSValue valueWithRange:NSMakeRange(static_cast<NSUInteger>(start), static_cast<NSUInteger>(end - start))];
    }
    return nil;
}

- (BOOL)accessibilityIsAttributeSettable:(NSString *)attribute {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return NO;

    if ([attribute isEqualToString:NSAccessibilityFocusedAttribute]) {
        return iface->state().focusable ? YES : NO;
    } else if ([attribute isEqualToString:NSAccessibilityValueAttribute]) {
        if (iface->textInterface() && iface->state().editable)
            return YES;
        if (iface->valueInterface())
            return YES;
        return NO;
    } else if ([attribute isEqualToString:NSAccessibilitySelectedTextRangeAttribute]) {
        return iface->textInterface() ? YES : NO;
    }
    return NO;
}

- (void)accessibilitySetValue:(id)value forAttribute:(NSString *)attribute {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return;
    if ([attribute isEqualToString:NSAccessibilityFocusedAttribute]) {
        if (QAccessibleActionInterface *action = iface->actionInterface())
            action->doAction(QAccessibleActionInterface::setFocusAction());
    } else if ([attribute isEqualToString:NSAccessibilityValueAttribute]) {
        if (iface->textInterface()) {
            QString text = QString::fromNSString((NSString *)value);
            iface->setText(QAccessible::Value, text);
        } else if (QAccessibleValueInterface *valueIface = iface->valueInterface()) {
            double val = [value doubleValue];
            valueIface->setCurrentValue(val);
        }
    } else if ([attribute isEqualToString:NSAccessibilitySelectedTextRangeAttribute]) {
        if (QAccessibleTextInterface *text = iface->textInterface()) {
            NSRange range = [value rangeValue];
            if (range.length > 0)
                text->setSelection(0, range.location, range.location + range.length);
            else
                text->setCursorPosition(range.location);
        }
    }
}

// actions

- (NSArray *)accessibilityActionNames {
    NSMutableArray *nsActions = [[NSMutableArray new] autorelease];
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return nsActions;

    const QStringList &supportedActionNames = QAccessibleBridgeUtils::effectiveActionNames(iface);
    foreach (const QString &qtAction, supportedActionNames) {
        NSString *nsAction = QCocoaAccessible::getTranslatedAction(qtAction);
        if (nsAction)
            [nsActions addObject : nsAction];
    }

    return nsActions;
}

- (NSString *)accessibilityActionDescription:(NSString *)action {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return nil; // FIXME is that the right return type??
    QString qtAction = QCocoaAccessible::translateAction(action, iface);
    QString description;
    // Return a description from the action interface if this action is not known to the OS.
    if (qtAction.isEmpty()) {
        if (QAccessibleActionInterface *actionInterface = iface->actionInterface()) {
            qtAction = QString::fromNSString((NSString *)action);
            description = actionInterface->localizedActionDescription(qtAction);
        }
    } else {
        description = qAccessibleLocalizedActionDescription(qtAction);
    }
    return description.toNSString();
}

- (void)accessibilityPerformAction:(NSString *)action {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (iface && iface->isValid()) {
        const QString qtAction = QCocoaAccessible::translateAction(action, iface);
        QAccessibleBridgeUtils::performEffectiveAction(iface, qtAction);
    }
}

// misc

- (BOOL)accessibilityIsIgnored {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid())
        return true;
    return QCocoaAccessible::shouldBeIgnored(iface);
}

- (id)accessibilityHitTest:(NSPoint)point {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);
    if (!iface || !iface->isValid()) {
//        qDebug("Hit test: INVALID");
        return NSAccessibilityUnignoredAncestor(self);
    }

    QPointF screenPoint = QCocoaScreen::mapFromNative(point);
    QAccessibleInterface *childInterface = iface->childAt(screenPoint.x(), screenPoint.y());
    // No child found, meaning we hit this element.
    if (!childInterface || !childInterface->isValid())
        return NSAccessibilityUnignoredAncestor(self);

    // find the deepest child at the point
    QAccessibleInterface *childOfChildInterface = nullptr;
    do {
        childOfChildInterface = childInterface->childAt(screenPoint.x(), screenPoint.y());
        if (childOfChildInterface && childOfChildInterface->isValid())
            childInterface = childOfChildInterface;
    } while (childOfChildInterface && childOfChildInterface->isValid());

    QAccessible::Id childId = QAccessible::uniqueId(childInterface);
    // hit a child, forward to child accessible interface.
    QMacAccessibilityElement *accessibleElement = [QMacAccessibilityElement elementWithId:childId];
    if (accessibleElement)
        return NSAccessibilityUnignoredAncestor(accessibleElement);
    return NSAccessibilityUnignoredAncestor(self);
}

- (id)accessibilityFocusedUIElement {
    QAccessibleInterface *iface = QAccessible::accessibleInterface(axid);

    if (!iface || !iface->isValid()) {
        qWarning("FocusedUIElement for INVALID");
        return nil;
    }

    QAccessibleInterface *childInterface = iface->focusChild();
    if (childInterface && childInterface->isValid()) {
        QAccessible::Id childAxid = QAccessible::uniqueId(childInterface);
        QMacAccessibilityElement *accessibleElement = [QMacAccessibilityElement elementWithId:childAxid];
        return NSAccessibilityUnignoredAncestor(accessibleElement);
    }

    return NSAccessibilityUnignoredAncestor(self);
}

@end

#endif // QT_NO_ACCESSIBILITY
