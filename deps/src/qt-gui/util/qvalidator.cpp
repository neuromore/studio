/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Giuseppe D'Angelo <giuseppe.dangelo@kdab.com>
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

#include <qdebug.h>

#include "qvalidator.h"
#ifndef QT_NO_VALIDATOR
#include "private/qobject_p.h"
#include "private/qlocale_p.h"
#include "private/qnumeric_p.h"

#include <limits.h>
#include <cmath>

QT_BEGIN_NAMESPACE

/*!
    \class QValidator
    \brief The QValidator class provides validation of input text.
    \inmodule QtGui

    The class itself is abstract. Two subclasses, \l QIntValidator and
    \l QDoubleValidator, provide basic numeric-range checking, and \l
    QRegExpValidator provides general checking using a custom regular
    expression.

    If the built-in validators aren't sufficient, you can subclass
    QValidator. The class has two virtual functions: validate() and
    fixup().

    \l validate() must be implemented by every subclass. It returns
    \l Invalid, \l Intermediate or \l Acceptable depending on whether
    its argument is valid (for the subclass's definition of valid).

    These three states require some explanation. An \l Invalid string
    is \e clearly invalid. \l Intermediate is less obvious: the
    concept of validity is difficult to apply when the string is
    incomplete (still being edited). QValidator defines \l Intermediate
    as the property of a string that is neither clearly invalid nor
    acceptable as a final result. \l Acceptable means that the string
    is acceptable as a final result. One might say that any string
    that is a plausible intermediate state during entry of an \l
    Acceptable string is \l Intermediate.

    Here are some examples:

    \list

    \li For a line edit that accepts integers from 10 to 1000 inclusive,
    42 and 123 are \l Acceptable, the empty string and 5 are \l
    Intermediate, and "asdf" and 1114 is \l Invalid.

    \li For an editable combobox that accepts URLs, any well-formed URL
    is \l Acceptable, "http://example.com/," is \l Intermediate
    (it might be a cut and paste action that accidentally took in a
    comma at the end), the empty string is \l Intermediate (the user
    might select and delete all of the text in preparation for entering
    a new URL) and "http:///./" is \l Invalid.

    \li For a spin box that accepts lengths, "11cm" and "1in" are \l
    Acceptable, "11" and the empty string are \l Intermediate, and
    "http://example.com" and "hour" are \l Invalid.

    \endlist

    \l fixup() is provided for validators that can repair some user
    errors. The default implementation does nothing. QLineEdit, for
    example, will call fixup() if the user presses Enter (or Return)
    and the content is not currently valid. This allows the fixup()
    function the opportunity of performing some magic to make an \l
    Invalid string \l Acceptable.

    A validator has a locale, set with setLocale(). It is typically used
    to parse localized data. For example, QIntValidator and QDoubleValidator
    use it to parse localized representations of integers and doubles.

    QValidator is typically used with QLineEdit, QSpinBox and
    QComboBox.

    \sa QIntValidator, QDoubleValidator, QRegExpValidator, {Line Edits Example}
*/


/*!
    \enum QValidator::State

    This enum type defines the states in which a validated string can
    exist.

    \value Invalid       The string is \e clearly invalid.
    \value Intermediate  The string is a plausible intermediate value.
    \value Acceptable    The string is acceptable as a final result;
                         i.e. it is valid.
*/

/*!
    \fn void QValidator::changed()

    This signal is emitted when any property that may affect the validity of
    a string has changed.
*/

/*!
    \fn void QIntValidator::topChanged(int top)

    This signal is emitted after the top property changed.

    \sa QIntValidator::top(), QIntValidator::setTop(), QIntValidator::bottom(), QIntValidator::setBottom()
    \internal
*/

/*!
    \fn void QIntValidator::bottomChanged(int bottom)

    This signal is emitted after the bottom property changed.

    \sa QIntValidator::top(), QIntValidator::setTop(), QIntValidator::bottom(), QIntValidator::setBottom()
    \internal
*/

/*!
    \fn void QDoubleValidator::topChanged(double top)

    This signal is emitted after the top property changed.

    \sa QDoubleValidator::top(), QDoubleValidator::setTop(), QDoubleValidator::bottom(), QDoubleValidator::setBottom()
    \internal
*/

