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

#include "qcocoakeymapper.h"

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>

QT_BEGIN_NAMESPACE

// QCocoaKeyMapper debug facilities
//#define DEBUG_KEY_BINDINGS
//#define DEBUG_KEY_BINDINGS_MODIFIERS
//#define DEBUG_KEY_MAPS

// Possible modifier states.
// NOTE: The order of these states match the order in updatePossibleKeyCodes()!
static const Qt::KeyboardModifiers ModsTbl[] = {
    Qt::NoModifier,                                             // 0
    Qt::ShiftModifier,                                          // 1
    Qt::ControlModifier,                                        // 2
    Qt::ControlModifier | Qt::ShiftModifier,                    // 3
    Qt::AltModifier,                                            // 4
    Qt::AltModifier | Qt::ShiftModifier,                        // 5
    Qt::AltModifier | Qt::ControlModifier,                      // 6
    Qt::AltModifier | Qt::ShiftModifier | Qt::ControlModifier,  // 7
    Qt::MetaModifier,                                           // 8
    Qt::MetaModifier | Qt::ShiftModifier,                       // 9
    Qt::MetaModifier | Qt::ControlModifier,                    // 10
    Qt::MetaModifier | Qt::ControlModifier | Qt::ShiftModifier,// 11
    Qt::MetaModifier | Qt::AltModifier,                        // 12
    Qt::MetaModifier | Qt::AltModifier | Qt::ShiftModifier,    // 13
    Qt::MetaModifier | Qt::AltModifier | Qt::ControlModifier,  // 14
    Qt::MetaModifier | Qt::AltModifier | Qt::ShiftModifier | Qt::ControlModifier,  // 15
};

bool qt_mac_eat_unicode_key = false;


/* key maps */
struct qt_mac_enum_mapper
{
    int mac_code;
    int qt_code;
#if defined(DEBUG_KEY_BINDINGS)
#   define QT_MAC_MAP_ENUM(x) x, #x
    const char *desc;
#else
#   define QT_MAC_MAP_ENUM(x) x
#endif
};

//modifiers
static qt_mac_enum_mapper qt_mac_modifier_symbols[] = {
    { shiftKey, QT_MAC_MAP_ENUM(Qt::ShiftModifier) },
    { rightShiftKey, QT_MAC_MAP_ENUM(Qt::ShiftModifier) },
    { controlKey, QT_MAC_MAP_ENUM(Qt::MetaModifier) },
    { rightControlKey, QT_MAC_MAP_ENUM(Qt::MetaModifier) },
    { cmdKey, QT_MAC_MAP_ENUM(Qt::ControlModifier) },
    { optionKey, QT_MAC_MAP_ENUM(Qt::AltModifier) },
    { rightOptionKey, QT_MAC_MAP_ENUM(Qt::AltModifier) },
    { kEventKeyModifierNumLockMask, QT_MAC_MAP_ENUM(Qt::KeypadModifier) },
    { 0, QT_MAC_MAP_ENUM(0) }
};
Qt::KeyboardModifiers qt_mac_get_modifiers(int keys)
{
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
    qDebug("Qt: internal: **Mapping modifiers: %d (0x%04x)", keys, keys);
#endif
    Qt::KeyboardModifiers ret = Qt::NoModifier;
    for (int i = 0; qt_mac_modifier_symbols[i].qt_code; i++) {
        if (keys & qt_mac_modifier_symbols[i].mac_code) {
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
            qDebug("Qt: internal: got modifier: %s", qt_mac_modifier_symbols[i].desc);
#endif
            ret |= Qt::KeyboardModifier(qt_mac_modifier_symbols[i].qt_code);
        }
    }
    if (qApp->testAttribute(Qt::AA_MacDontSwapCtrlAndMeta)) {
        Qt::KeyboardModifiers oldModifiers = ret;
        ret &= ~(Qt::MetaModifier | Qt::ControlModifier);
        if (oldModifiers & Qt::ControlModifier)
            ret |= Qt::MetaModifier;
        if (oldModifiers & Qt::MetaModifier)
            ret |= Qt::ControlModifier;
    }
    return ret;
}
static int qt_mac_get_mac_modifiers(Qt::KeyboardModifiers keys)
{
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
    qDebug("Qt: internal: **Mapping modifiers: %d (0x%04x)", (int)keys, (int)keys);
#endif
    int ret = 0;
    for (int i = 0; qt_mac_modifier_symbols[i].qt_code; i++) {
        if (keys & qt_mac_modifier_symbols[i].qt_code) {
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
            qDebug("Qt: internal: got modifier: %s", qt_mac_modifier_symbols[i].desc);
#endif
            ret |= qt_mac_modifier_symbols[i].mac_code;
        }
    }

    if (qApp->testAttribute(Qt::AA_MacDontSwapCtrlAndMeta)) {
        int oldModifiers = ret;
        ret &= ~(controlKeyBit | cmdKeyBit);
        if (oldModifiers & controlKeyBit)
            ret |= cmdKeyBit;
        if (oldModifiers & cmdKeyBit)
            ret |= controlKeyBit;
    }
    return ret;
}

