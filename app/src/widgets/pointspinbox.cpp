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
#include "pointspinbox.h"

using namespace OTP;

PointSpinBox::PointSpinBox(const QList<point_t> &usedPoints, QWidget* parent) : QAbstractSpinBox(parent),
    usedPoints(usedPoints)
{
    setRange(point_t::getMin(), point_t::getMax());
    setValue(point_t::getMin());
    while (QValidator::Invalid == validate(value()))
        PointSpinBox::stepBy(1);

    connect(this->lineEdit(), &QLineEdit::textEdited, this,
        [this]() {
            QString input = lineEdit()->text();
            int pos = 0;
            if (QValidator::Acceptable == validate(input, pos))
                setValue(valueFromText(input));
            else
                lineEdit()->setText(textFromValue(m_value));
        });
}

void PointSpinBox::setValue(point_t val)
{
    auto oldValue = m_value;
    if (m_value != val) {
        lineEdit()->setText(textFromValue(val));
        m_value = val;

        emit valueChanged(static_cast<int>(m_value));
        emit valueChanged(m_value);
        emit valueChanged(oldValue, m_value);
    }
}

void PointSpinBox::stepBy(int steps)
{
    point_t newValue = value() + static_cast<point_t>(steps);
    if (steps > 0)
    {
        while (QValidator::Invalid == validate(newValue)) ++newValue;
    } else if (steps < 0)
    {
        while (QValidator::Invalid == validate(newValue)) --newValue;
    }

    newValue = std::clamp(newValue, point_t::getMin(), point_t::getMax());
    lineEdit()->setText(textFromValue(newValue));
    setValue(newValue);
}

QValidator::State PointSpinBox::validate(const OTP::point_t &input) const
{
    if (input < minimum() || input > maximum())
        return QValidator::Invalid;

    if (usedPoints.contains(input))
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

QValidator::State PointSpinBox::validate(QString &input, int &pos) const
{
    bool ok;
    OTP::point_t val = input.mid(pos).toUInt(&ok);
    if (!ok)
        return QValidator::Invalid;

    return validate(val);
}


OTP::point_t PointSpinBox::valueFromText(const QString &text) const
{
    return text.toUInt();
}

QString PointSpinBox::textFromValue(OTP::point_t val) const
{
    return QString::number(val);
}