/*!
    \fn void QDoubleValidator::bottomChanged(double bottom)

    This signal is emitted after the bottom property changed.

    \sa QDoubleValidator::top(), QDoubleValidator::setTop(), QDoubleValidator::bottom(), QDoubleValidator::setBottom()
    \internal
*/

/*!
    \fn void QDoubleValidator::decimalsChanged(int decimals)

    This signal is emitted after the decimals property changed.

    \internal
*/

/*!
    \fn void QDoubleValidator::notationChanged(QDoubleValidator::Notation notation)

    This signal is emitted after the notation property changed.

    QDoubleValidator::Notation is not a registered metatype, so for queued connections,
    you will have to register it with Q_DECLARE_METATYPE() and qRegisterMetaType().

    \internal
*/

/*!
    \fn void QRegExpValidator::regExpChanged(const QRegExp &regExp)

    This signal is emitted after the regExp property changed.
    \internal
*/

class QValidatorPrivate : public QObjectPrivate{
    Q_DECLARE_PUBLIC(QValidator)
public:
    QValidatorPrivate() : QObjectPrivate()
    {
    }

    QLocale locale;
};


/*!
    Sets up the validator. The \a parent parameter is
    passed on to the QObject constructor.
*/

QValidator::QValidator(QObject * parent)
    : QValidator(*new QValidatorPrivate, parent)
{
}

/*!
    Destroys the validator, freeing any storage and other resources
    used.
*/

QValidator::~QValidator()
{
}

/*!
    Returns the locale for the validator. The locale is by default initialized to the same as QLocale().

    \sa setLocale()
    \sa QLocale::QLocale()
*/
QLocale QValidator::locale() const
{
    Q_D(const QValidator);
    return d->locale;
}

/*!
    Sets the \a locale that will be used for the validator. Unless
    setLocale has been called, the validator will use the default
    locale set with QLocale::setDefault(). If a default locale has not
    been set, it is the operating system's locale.

    \sa locale(), QLocale::setDefault()
*/
void QValidator::setLocale(const QLocale &locale)
{
    Q_D(QValidator);
    if (d->locale != locale) {
        d->locale = locale;
        emit changed();
    }
}

/*!
    \fn QValidator::State QValidator::validate(QString &input, int &pos) const

    This virtual function returns \l Invalid if \a input is invalid
    according to this validator's rules, \l Intermediate if it
    is likely that a little more editing will make the input
    acceptable (e.g. the user types "4" into a widget which accepts
    integers between 10 and 99), and \l Acceptable if the input is
    valid.

    The function can change both \a input and \a pos (the cursor position)
    if required.
*/


/*!
    \fn void QValidator::fixup(QString & input) const

    This function attempts to change \a input to be valid according to
    this validator's rules. It need not result in a valid string:
    callers of this function must re-test afterwards; the default does
    nothing.

    Reimplementations of this function can change \a input even if
    they do not produce a valid string. For example, an ISBN validator
    might want to delete every character except digits and "-", even
    if the result is still not a valid ISBN; a surname validator might
    want to remove whitespace from the start and end of the string,
    even if the resulting string is not in the list of accepted
    surnames.
*/

void QValidator::fixup(QString &) const
{
}


/*!
    \class QIntValidator
    \brief The QIntValidator class provides a validator that ensures
    a string contains a valid integer within a specified range.
    \inmodule QtGui

    Example of use:

    \snippet code/src_gui_util_qvalidator.cpp 0

    Below we present some examples of validators. In practice they would
    normally be associated with a widget as in the example above.

    \snippet code/src_gui_util_qvalidator.cpp 1

    Notice that the value \c 999 returns Intermediate. Values
    consisting of a number of digits equal to or less than the max
    value are considered intermediate. This is intended because the
    digit that prevents a number from being in range is not necessarily the
    last digit typed. This also means that an intermediate number can
    have leading zeros.

    The minimum and maximum values are set in one call with setRange(),
    or individually with setBottom() and setTop().

    QIntValidator uses its locale() to interpret the number. For example,
    in Arabic locales, QIntValidator will accept Arabic digits.

    \note The QLocale::NumberOptions set on the locale() also affect the
    way the number is interpreted. For example, since QLocale::RejectGroupSeparator
    is not set by default, the validator will accept group separators. It is thus
    recommended to use QLocale::toInt() to obtain the numeric value.

    \sa QDoubleValidator, QRegExpValidator, QLocale::toInt(), {Line Edits Example}
*/