//keyboard keys (non-modifiers)
static qt_mac_enum_mapper qt_mac_keyboard_symbols[] = {
    { kHomeCharCode, QT_MAC_MAP_ENUM(Qt::Key_Home) },
    { kEnterCharCode, QT_MAC_MAP_ENUM(Qt::Key_Enter) },
    { kEndCharCode, QT_MAC_MAP_ENUM(Qt::Key_End) },
    { kBackspaceCharCode, QT_MAC_MAP_ENUM(Qt::Key_Backspace) },
    { kTabCharCode, QT_MAC_MAP_ENUM(Qt::Key_Tab) },
    { kPageUpCharCode, QT_MAC_MAP_ENUM(Qt::Key_PageUp) },
    { kPageDownCharCode, QT_MAC_MAP_ENUM(Qt::Key_PageDown) },
    { kReturnCharCode, QT_MAC_MAP_ENUM(Qt::Key_Return) },
    { kEscapeCharCode, QT_MAC_MAP_ENUM(Qt::Key_Escape) },
    { kLeftArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Left) },
    { kRightArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Right) },
    { kUpArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Up) },
    { kDownArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Down) },
    { kHelpCharCode, QT_MAC_MAP_ENUM(Qt::Key_Help) },
    { kDeleteCharCode, QT_MAC_MAP_ENUM(Qt::Key_Delete) },
