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
#include "qcocoaaccessibility.h"
#include "qcocoaaccessibilityelement.h"
#include <QtGui/qaccessible.h>
#include <private/qcore_mac_p.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

QCocoaAccessibility::QCocoaAccessibility()
{

}

QCocoaAccessibility::~QCocoaAccessibility()
{

}

void QCocoaAccessibility::notifyAccessibilityUpdate(QAccessibleEvent *event)
{
    if (!isActive() || !event->accessibleInterface() || !event->accessibleInterface()->isValid())
        return;
    QMacAccessibilityElement *element = [QMacAccessibilityElement elementWithId: event->uniqueId()];
    if (!element) {
        qWarning("QCocoaAccessibility::notifyAccessibilityUpdate: invalid element");
        return;
    }

    switch (event->type()) {
    case QAccessible::Focus: {
        NSAccessibilityPostNotification(element, NSAccessibilityFocusedUIElementChangedNotification);
        break;
    }
    case QAccessible::StateChanged:
    case QAccessible::ValueChanged:
    case QAccessible::TextInserted:
    case QAccessible::TextRemoved:
    case QAccessible::TextUpdated:
        NSAccessibilityPostNotification(element, NSAccessibilityValueChangedNotification);
        break;
    case QAccessible::TextCaretMoved:
    case QAccessible::TextSelectionChanged:
        NSAccessibilityPostNotification(element, NSAccessibilitySelectedTextChangedNotification);
        break;
    case QAccessible::NameChanged:
        NSAccessibilityPostNotification(element, NSAccessibilityTitleChangedNotification);
        break;
    default:
        break;
    }
}

void QCocoaAccessibility::setRootObject(QObject *o)
{
    Q_UNUSED(o)
}

void QCocoaAccessibility::initialize()
{

}

void QCocoaAccessibility::cleanup()
{

}

namespace QCocoaAccessible {

typedef QMap<QAccessible::Role, NSString *> QMacAccessibiltyRoleMap;
Q_GLOBAL_STATIC(QMacAccessibiltyRoleMap, qMacAccessibiltyRoleMap);

static void populateRoleMap()
{
    QMacAccessibiltyRoleMap &roleMap = *qMacAccessibiltyRoleMap();
    roleMap[QAccessible::MenuItem] = NSAccessibilityMenuItemRole;
    roleMap[QAccessible::MenuBar] = NSAccessibilityMenuBarRole;
    roleMap[QAccessible::ScrollBar] = NSAccessibilityScrollBarRole;
    roleMap[QAccessible::Grip] = NSAccessibilityGrowAreaRole;
    roleMap[QAccessible::Window] = NSAccessibilityWindowRole;
    roleMap[QAccessible::Dialog] = NSAccessibilityWindowRole;
    roleMap[QAccessible::AlertMessage] = NSAccessibilityWindowRole;
    roleMap[QAccessible::ToolTip] = NSAccessibilityWindowRole;
    roleMap[QAccessible::HelpBalloon] = NSAccessibilityWindowRole;
    roleMap[QAccessible::PopupMenu] = NSAccessibilityMenuRole;
    roleMap[QAccessible::Application] = NSAccessibilityApplicationRole;
    roleMap[QAccessible::Pane] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Grouping] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Separator] = NSAccessibilitySplitterRole;
    roleMap[QAccessible::ToolBar] = NSAccessibilityToolbarRole;
    roleMap[QAccessible::PageTab] = NSAccessibilityRadioButtonRole;
    roleMap[QAccessible::ButtonMenu] = NSAccessibilityMenuButtonRole;
    roleMap[QAccessible::ButtonDropDown] = NSAccessibilityPopUpButtonRole;
    roleMap[QAccessible::SpinBox] = NSAccessibilityIncrementorRole;
    roleMap[QAccessible::Slider] = NSAccessibilitySliderRole;
    roleMap[QAccessible::ProgressBar] = NSAccessibilityProgressIndicatorRole;
    roleMap[QAccessible::ComboBox] = NSAccessibilityPopUpButtonRole;
    roleMap[QAccessible::RadioButton] = NSAccessibilityRadioButtonRole;
    roleMap[QAccessible::CheckBox] = NSAccessibilityCheckBoxRole;
    roleMap[QAccessible::StaticText] = NSAccessibilityStaticTextRole;
    roleMap[QAccessible::Table] = NSAccessibilityTableRole;
    roleMap[QAccessible::StatusBar] = NSAccessibilityStaticTextRole;
    roleMap[QAccessible::Column] = NSAccessibilityColumnRole;
    roleMap[QAccessible::ColumnHeader] = NSAccessibilityColumnRole;
    roleMap[QAccessible::Row] = NSAccessibilityRowRole;
    roleMap[QAccessible::RowHeader] = NSAccessibilityRowRole;
    roleMap[QAccessible::Cell] = NSAccessibilityTextFieldRole;
    roleMap[QAccessible::Button] = NSAccessibilityButtonRole;
    roleMap[QAccessible::EditableText] = NSAccessibilityTextFieldRole;
    roleMap[QAccessible::Link] = NSAccessibilityLinkRole;
    roleMap[QAccessible::Indicator] = NSAccessibilityValueIndicatorRole;
    roleMap[QAccessible::Splitter] = NSAccessibilitySplitGroupRole;
    roleMap[QAccessible::List] = NSAccessibilityListRole;
    roleMap[QAccessible::ListItem] = NSAccessibilityStaticTextRole;
    roleMap[QAccessible::Cell] = NSAccessibilityStaticTextRole;
    roleMap[QAccessible::Client] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Paragraph] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Section] = NSAccessibilityGroupRole;
    roleMap[QAccessible::WebDocument] = NSAccessibilityGroupRole;
    roleMap[QAccessible::ColorChooser] = NSAccessibilityColorWellRole;
    roleMap[QAccessible::Footer] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Form] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Heading] = @"AXHeading";
    roleMap[QAccessible::Note] = NSAccessibilityGroupRole;
    roleMap[QAccessible::ComplementaryContent] = NSAccessibilityGroupRole;
    roleMap[QAccessible::Graphic] = NSAccessibilityImageRole;
}