/*!
    Constructs a validator with a \a parent object that
    accepts all integers.
*/

QIntValidator::QIntValidator(QObject * parent)
    : QIntValidator(INT_MIN, INT_MAX, parent)
{
}


/*!
    Constructs a validator with a \a parent, that accepts integers
    from \a minimum to \a maximum inclusive.
*/

QIntValidator::QIntValidator(int minimum, int maximum,
                              QObject * parent)
    : QValidator(parent)
{
    b = minimum;
    t = maximum;
}


/*!
    Destroys the validator.
*/

QIntValidator::~QIntValidator()
{
    // nothing
}


/*!
    \fn QValidator::State QIntValidator::validate(QString &input, int &pos) const

    Returns \l Acceptable if the \a input is an integer within the
    valid range, \l Intermediate if the \a input is a prefix of an integer in the
    valid range, and \l Invalid otherwise.

    If the valid range consists of just positive integers (e.g., 32 to 100)
    and \a input is a negative integer, then Invalid is returned. (On the other
    hand, if the range consists of negative integers (e.g., -100 to -32) and
    \a input is a positive integer, then Intermediate is returned, because
    the user might be just about to type the minus (especially for right-to-left
    languages).

    \snippet code/src_gui_util_qvalidator.cpp 2

    By default, the \a pos parameter is not used by this validator.
*/

static int numDigits(qlonglong n)
{
    if (n == 0)
        return 1;
    return (int)std::log10(double(n)) + 1;
}

static qlonglong pow10(int exp)
{
    qlonglong result = 1;
    for (int i = 0; i < exp; ++i)
        result *= 10;
    return result;
}

QValidator::State QIntValidator::validate(QString & input, int&) const
{
    QByteArray buff;
    if (!locale().d->m_data->validateChars(input, QLocaleData::IntegerMode, &buff, -1,
                                           locale().numberOptions())) {
        return Invalid;
    }

    if (buff.isEmpty())
        return Intermediate;

    const bool startsWithMinus(buff[0] == '-');
    if (b >= 0 && startsWithMinus)
        return Invalid;

    const bool startsWithPlus(buff[0] == '+');
    if (t < 0 && startsWithPlus)
        return Invalid;

    if (buff.size() == 1 && (startsWithPlus || startsWithMinus))
        return Intermediate;

    bool ok;
    qlonglong entered = QLocaleData::bytearrayToLongLong(buff.constData(), 10, &ok);
    if (!ok)
        return Invalid;

    if (entered >= b && entered <= t) {
        locale().toInt(input, &ok);
        return ok ? Acceptable : Intermediate;
    }

    if (entered >= 0) {
        // the -entered < b condition is necessary to allow people to type
        // the minus last (e.g. for right-to-left languages)
        // The buffLength > tLength condition validates values consisting
        // of a number of digits equal to or less than the max value as intermediate.

        int buffLength = buff.size();
        if (startsWithPlus)
            buffLength--;
        const int tLength = t != 0 ? static_cast<int>(std::log10(qAbs(t))) + 1 : 1;

        return (entered > t && -entered < b && buffLength > tLength) ? Invalid : Intermediate;
    } else {
        return (entered < b) ? Invalid : Intermediate;
    }
}

/*! \reimp */
void QIntValidator::fixup(QString &input) const
{
    QByteArray buff;
    if (!locale().d->m_data->validateChars(input, QLocaleData::IntegerMode, &buff, -1,
                                           locale().numberOptions())) {
        return;
    }
    bool ok;
    qlonglong entered = QLocaleData::bytearrayToLongLong(buff.constData(), 10, &ok);
    if (ok)
        input = locale().toString(entered);
}

// FIXME: Qt 6: Make QIntValidator::setRange() non-virtual

/*!
    Sets the range of the validator to only accept integers between \a
    bottom and \a top inclusive.
*/

void QIntValidator::setRange(int bottom, int top)
{
    bool rangeChanged = false;
    if (b != bottom) {
        b = bottom;
        rangeChanged = true;
        emit bottomChanged(b);
    }

    if (t != top) {
        t = top;
        rangeChanged = true;
        emit topChanged(t);
    }

    if (rangeChanged)
        emit changed();
}