//ascii maps, for debug
    { ':', QT_MAC_MAP_ENUM(Qt::Key_Colon) },
    { ';', QT_MAC_MAP_ENUM(Qt::Key_Semicolon) },
    { '<', QT_MAC_MAP_ENUM(Qt::Key_Less) },
    { '=', QT_MAC_MAP_ENUM(Qt::Key_Equal) },
    { '>', QT_MAC_MAP_ENUM(Qt::Key_Greater) },
    { '?', QT_MAC_MAP_ENUM(Qt::Key_Question) },
    { '@', QT_MAC_MAP_ENUM(Qt::Key_At) },
    { ' ', QT_MAC_MAP_ENUM(Qt::Key_Space) },
    { '!', QT_MAC_MAP_ENUM(Qt::Key_Exclam) },
    { '"', QT_MAC_MAP_ENUM(Qt::Key_QuoteDbl) },
    { '#', QT_MAC_MAP_ENUM(Qt::Key_NumberSign) },
    { '$', QT_MAC_MAP_ENUM(Qt::Key_Dollar) },
    { '%', QT_MAC_MAP_ENUM(Qt::Key_Percent) },
    { '&', QT_MAC_MAP_ENUM(Qt::Key_Ampersand) },
    { '\'', QT_MAC_MAP_ENUM(Qt::Key_Apostrophe) },
    { '(', QT_MAC_MAP_ENUM(Qt::Key_ParenLeft) },
    { ')', QT_MAC_MAP_ENUM(Qt::Key_ParenRight) },
    { '*', QT_MAC_MAP_ENUM(Qt::Key_Asterisk) },
    { '+', QT_MAC_MAP_ENUM(Qt::Key_Plus) },
    { ',', QT_MAC_MAP_ENUM(Qt::Key_Comma) },
    { '-', QT_MAC_MAP_ENUM(Qt::Key_Minus) },
    { '.', QT_MAC_MAP_ENUM(Qt::Key_Period) },
    { '/', QT_MAC_MAP_ENUM(Qt::Key_Slash) },
    { '[', QT_MAC_MAP_ENUM(Qt::Key_BracketLeft) },
    { ']', QT_MAC_MAP_ENUM(Qt::Key_BracketRight) },
    { '\\', QT_MAC_MAP_ENUM(Qt::Key_Backslash) },
    { '_', QT_MAC_MAP_ENUM(Qt::Key_Underscore) },
    { '`', QT_MAC_MAP_ENUM(Qt::Key_QuoteLeft) },
    { '{', QT_MAC_MAP_ENUM(Qt::Key_BraceLeft) },
    { '}', QT_MAC_MAP_ENUM(Qt::Key_BraceRight) },
    { '|', QT_MAC_MAP_ENUM(Qt::Key_Bar) },
    { '~', QT_MAC_MAP_ENUM(Qt::Key_AsciiTilde) },
    { '^', QT_MAC_MAP_ENUM(Qt::Key_AsciiCircum) },
    {   0, QT_MAC_MAP_ENUM(0) }
};

static qt_mac_enum_mapper qt_mac_keyvkey_symbols[] = { //real scan codes
    { kVK_F1, QT_MAC_MAP_ENUM(Qt::Key_F1) },
    { kVK_F2, QT_MAC_MAP_ENUM(Qt::Key_F2) },
    { kVK_F3, QT_MAC_MAP_ENUM(Qt::Key_F3) },
    { kVK_F4, QT_MAC_MAP_ENUM(Qt::Key_F4) },
    { kVK_F5, QT_MAC_MAP_ENUM(Qt::Key_F5) },
    { kVK_F6, QT_MAC_MAP_ENUM(Qt::Key_F6) },
    { kVK_F7, QT_MAC_MAP_ENUM(Qt::Key_F7) },
    { kVK_F8, QT_MAC_MAP_ENUM(Qt::Key_F8) },
    { kVK_F9, QT_MAC_MAP_ENUM(Qt::Key_F9) },
    { kVK_F10, QT_MAC_MAP_ENUM(Qt::Key_F10) },
    { kVK_F11, QT_MAC_MAP_ENUM(Qt::Key_F11) },
    { kVK_F12, QT_MAC_MAP_ENUM(Qt::Key_F12) },
    { kVK_F13, QT_MAC_MAP_ENUM(Qt::Key_F13) },
    { kVK_F14, QT_MAC_MAP_ENUM(Qt::Key_F14) },
    { kVK_F15, QT_MAC_MAP_ENUM(Qt::Key_F15) },
    { kVK_F16, QT_MAC_MAP_ENUM(Qt::Key_F16) },
    { kVK_Return, QT_MAC_MAP_ENUM(Qt::Key_Return) },
    { kVK_Tab, QT_MAC_MAP_ENUM(Qt::Key_Tab) },
    { kVK_Escape, QT_MAC_MAP_ENUM(Qt::Key_Escape) },
    { kVK_Help, QT_MAC_MAP_ENUM(Qt::Key_Help) },
    { kVK_UpArrow, QT_MAC_MAP_ENUM(Qt::Key_Up) },
    { kVK_DownArrow, QT_MAC_MAP_ENUM(Qt::Key_Down) },
    { kVK_LeftArrow, QT_MAC_MAP_ENUM(Qt::Key_Left) },
    { kVK_RightArrow, QT_MAC_MAP_ENUM(Qt::Key_Right) },
    { kVK_PageUp, QT_MAC_MAP_ENUM(Qt::Key_PageUp) },
    { kVK_PageDown, QT_MAC_MAP_ENUM(Qt::Key_PageDown) },
    {   0, QT_MAC_MAP_ENUM(0) }
};