/*
    Returns a Cocoa accessibility role for the given interface, or
    NSAccessibilityUnknownRole if no role mapping is found.
*/
NSString *macRole(QAccessibleInterface *interface)
{
    QAccessible::Role qtRole = interface->role();
    QMacAccessibiltyRoleMap &roleMap = *qMacAccessibiltyRoleMap();

    if (roleMap.isEmpty())
        populateRoleMap();

    // MAC_ACCESSIBILTY_DEBUG() << "role for" << interface.object() << "interface role" << hex << qtRole;

    if (roleMap.contains(qtRole)) {
       // MAC_ACCESSIBILTY_DEBUG() << "return" <<  roleMap[qtRole];
        if (roleMap[qtRole] == NSAccessibilityTextFieldRole && interface->state().multiLine)
            return NSAccessibilityTextAreaRole;
        return roleMap[qtRole];
    }

    // Treat unknown Qt roles as generic group container items. Returning
    // NSAccessibilityUnknownRole is also possible but makes the screen
    // reader focus on the item instead of passing focus to child items.
    // MAC_ACCESSIBILTY_DEBUG() << "return NSAccessibilityGroupRole for unknown Qt role";
    return NSAccessibilityGroupRole;
}

/*
    Returns a Cocoa sub role for the given interface.
*/
NSString *macSubrole(QAccessibleInterface *interface)
{
    QAccessible::State s = interface->state();
    if (s.searchEdit)
        return NSAccessibilitySearchFieldSubrole;
    if (s.passwordEdit)
        return NSAccessibilitySecureTextFieldSubrole;
    return nil;
}

/*
    Cocoa accessibility supports ignoring elements, which means that
    the elements are still present in the accessibility tree but is
    not used by the screen reader.
*/
bool shouldBeIgnored(QAccessibleInterface *interface)
{
    // Cocoa accessibility does not have an attribute that corresponds to the Invisible/Offscreen
    // state. Ignore interfaces with those flags set.
    const QAccessible::State state = interface->state();
    if (state.invisible ||
        state.offscreen ||
        state.invalid)
        return true;

    // Some roles are not interesting. In particular, container roles should be
    // ignored in order to flatten the accessibility tree as seen by the user.
    const QAccessible::Role role = interface->role();
    if (role == QAccessible::Border ||      // QFrame
        role == QAccessible::Application || // We use the system-provided application element.
        role == QAccessible::ToolBar ||     // Access the tool buttons directly.
        role == QAccessible::Pane ||        // Scroll areas.
        role == QAccessible::Client)        // The default for QWidget.
        return true;

    NSString *mac_role = macRole(interface);
    if (mac_role == NSAccessibilityWindowRole || // We use the system-provided window elements.
        mac_role == NSAccessibilityUnknownRole)
        return true;

    // Client is a generic role returned by plain QWidgets or other
    // widgets that does not have separate QAccessible interface, such
    // as the TabWidget. Return false unless macRole gives the interface
    // a special role.
    if (role == QAccessible::Client && mac_role == NSAccessibilityUnknownRole)
        return true;

    if (QObject * const object = interface->object()) {
        const QString className = QLatin1String(object->metaObject()->className());

        // VoiceOver focusing on tool tips can be confusing. The contents of the
        // tool tip is available through the description attribute anyway, so
        // we disable accessibility for tool tips.
        if (className == QLatin1String("QTipLabel"))
            return true;
    }

    return false;
}