/*!
    \property QIntValidator::bottom
    \brief the validator's lowest acceptable value

    By default, this property's value is derived from the lowest signed
    integer available (typically -2147483647).

    \sa setRange()
*/
void QIntValidator::setBottom(int bottom)
{
    setRange(bottom, top());
}

/*!
    \property QIntValidator::top
    \brief the validator's highest acceptable value

    By default, this property's value is derived from the highest signed
    integer available (typically 2147483647).

    \sa setRange()
*/
void QIntValidator::setTop(int top)
{
    setRange(bottom(), top);
}

/*!
    \internal
*/
QValidator::QValidator(QObjectPrivate &d, QObject *parent)
        : QObject(d, parent)
{
}

/*!
    \internal
*/
QValidator::QValidator(QValidatorPrivate &d, QObject *parent)
        : QObject(d, parent)
{
}

#ifndef QT_NO_REGEXP

class QDoubleValidatorPrivate : public QValidatorPrivate
{
    Q_DECLARE_PUBLIC(QDoubleValidator)
public:
    QDoubleValidatorPrivate()
        : QValidatorPrivate()
        , notation(QDoubleValidator::ScientificNotation)
    {
    }

    QDoubleValidator::Notation notation;

    QValidator::State validateWithLocale(QString & input, QLocaleData::NumberMode numMode, const QLocale &locale) const;
};


/*!
    \class QDoubleValidator

    \brief The QDoubleValidator class provides range checking of
    floating-point numbers.
    \inmodule QtGui

    QDoubleValidator provides an upper bound, a lower bound, and a
    limit on the number of digits after the decimal point. It does not
    provide a fixup() function.

    You can set the acceptable range in one call with setRange(), or
    with setBottom() and setTop(). Set the number of decimal places
    with setDecimals(). The validate() function returns the validation
    state.

    QDoubleValidator uses its locale() to interpret the number. For example,
    in the German locale, "1,234" will be accepted as the fractional number
    1.234. In Arabic locales, QDoubleValidator will accept Arabic digits.

    \note The QLocale::NumberOptions set on the locale() also affect the
    way the number is interpreted. For example, since QLocale::RejectGroupSeparator
    is not set by default, the validator will accept group separators. It is thus
    recommended to use QLocale::toDouble() to obtain the numeric value.

    \sa QIntValidator, QRegExpValidator, QLocale::toDouble(), {Line Edits Example}
*/

 /*!
    \enum QDoubleValidator::Notation
    \since 4.3
    This enum defines the allowed notations for entering a double.

    \value StandardNotation      The string is written as a standard number
                                 (i.e. 0.015).
    \value ScientificNotation    The string is written in scientific
                                 form. It may have an exponent part(i.e. 1.5E-2).
*/

/*!
    Constructs a validator object with a \a parent object
    that accepts any double.
*/

QDoubleValidator::QDoubleValidator(QObject * parent)
    : QDoubleValidator(-HUGE_VAL, HUGE_VAL, 1000, parent)
{
}


/*!
    Constructs a validator object with a \a parent object. This
    validator will accept doubles from \a bottom to \a top inclusive,
    with up to \a decimals digits after the decimal point.
*/

QDoubleValidator::QDoubleValidator(double bottom, double top, int decimals,
                                    QObject * parent)
    : QValidator(*new QDoubleValidatorPrivate , parent)
{
    b = bottom;
    t = top;
    dec = decimals;
}


/*!
    Destroys the validator.
*/

QDoubleValidator::~QDoubleValidator()
{
}


/*!
    \fn QValidator::State QDoubleValidator::validate(QString &input, int &pos) const

    Returns \l Acceptable if the string \a input contains a double
    that is within the valid range and is in the correct format.

    Returns \l Intermediate if \a input contains a double that is
    outside the range or is in the wrong format; e.g. is empty.

    Returns \l Invalid if the \a input is not a double or with too many
    digits after the decimal point.

    Note: If the valid range consists of just positive doubles (e.g. 0.0 to 100.0)
    and \a input is a negative double then \l Invalid is returned. If notation()
    is set to StandardNotation, and the input contains more digits before the
    decimal point than a double in the valid range may have, \l Invalid is returned.
    If notation() is ScientificNotation, and the input is not in the valid range,
    \l Intermediate is returned. The value may yet become valid by changing the exponent.

    By default, the \a pos parameter is not used by this validator.
*/

