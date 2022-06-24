/*
    OTPView
    A QT Frontend for E1.59  (Entertainment  Technology  Object  Transform  Protocol  (OTP))
    Copyright (C) 2020  Marcus Birkin

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
#include "scalespinbox.h"

using namespace OTP;

ScaleSpinBox::ScaleSpinBox(
        std::shared_ptr<class OTP::Producer> otpProducer,
        axis_t axis,
        QWidget* parent) : QAbstractSpinBox(parent),
    otpProducer(otpProducer),
    axis(axis)
{
    setRange(VALUES::RANGES::getRange(VALUES::SCALE));

    setValue(m_value);

    connect(this->lineEdit(), SIGNAL(editingFinished()), this, SLOT(processInput()));
    connect(this->lineEdit(), SIGNAL(returnPressed()), this, SLOT(processInput()));

    connect(otpProducer.get(), &Producer::updatedScale, this, [=](address_t address, axis_t axis) {
        if (this->address.isValid() &&
                std::tie(this->address, this->axis)
                ==  std::tie(address, axis))
        {
            m_value = otpProducer.get()->getLocalScale(address, axis).value;
            lineEdit()->setText(textFromValue(m_value));
        }
    });
}

void ScaleSpinBox::processInput()
{
    QString input = lineEdit()->text();
    int pos = 0;
    if (QValidator::Acceptable == validate(input, pos))
        setValue(valueFromText(input));
    else
        lineEdit()->setText(textFromValue(m_value));
}

void ScaleSpinBox::setValue(value_t val)
{
    auto oldValue = m_value;
    m_value = val;

    lineEdit()->setText(textFromValue(val));


    if (!address.isValid()) return;
    emit valueChanged(m_value);
    if (m_value != val)
        emit valueChanged(oldValue, m_value);

    auto scale = otpProducer->getLocalScale(address, axis);
    scale.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
    scale.value = m_value;
    otpProducer->setLocalScale(address, axis, scale);
}

void ScaleSpinBox::setAddress(OTP::address_t value)
{
    address = value;
    setValue(otpProducer->getLocalScale(address, axis).value);
}

void ScaleSpinBox::stepBy(int steps)
{
    value_t newValue = value() + static_cast<value_t>(steps);
    if (steps > 0)
    {
        while (QValidator::Invalid == validate(newValue)) ++newValue;
    } else if (steps < 0)
    {
        while (QValidator::Invalid == validate(newValue)) --newValue;
    }

    newValue = std::clamp(newValue, minimum(), maximum());
    lineEdit()->setText(textFromValue(newValue));
    setValue(newValue);
}

QValidator::State ScaleSpinBox::validate(const value_t &input) const
{
    if (input < minimum() || input > maximum())
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

QValidator::State ScaleSpinBox::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos)
    bool ok;

    value_t val = valueFromText(input, &ok);
    if (!ok)
        return QValidator::Invalid;

    return validate(val);
}

ScaleSpinBox::value_t ScaleSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return valueFromText(text, &ok);
}

ScaleSpinBox::value_t ScaleSpinBox::valueFromText(const QString &text, bool *ok) const
{
    auto temp = text;
    return ScaleModule_t::fromPercent(temp.remove(suffix).toFloat(ok));
}

QString ScaleSpinBox::textFromValue(value_t val) const
{
    return QString("%1 %2").arg(
                ScaleModule_t::toPercentString(val),
                suffix);
}