NSArray<QMacAccessibilityElement *> *unignoredChildren(QAccessibleInterface *interface)
{
    int numKids = interface->childCount();
    // qDebug() << "Children for: " << axid << iface << " are: " << numKids;

    NSMutableArray<QMacAccessibilityElement *> *kids = [NSMutableArray<QMacAccessibilityElement *> arrayWithCapacity:numKids];
    for (int i = 0; i < numKids; ++i) {
        QAccessibleInterface *child = interface->child(i);
        if (!child || !child->isValid() || child->state().invalid || child->state().invisible)
            continue;

        QAccessible::Id childId = QAccessible::uniqueId(child);
        //qDebug() << "    kid: " << childId << child;

        QMacAccessibilityElement *element = [QMacAccessibilityElement elementWithId: childId];
        if (element)
            [kids addObject: element];
        else
            qWarning("QCocoaAccessibility: invalid child");
    }
    return NSAccessibilityUnignoredChildren(kids);
}
/*
    Translates a predefined QAccessibleActionInterface action to a Mac action constant.
    Returns 0 if the Qt Action has no mac equivalent. Ownership of the NSString is
    not transferred.
*/
NSString *getTranslatedAction(const QString &qtAction)
{
    if (qtAction == QAccessibleActionInterface::pressAction())
        return NSAccessibilityPressAction;
    else if (qtAction == QAccessibleActionInterface::increaseAction())
        return NSAccessibilityIncrementAction;
    else if (qtAction == QAccessibleActionInterface::decreaseAction())
        return NSAccessibilityDecrementAction;
    else if (qtAction == QAccessibleActionInterface::showMenuAction())
        return NSAccessibilityShowMenuAction;
    else if (qtAction == QAccessibleActionInterface::setFocusAction()) // Not 100% sure on this one
        return NSAccessibilityRaiseAction;
    else if (qtAction == QAccessibleActionInterface::toggleAction())
        return NSAccessibilityPressAction;

    // Not translated:
    //
    // Qt:
    //     static const QString &checkAction();
    //     static const QString &uncheckAction();
    //
    // Cocoa:
    //      NSAccessibilityConfirmAction;
    //      NSAccessibilityPickAction;
    //      NSAccessibilityCancelAction;
    //      NSAccessibilityDeleteAction;

    return nil;
}


/*
    Translates between a Mac action constant and a QAccessibleActionInterface action
    Returns an empty QString if there is no Qt predefined equivalent.
*/
QString translateAction(NSString *nsAction, QAccessibleInterface *interface)
{
    if ([nsAction compare: NSAccessibilityPressAction] == NSOrderedSame) {
        if (interface->role() == QAccessible::CheckBox || interface->role() == QAccessible::RadioButton)
            return QAccessibleActionInterface::toggleAction();
        return QAccessibleActionInterface::pressAction();
    } else if ([nsAction compare: NSAccessibilityIncrementAction] == NSOrderedSame)
        return QAccessibleActionInterface::increaseAction();
    else if ([nsAction compare: NSAccessibilityDecrementAction] == NSOrderedSame)
        return QAccessibleActionInterface::decreaseAction();
    else if ([nsAction compare: NSAccessibilityShowMenuAction] == NSOrderedSame)
        return QAccessibleActionInterface::showMenuAction();
    else if ([nsAction compare: NSAccessibilityRaiseAction] == NSOrderedSame)
        return QAccessibleActionInterface::setFocusAction();

    // See getTranslatedAction for not matched translations.

    return QString();
}

bool hasValueAttribute(QAccessibleInterface *interface)
{
    Q_ASSERT(interface);
    const QAccessible::Role qtrole = interface->role();
    if (qtrole == QAccessible::EditableText
            || qtrole == QAccessible::StaticText
            || interface->valueInterface()
            || interface->state().checkable) {
        return true;
    }

    return false;
}

id getValueAttribute(QAccessibleInterface *interface)
{
    const QAccessible::Role qtrole = interface->role();
    if (qtrole == QAccessible::StaticText) {
        return interface->text(QAccessible::Name).toNSString();
    }
    if (qtrole == QAccessible::EditableText) {
        if (QAccessibleTextInterface *textInterface = interface->textInterface()) {

            int begin = 0;
            int end = textInterface->characterCount();
            QString text;
            if (interface->state().passwordEdit) {
                // return round password replacement chars
                text = QString(end, QChar(0x2022));
            } else {
                // VoiceOver will read out the entire text string at once when returning
                // text as a value. For large text edits the size of the returned string
                // needs to be limited and text range attributes need to be used instead.
                // NSTextEdit returns the first sentence as the value, Do the same here:
                // ### call to textAfterOffset hangs. Booo!
                //if (textInterface->characterCount() > 0)
                //    textInterface->textAfterOffset(0, QAccessible2::SentenceBoundary, &begin, &end);
                text = textInterface->text(begin, end);
            }
            return text.toNSString();
        }
    }

    if (QAccessibleValueInterface *valueInterface = interface->valueInterface()) {
        return valueInterface->currentValue().toString().toNSString();
    }

    if (interface->state().checkable) {
        return interface->state().checked ? @(1) : @(0);
    }

    return nil;
}

} // namespace QCocoaAccessible

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