#ifndef LLONG_MAX
#   define LLONG_MAX Q_INT64_C(0x7fffffffffffffff)
#endif

QValidator::State QDoubleValidator::validate(QString & input, int &) const
{
    Q_D(const QDoubleValidator);

    QLocaleData::NumberMode numMode = QLocaleData::DoubleStandardMode;
    switch (d->notation) {
        case StandardNotation:
            numMode = QLocaleData::DoubleStandardMode;
            break;
        case ScientificNotation:
            numMode = QLocaleData::DoubleScientificMode;
            break;
    }

    return d->validateWithLocale(input, numMode, locale());
}

QValidator::State QDoubleValidatorPrivate::validateWithLocale(QString &input, QLocaleData::NumberMode numMode, const QLocale &locale) const
{
    Q_Q(const QDoubleValidator);
    QByteArray buff;
    if (!locale.d->m_data->validateChars(input, numMode, &buff, q->dec, locale.numberOptions())) {
        return QValidator::Invalid;
    }

    if (buff.isEmpty())
        return QValidator::Intermediate;

    if (q->b >= 0 && buff.startsWith('-'))
        return QValidator::Invalid;

    if (q->t < 0 && buff.startsWith('+'))
        return QValidator::Invalid;

    bool ok = false;
    double i = buff.toDouble(&ok); // returns 0.0 if !ok
    if (i == qt_qnan())
        return QValidator::Invalid;
    if (!ok)
        return QValidator::Intermediate;

    if (i >= q->b && i <= q->t)
        return QValidator::Acceptable;

    if (notation == QDoubleValidator::StandardNotation) {
        double max = qMax(qAbs(q->b), qAbs(q->t));
        if (max < (double)LLONG_MAX) {
            qlonglong n = pow10(numDigits(qlonglong(max)));
            // In order to get the highest possible number in the intermediate
            // range we need to get 10 to the power of the number of digits
            // after the decimal's and subtract that from the top number.
            //
            // For example, where q->dec == 2 and with a range of 0.0 - 9.0
            // then the minimum possible number is 0.00 and the maximum
            // possible is 9.99. Therefore 9.999 and 10.0 should be seen as
            // invalid.
            if (qAbs(i) > (n - std::pow(10, -q->dec)))
                return QValidator::Invalid;
        }
    }

    return QValidator::Intermediate;
}

// FIXME: Qt 6: Make QDoubleValidator::setRange() non-virtual

/*!
    Sets the validator to accept doubles from \a minimum to \a maximum
    inclusive, with at most \a decimals digits after the decimal
    point.
*/

void QDoubleValidator::setRange(double minimum, double maximum, int decimals)
{
    bool rangeChanged = false;
    if (b != minimum) {
        b = minimum;
        rangeChanged = true;
        emit bottomChanged(b);
    }

    if (t != maximum) {
        t = maximum;
        rangeChanged = true;
        emit topChanged(t);
    }

    if (dec != decimals) {
        dec = decimals;
        rangeChanged = true;
        emit decimalsChanged(dec);
    }
    if (rangeChanged)
        emit changed();
}

/*!
    \property QDoubleValidator::bottom
    \brief the validator's minimum acceptable value

    By default, this property contains a value of -infinity.

    \sa setRange()
*/

void QDoubleValidator::setBottom(double bottom)
{
    setRange(bottom, top(), decimals());
}


/*!
    \property QDoubleValidator::top
    \brief the validator's maximum acceptable value

    By default, this property contains a value of infinity.

    \sa setRange()
*/

void QDoubleValidator::setTop(double top)
{
    setRange(bottom(), top, decimals());
}

/*!
    \property QDoubleValidator::decimals
    \brief the validator's maximum number of digits after the decimal point

    By default, this property contains a value of 1000.

    \sa setRange()
*/

void QDoubleValidator::setDecimals(int decimals)
{
    setRange(bottom(), top(), decimals);
}

/*!
    \property QDoubleValidator::notation
    \since 4.3
    \brief the notation of how a string can describe a number

    By default, this property is set to ScientificNotation.

    \sa Notation
*/