static qt_mac_enum_mapper qt_mac_private_unicode[] = {
    { 0xF700, QT_MAC_MAP_ENUM(Qt::Key_Up) },            //NSUpArrowFunctionKey
    { 0xF701, QT_MAC_MAP_ENUM(Qt::Key_Down) },          //NSDownArrowFunctionKey
    { 0xF702, QT_MAC_MAP_ENUM(Qt::Key_Left) },          //NSLeftArrowFunctionKey
    { 0xF703, QT_MAC_MAP_ENUM(Qt::Key_Right) },         //NSRightArrowFunctionKey
    { 0xF727, QT_MAC_MAP_ENUM(Qt::Key_Insert) },        //NSInsertFunctionKey
    { 0xF728, QT_MAC_MAP_ENUM(Qt::Key_Delete) },        //NSDeleteFunctionKey
    { 0xF729, QT_MAC_MAP_ENUM(Qt::Key_Home) },          //NSHomeFunctionKey
    { 0xF72B, QT_MAC_MAP_ENUM(Qt::Key_End) },           //NSEndFunctionKey
    { 0xF72C, QT_MAC_MAP_ENUM(Qt::Key_PageUp) },        //NSPageUpFunctionKey
    { 0xF72D, QT_MAC_MAP_ENUM(Qt::Key_PageDown) },      //NSPageDownFunctionKey
    { 0xF72E, QT_MAC_MAP_ENUM(Qt::Key_Print) },         //NSPrintScreenFunctionKey
    { 0xF72F, QT_MAC_MAP_ENUM(Qt::Key_ScrollLock) },    //NSScrollLockFunctionKey
    { 0xF730, QT_MAC_MAP_ENUM(Qt::Key_Pause) },         //NSPauseFunctionKey
    { 0xF731, QT_MAC_MAP_ENUM(Qt::Key_SysReq) },        //NSSysReqFunctionKey
    { 0xF735, QT_MAC_MAP_ENUM(Qt::Key_Menu) },          //NSMenuFunctionKey
    { 0xF738, QT_MAC_MAP_ENUM(Qt::Key_Printer) },       //NSPrintFunctionKey
    { 0xF73A, QT_MAC_MAP_ENUM(Qt::Key_Clear) },         //NSClearDisplayFunctionKey
    { 0xF73D, QT_MAC_MAP_ENUM(Qt::Key_Insert) },        //NSInsertCharFunctionKey
    { 0xF73E, QT_MAC_MAP_ENUM(Qt::Key_Delete) },        //NSDeleteCharFunctionKey
    { 0xF741, QT_MAC_MAP_ENUM(Qt::Key_Select) },        //NSSelectFunctionKey
    { 0xF742, QT_MAC_MAP_ENUM(Qt::Key_Execute) },       //NSExecuteFunctionKey
    { 0xF743, QT_MAC_MAP_ENUM(Qt::Key_Undo) },          //NSUndoFunctionKey
    { 0xF744, QT_MAC_MAP_ENUM(Qt::Key_Redo) },          //NSRedoFunctionKey
    { 0xF745, QT_MAC_MAP_ENUM(Qt::Key_Find) },          //NSFindFunctionKey
    { 0xF746, QT_MAC_MAP_ENUM(Qt::Key_Help) },          //NSHelpFunctionKey
    { 0xF747, QT_MAC_MAP_ENUM(Qt::Key_Mode_switch) },   //NSModeSwitchFunctionKey
    {   0,    QT_MAC_MAP_ENUM(0) }
};

