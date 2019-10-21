/*
    OTPView
    A QT Frontend for E1.59  (Entertainment  Technology  Object  Transform  Protocol  (OTP))
    Copyright (C) 2019  Marcus Birkin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "groupspinbox.h"

using namespace ACN::OTP;

GroupSpinBox::GroupSpinBox(QList<group_t> usedGroups, QWidget* parent) : QAbstractSpinBox(parent),
    usedGroups(usedGroups)
{
    setRange(group_t::getMin(), group_t::getMax());
    setValue(group_t::getMin());
    while (QValidator::Invalid == validate(value())) stepBy(1);

    connect(this->lineEdit(), &QLineEdit::textEdited,
        [this]() {
            QString input = lineEdit()->text();
            int pos = 0;
            if (QValidator::Acceptable == validate(input, pos))
                setValue(valueFromText(input));
            else
                lineEdit()->setText(textFromValue(m_value));
        });
}

void GroupSpinBox::setValue(group_t val)
{
    auto oldValue = m_value;
    if (m_value != val) {
        lineEdit()->setText(textFromValue(val));
        m_value = val;

        emit valueChanged(m_value);
        emit valueChanged(oldValue, m_value);
    }
}

void GroupSpinBox::stepBy(int steps)
{
    group_t newValue = value() + static_cast<group_t>(steps);
    if (steps > 0)
    {
        while (QValidator::Invalid == validate(newValue)) ++newValue;
    } else if (steps < 0)
    {
        while (QValidator::Invalid == validate(newValue)) --newValue;
    }

    newValue = std::clamp(newValue, group_t::getMin(), group_t::getMax());
    lineEdit()->setText(textFromValue(newValue));
    setValue(newValue);
}

QValidator::State GroupSpinBox::validate(const ACN::OTP::group_t &input) const
{
    if (input < minimum() || input > maximum())
        return QValidator::Invalid;

    if (usedGroups.contains(input))
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

QValidator::State GroupSpinBox::validate(QString &input, int &pos) const
{
    bool ok;
    ACN::OTP::group_t val = input.mid(pos).toUShort(&ok);
    if (!ok)
        return QValidator::Invalid;

    return validate(val);
}


ACN::OTP::group_t GroupSpinBox::valueFromText(const QString &text) const
{
    return text.toUShort();
}

QString GroupSpinBox::textFromValue(ACN::OTP::group_t val) const
{
    return QString::number(val);
}
