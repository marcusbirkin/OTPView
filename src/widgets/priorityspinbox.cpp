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
#include "priorityspinbox.h"

using namespace OTP;

PrioritySpinBox::PrioritySpinBox(
        std::shared_ptr<class OTP::Producer> otpProducer,
        QWidget* parent) : QAbstractSpinBox(parent),
    otpProducer(otpProducer)
{
    setRange(RANGES::Priority);

    lineEdit()->setText(textFromValue(m_value));

    connect(this->lineEdit(), SIGNAL(editingFinished()), this, SLOT(processInput()));
    connect(this->lineEdit(), SIGNAL(returnPressed()), this, SLOT(processInput()));

    connect(otpProducer.get(), &Producer::updatedProducerPointPriority, this, [=](address_t address) {
        if (this->address.isValid() && this->address == address) {
            m_value = otpProducer.get()->getProducerPointPriority(address);
            lineEdit()->setText(textFromValue(m_value));
        }
    });
}

void PrioritySpinBox::processInput()
{
    QString input = lineEdit()->text();
    int pos = 0;
    if (QValidator::Acceptable == validate(input, pos))
        setValue(valueFromText(input));
    else
        lineEdit()->setText(textFromValue(m_value));
}

void PrioritySpinBox::setValue(value_t val)
{
    auto oldValue = m_value;
    if (m_value != val) {
        lineEdit()->setText(textFromValue(val));
        m_value = val;

        if (!address.isValid()) return;
        emit valueChanged(m_value);
        emit valueChanged(oldValue, m_value);

        otpProducer->setProducerPointPriority(address, m_value);
    }
}

void PrioritySpinBox::setAddress(OTP::address_t value)
{
    address = value;
    setValue(otpProducer->getProducerPointPriority(address));
}

void PrioritySpinBox::stepBy(int steps)
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

QValidator::State PrioritySpinBox::validate(const value_t &input) const
{
    if (input < minimum() || input > maximum())
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

QValidator::State PrioritySpinBox::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos)
    bool ok;

    value_t val = valueFromText(input, &ok);
    if (!ok)
        return QValidator::Invalid;

    return validate(val);
}

PrioritySpinBox::value_t PrioritySpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return valueFromText(text, &ok);
}

PrioritySpinBox::value_t PrioritySpinBox::valueFromText(const QString &text, bool *ok) const
{
    auto temp = text.split(" ").first();
    *ok = !temp.isEmpty();
    return temp.toInt();
}

QString PrioritySpinBox::textFromValue(value_t val) const
{
    return QString::number(val);
}
