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
            {
                auto position = otpProducer->getProducerPosition(address, axis);
                position.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                position.value = m_value;
                otpProducer->setProducerPosition(address, axis, position); break;
            }
            case VALUES::POSITION_VELOCITY:
            {
                auto positionVel = otpProducer->getProducerPositionVelocity(address, axis);
                positionVel.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                positionVel.value = m_value;
                otpProducer->setProducerPositionVelocity(address, axis, positionVel); break;
            }
            case VALUES::POSITION_ACCELERATION:
            {
                auto positionAccel = otpProducer->getProducerPositionAcceleration(address, axis);
                positionAccel.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                positionAccel.value = m_value;
                otpProducer->setProducerPositionAcceleration(address, axis, positionAccel); break;
            }
            case VALUES::ROTATION:
            {
                auto rotation = otpProducer->getProducerRotation(address, axis);
                rotation.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                rotation.value = m_value;
                otpProducer->setProducerRotation(address, axis, rotation); break;
            }
            case VALUES::ROTATION_VELOCITY:
            {
                auto rotationVel = otpProducer->getProducerRotationVelocity(address, axis);
                rotationVel.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                rotationVel.value = m_value;
                otpProducer->setProducerRotationVelocity(address, axis, rotationVel); break;
            }
            case VALUES::ROTATION_ACCELERATION:
            {
                auto rotationAccel = otpProducer->getProducerRotationAcceleration(address, axis);
                rotationAccel.timestamp = static_cast<ACN::OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                rotationAccel.value = m_value;
                otpProducer->setProducerRotationAcceleration(address, axis, rotationAccel); break;
            }
        }
    }
}

void SpacialSpinBox::setAddress(ACN::OTP::address_t value)
{
    address = value;
    switch (moduleValue) {
        case VALUES::POSITION:
            setValue(otpProducer->getProducerPosition(address, axis).value); break;
        case VALUES::POSITION_VELOCITY:
            setValue(otpProducer->getProducerPositionVelocity(address, axis).value); break;
        case VALUES::POSITION_ACCELERATION:
            setValue(otpProducer->getProducerPositionAcceleration(address, axis).value); break;
        case VALUES::ROTATION:
            setValue(otpProducer->getProducerRotation(address, axis).value); break;
        case VALUES::ROTATION_VELOCITY:
            setValue(otpProducer->getProducerRotationVelocity(address, axis).value); break;
        case VALUES::ROTATION_ACCELERATION:
            setValue(otpProducer->getProducerRotationAcceleration(address, axis).value); break;
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
    return QString("%1 %2").arg(QString::number(val)).arg(
                otpProducer->getUnitString(
                    otpProducer->getProducerPosition(address, axis).scale,
                    moduleValue));
}

QAbstractSpinBox::StepEnabled SpacialSpinBox::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled ret = StepNone;
    if (validate(value() + 1) == QValidator::Acceptable) ret |= StepUpEnabled;
    if (validate(value() - 1) == QValidator::Acceptable) ret |= StepDownEnabled;
    return ret;
}