void QDoubleValidator::setNotation(Notation newNotation)
{
    Q_D(QDoubleValidator);
    if (d->notation != newNotation) {
        d->notation = newNotation;
        emit notationChanged(d->notation);
        emit changed();
    }
}

QDoubleValidator::Notation QDoubleValidator::notation() const
{
    Q_D(const QDoubleValidator);
    return d->notation;
}

/*!
    \class QRegExpValidator
    \brief The QRegExpValidator class is used to check a string
    against a regular expression.
    \inmodule QtGui

    QRegExpValidator uses a regular expression (regexp) to
    determine whether an input string is \l Acceptable, \l
    Intermediate, or \l Invalid. The regexp can either be supplied
    when the QRegExpValidator is constructed, or at a later time.

    When QRegExpValidator determines whether a string is \l Acceptable
    or not, the regexp is treated as if it begins with the start of string
    assertion (\b{^}) and ends with the end of string assertion
    (\b{$}); the match is against the entire input string, or from
    the given position if a start position greater than zero is given.

    If a string is a prefix of an \l Acceptable string, it is considered
    \l Intermediate. For example, "" and "A" are \l Intermediate for the
    regexp \b{[A-Z][0-9]} (whereas "_" would be \l Invalid).

    For a brief introduction to Qt's regexp engine, see \l QRegExp.

    Example of use:
    \snippet code/src_gui_util_qvalidator.cpp 3

    Below we present some examples of validators. In practice they would
    normally be associated with a widget as in the example above.

    \snippet code/src_gui_util_qvalidator.cpp 4

    \sa QRegExp, QIntValidator, QDoubleValidator, {Settings Editor Example}
*/

/*!
    Constructs a validator with a \a parent object that accepts
    any string (including an empty one) as valid.
*/

QRegExpValidator::QRegExpValidator(QObject *parent)
    : QRegExpValidator(QRegExp(QString::fromLatin1(".*")), parent)
{
}

/*!
    Constructs a validator with a \a parent object that
    accepts all strings that match the regular expression \a rx.

    The match is made against the entire string; e.g. if the regexp is
    \b{[A-Fa-f0-9]+} it will be treated as \b{^[A-Fa-f0-9]+$}.
*/

QRegExpValidator::QRegExpValidator(const QRegExp& rx, QObject *parent)
    : QValidator(parent), r(rx)
{
}


/*!
    Destroys the validator.
*/

QRegExpValidator::~QRegExpValidator()
{
}

/*!
    Returns \l Acceptable if \a input is matched by the regular
    expression for this validator, \l Intermediate if it has matched
    partially (i.e. could be a valid match if additional valid
    characters are added), and \l Invalid if \a input is not matched.

    Additionally, if \a input is not matched, the \a pos parameter is set to
    the length of the \a input parameter.

    For example, if the regular expression is \b{\\w\\d\\d}
    (word-character, digit, digit) then "A57" is \l Acceptable,
    "E5" is \l Intermediate, and "+9" is \l Invalid.

    \sa QRegExp::exactMatch()
*/

QValidator::State QRegExpValidator::validate(QString &input, int& pos) const
{
    QRegExp copy = r;
    if (copy.exactMatch(input)) {
        return Acceptable;
    } else {
        if (copy.matchedLength() == input.size()) {
            return Intermediate;
        } else {
            pos = input.size();
            return Invalid;
        }
    }
}

/*!
    \property QRegExpValidator::regExp
    \brief the regular expression used for validation

    By default, this property contains a regular expression with the pattern \c{.*}
    that matches any string.
*/

void QRegExpValidator::setRegExp(const QRegExp& rx)
{
    if (r != rx) {
        r = rx;
        emit regExpChanged(r);
        emit changed();
    }
}

#endif

#if QT_CONFIG(regularexpression)