static int qt_mac_get_key(int modif, const QChar &key, int virtualKey)
{
#ifdef DEBUG_KEY_BINDINGS
    qDebug("**Mapping key: %d (0x%04x) - %d (0x%04x)", key.unicode(), key.unicode(), virtualKey, virtualKey);
#endif

    if (key == kClearCharCode && virtualKey == 0x47)
        return Qt::Key_Clear;

    if (key.isDigit()) {
#ifdef DEBUG_KEY_BINDINGS
            qDebug("%d: got key: %d", __LINE__, key.digitValue());
#endif
        return key.digitValue() + Qt::Key_0;
    }

    if (key.isLetter()) {
#ifdef DEBUG_KEY_BINDINGS
        qDebug("%d: got key: %d", __LINE__, (key.toUpper().unicode() - 'A'));
#endif
        return (key.toUpper().unicode() - 'A') + Qt::Key_A;
    }
    if (key.isSymbol()) {
#ifdef DEBUG_KEY_BINDINGS
        qDebug("%d: got key: %d", __LINE__, (key.unicode()));
#endif
        return key.unicode();
    }

    for (int i = 0; qt_mac_keyboard_symbols[i].qt_code; i++) {
        if (qt_mac_keyboard_symbols[i].mac_code == key) {
            /* To work like Qt for X11 we issue Backtab when Shift + Tab are pressed */
            if (qt_mac_keyboard_symbols[i].qt_code == Qt::Key_Tab && (modif & Qt::ShiftModifier)) {
#ifdef DEBUG_KEY_BINDINGS
                qDebug("%d: got key: Qt::Key_Backtab", __LINE__);
#endif
                return Qt::Key_Backtab;
            }

#ifdef DEBUG_KEY_BINDINGS
            qDebug("%d: got key: %s", __LINE__, qt_mac_keyboard_symbols[i].desc);
#endif
            return qt_mac_keyboard_symbols[i].qt_code;
        }
    }

    //last ditch try to match the scan code
    for (int i = 0; qt_mac_keyvkey_symbols[i].qt_code; i++) {
        if (qt_mac_keyvkey_symbols[i].mac_code == virtualKey) {
#ifdef DEBUG_KEY_BINDINGS
            qDebug("%d: got key: %s", __LINE__, qt_mac_keyvkey_symbols[i].desc);
#endif
            return qt_mac_keyvkey_symbols[i].qt_code;
        }
    }

    // check if they belong to key codes in private unicode range
    if (key >= 0xf700 && key <= 0xf747) {
        if (key >= 0xf704 && key <= 0xf726) {
            return Qt::Key_F1 + (key.unicode() - 0xf704) ;
        }
        for (int i = 0; qt_mac_private_unicode[i].qt_code; i++) {
            if (qt_mac_private_unicode[i].mac_code == key) {
                return qt_mac_private_unicode[i].qt_code;
            }
        }

    }

    //oh well
#ifdef DEBUG_KEY_BINDINGS
    qDebug("Unknown case.. %s:%d %d[%d] %d", __FILE__, __LINE__, key.unicode(), key.toLatin1(), virtualKey);
#endif
    return Qt::Key_unknown;
}

QCocoaKeyMapper::QCocoaKeyMapper()
{
    memset(keyLayout, 0, sizeof(keyLayout));
}

QCocoaKeyMapper::~QCocoaKeyMapper()
{
    deleteLayouts();
}

Qt::KeyboardModifiers QCocoaKeyMapper::queryKeyboardModifiers()
{
    return qt_mac_get_modifiers(GetCurrentKeyModifiers());
}

bool QCocoaKeyMapper::updateKeyboard()
{
    const UCKeyboardLayout *uchrData = nullptr;
    QCFType<TISInputSourceRef> source = TISCopyInputMethodKeyboardLayoutOverride();
    if (!source)
        source = TISCopyCurrentKeyboardInputSource();
    if (keyboard_mode != NullMode && source == currentInputSource) {
        return false;
    }
    Q_ASSERT(source);
    CFDataRef data = static_cast<CFDataRef>(TISGetInputSourceProperty(source,
                                                                 kTISPropertyUnicodeKeyLayoutData));
    uchrData = data ? reinterpret_cast<const UCKeyboardLayout *>(CFDataGetBytePtr(data)) : nullptr;

    keyboard_kind = LMGetKbdType();
    if (uchrData) {
        keyboard_layout_format = uchrData;
        keyboard_mode = UnicodeMode;
    } else {
        keyboard_layout_format = nullptr;
        keyboard_mode = NullMode;
    }
    currentInputSource = source;
    keyboard_dead = 0;

    const auto newMode = keyboard_mode;
    deleteLayouts();
    keyboard_mode = newMode;

    return true;
}

