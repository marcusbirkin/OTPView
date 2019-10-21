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
#include "spacialspinbox.h"
#include <QLineEdit>
#include <regex>

using namespace ACN::OTP::MODULES::STANDARD;

SpacialSpinBox::SpacialSpinBox(
        std::shared_ptr<class ACN::OTP::Producer> otpProducer,
        ACN::OTP::axis_t axis,
        VALUES::moduleValue_t moduleValue,
        QWidget *parent) : QAbstractSpinBox(parent),
    otpProducer(otpProducer),
    axis(axis),
    moduleValue(moduleValue)
{
    setRange(VALUES::RANGES::getRange(moduleValue));

    lineEdit()->setText(textFromValue(m_value));

    connect(this->lineEdit(), SIGNAL(editingFinished()), this, SLOT(processInput()));
    connect(this->lineEdit(), SIGNAL(returnPressed()), this, SLOT(processInput()));
}

void SpacialSpinBox::processInput()
{
    QString input = lineEdit()->text();
    int pos = 0;
    if (QValidator::Acceptable == validate(input, pos))
        setValue(valueFromText(input));
    else
        lineEdit()->setText(textFromValue(m_value));
}

void SpacialSpinBox::setValue(value_t val)
{
    lineEdit()->setText(textFromValue(val));
    if (m_value != val) {
        m_value = val;

        if (!address.isValid()) return;
        emit valueChanged(m_value);
        switch (moduleValue) {
            case VALUES::POSITION:
                otpProducer->setPositionLocation(address, axis, m_value); break;
            case VALUES::POSITION_VELOCITY:
                otpProducer->setPositionVelocity(address, axis, m_value); break;
            case VALUES::POSITION_ACCELERATION:
                otpProducer->setPositionAcceleration(address, axis, m_value); break;
            case VALUES::ROTATION:
                otpProducer->setRotation(address, axis, m_value); break;
            case VALUES::ROTATION_VELOCITY:
                otpProducer->setRotationVelocity(address, axis, m_value); break;
            case VALUES::ROTATION_ACCELERATION:
                otpProducer->setRotationAcceleration(address, axis, m_value); break;
        }
    }
}

void SpacialSpinBox::setAddress(ACN::OTP::address_t value)
{
    address = value;
    switch (moduleValue) {
        case VALUES::POSITION: setValue(otpProducer->getPositionLocation(address, axis)); break;
        case VALUES::POSITION_VELOCITY: setValue(otpProducer->getPositionVelocity(address, axis)); break;
        case VALUES::POSITION_ACCELERATION: setValue(otpProducer->getPositionVelocity(address, axis)); break;
        case VALUES::ROTATION: setValue(otpProducer->getRotation(address, axis)); break;
        case VALUES::ROTATION_VELOCITY: setValue(otpProducer->getRotationVelocity(address, axis)); break;
        case VALUES::ROTATION_ACCELERATION: setValue(otpProducer->getRotationAcceleration(address, axis)); break;
    }
}

void SpacialSpinBox::stepBy(int steps)
{
    auto newValue = std::clamp(value() + steps, minimum(), maximum());
    if (validate(newValue) != QValidator::Acceptable) return;
    lineEdit()->setText(textFromValue(newValue));
    setValue(newValue);
}

QValidator::State SpacialSpinBox::validate(const value_t &input) const
{
    if (input < minimum() || input > maximum())
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

QValidator::State SpacialSpinBox::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos)
    bool ok;

    value_t val = valueFromText(input, &ok);
    if (!ok)
        return QValidator::Invalid;

    return validate(val);
}

SpacialSpinBox::value_t SpacialSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return valueFromText(text, &ok);
}

SpacialSpinBox::value_t SpacialSpinBox::valueFromText(const QString &text, bool *ok) const
{
    auto temp = text.split(" ").first();
    *ok = !temp.isEmpty();
    return temp.toInt();
}

QString SpacialSpinBox::textFromValue(value_t val) const
{
    return QString("%1 %2").arg(QString::number(val)).arg(otpProducer->getUnitString(address, moduleValue));
}

QAbstractSpinBox::StepEnabled SpacialSpinBox::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled ret = StepNone;
    if (validate(value() + 1) == QValidator::Acceptable) ret |= StepUpEnabled;
    if (validate(value() - 1) == QValidator::Acceptable) ret |= StepDownEnabled;
    return ret;
}