/*!
    \class QRegularExpressionValidator
    \brief The QRegularExpressionValidator class is used to check a string
    against a regular expression.

    \since 5.1

    QRegularExpressionValidator uses a regular expression (regexp) to
    determine whether an input string is \l Acceptable, \l
    Intermediate, or \l Invalid. The regexp can either be supplied
    when the QRegularExpressionValidator is constructed, or at a later time.

    If the regexp partially matches against the string, the result is
    considered \l Intermediate. For example, "" and "A" are \l Intermediate for
    the regexp \b{[A-Z][0-9]} (whereas "_" would be \l Invalid).

    QRegularExpressionValidator automatically wraps the regular expression in
    the \c{\\A} and \c{\\z} anchors; in other words, it always attempts to do
    an exact match.

    Example of use:
    \snippet code/src_gui_util_qvalidator.cpp 5

    Below we present some examples of validators. In practice they would
    normally be associated with a widget as in the example above.

    \snippet code/src_gui_util_qvalidator.cpp 6

    \sa QRegularExpression, QIntValidator, QDoubleValidator, QRegExpValidator
*/

class QRegularExpressionValidatorPrivate : public QValidatorPrivate
{
    Q_DECLARE_PUBLIC(QRegularExpressionValidator)

public:
    QRegularExpression origRe; // the one set by the user
    QRegularExpression usedRe; // the one actually used
    void setRegularExpression(const QRegularExpression &re);
};

/*!
    Constructs a validator with a \a parent object that accepts
    any string (including an empty one) as valid.
*/

QRegularExpressionValidator::QRegularExpressionValidator(QObject *parent)
    : QValidator(*new QRegularExpressionValidatorPrivate, parent)
{
    // origRe in the private will be an empty QRegularExpression,
    // and therefore this validator will match any string.
}

/*!
    Constructs a validator with a \a parent object that
    accepts all strings that match the regular expression \a re.
*/

QRegularExpressionValidator::QRegularExpressionValidator(const QRegularExpression &re, QObject *parent)
    : QRegularExpressionValidator(parent)
{
    Q_D(QRegularExpressionValidator);
    d->setRegularExpression(re);
}


/*!
    Destroys the validator.
*/

QRegularExpressionValidator::~QRegularExpressionValidator()
{
}

/*!
    Returns \l Acceptable if \a input is matched by the regular expression for
    this validator, \l Intermediate if it has matched partially (i.e. could be
    a valid match if additional valid characters are added), and \l Invalid if
    \a input is not matched.

    In case the \a input is not matched, the \a pos parameter is set to
    the length of the \a input parameter; otherwise, it is not modified.

    For example, if the regular expression is \b{\\w\\d\\d} (word-character,
    digit, digit) then "A57" is \l Acceptable, "E5" is \l Intermediate, and
    "+9" is \l Invalid.

    \sa QRegularExpression::match()
*/

QValidator::State QRegularExpressionValidator::validate(QString &input, int &pos) const
{
    Q_D(const QRegularExpressionValidator);

    // We want a validator with an empty QRegularExpression to match anything;
    // since we're going to do an exact match (by using d->usedRe), first check if the rx is empty
    // (and, if so, accept the input).
    if (d->origRe.pattern().isEmpty())
        return Acceptable;

    const QRegularExpressionMatch m = d->usedRe.match(input, 0, QRegularExpression::PartialPreferCompleteMatch);
    if (m.hasMatch()) {
        return Acceptable;
    } else if (input.isEmpty() || m.hasPartialMatch()) {
        return Intermediate;
    } else {
        pos = input.size();
        return Invalid;
    }
}

/*!
    \property QRegularExpressionValidator::regularExpression
    \brief the regular expression used for validation

    By default, this property contains a regular expression with an empty
    pattern (which therefore matches any string).
*/

QRegularExpression QRegularExpressionValidator::regularExpression() const
{
    Q_D(const QRegularExpressionValidator);
    return d->origRe;
}

void QRegularExpressionValidator::setRegularExpression(const QRegularExpression &re)
{
    Q_D(QRegularExpressionValidator);
    d->setRegularExpression(re);
}

/*!
    \internal

    Sets \a re as the regular expression. It wraps the regexp that's actually used
    between \\A and \\z, therefore forcing an exact match.
*/
void QRegularExpressionValidatorPrivate::setRegularExpression(const QRegularExpression &re)
{
    Q_Q(QRegularExpressionValidator);

    if (origRe != re) {
        usedRe = origRe = re; // copies also the pattern options
        usedRe.setPattern(QRegularExpression::anchoredPattern(re.pattern()));
        emit q->regularExpressionChanged(re);
        emit q->changed();
    }
}

#endif // QT_CONFIG(regularexpression)

QT_END_NAMESPACE

#endif // QT_NO_VALIDATOR