void QCocoaKeyMapper::deleteLayouts()
{
    keyboard_mode = NullMode;
    for (int i = 0; i < 255; ++i) {
        if (keyLayout[i]) {
            delete keyLayout[i];
            keyLayout[i] = nullptr;
        }
    }
}

void QCocoaKeyMapper::clearMappings()
{
    deleteLayouts();
    updateKeyboard();
}

void QCocoaKeyMapper::updateKeyMap(unsigned short macVirtualKey, QChar unicodeKey)
{
    updateKeyboard();

    if (keyLayout[macVirtualKey])
        return;

    UniCharCount buffer_size = 10;
    UniChar buffer[buffer_size];
    keyLayout[macVirtualKey] = new KeyboardLayoutItem;
    for (int i = 0; i < 16; ++i) {
        UniCharCount out_buffer_size = 0;
        keyLayout[macVirtualKey]->qtKey[i] = 0;

        const UInt32 keyModifier = ((qt_mac_get_mac_modifiers(ModsTbl[i]) >> 8) & 0xFF);
        OSStatus err = UCKeyTranslate(keyboard_layout_format, macVirtualKey, kUCKeyActionDown, keyModifier,
                                      keyboard_kind, 0, &keyboard_dead, buffer_size, &out_buffer_size, buffer);
        if (err == noErr && out_buffer_size) {
            const QChar unicode(buffer[0]);
            int qtkey = qt_mac_get_key(keyModifier, unicode, macVirtualKey);
            if (qtkey == Qt::Key_unknown)
                qtkey = unicode.unicode();
            keyLayout[macVirtualKey]->qtKey[i] = qtkey;
        } else {
            int qtkey = qt_mac_get_key(keyModifier, unicodeKey, macVirtualKey);
            if (qtkey == Qt::Key_unknown)
                qtkey = unicodeKey.unicode();
            keyLayout[macVirtualKey]->qtKey[i] = qtkey;
        }
    }
#ifdef DEBUG_KEY_MAPS
    qDebug("updateKeyMap for virtual key = 0x%02x!", (uint)macVirtualKey);
    for (int i = 0; i < 16; ++i) {
        qDebug("    [%d] (%d,0x%02x,'%c')", i,
               keyLayout[macVirtualKey]->qtKey[i],
               keyLayout[macVirtualKey]->qtKey[i],
               keyLayout[macVirtualKey]->qtKey[i]);
    }
#endif
}

QList<int> QCocoaKeyMapper::possibleKeys(const QKeyEvent *event) const
{
    QList<int> ret;
    const_cast<QCocoaKeyMapper *>(this)->updateKeyMap(event->nativeVirtualKey(), QChar(event->key()));

    KeyboardLayoutItem *kbItem = keyLayout[event->nativeVirtualKey()];

    if (!kbItem) // Key is not in any keyboard layout (e.g. eisu-key on Japanese keyboard)
        return ret;

    int baseKey = kbItem->qtKey[0];
    Qt::KeyboardModifiers keyMods = event->modifiers();

    ret << int(baseKey + keyMods); // The base key is _always_ valid, of course

    for (int i = 1; i < 8; ++i) {
        Qt::KeyboardModifiers neededMods = ModsTbl[i];
        int key = kbItem->qtKey[i];
        if (key && key != baseKey && ((keyMods & neededMods) == neededMods)) {
            ret << int(key + (keyMods & ~neededMods));
        }
    }
    return ret;
}

QT_END_NAMESPACE
