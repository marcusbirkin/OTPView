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

using namespace OTP::MODULES::STANDARD;

SpacialSpinBox::SpacialSpinBox(
        std::shared_ptr<class OTP::Producer> otpProducer,
        OTP::axis_t axis,
        VALUES::moduleValue_t moduleValue,
        QWidget *parent) : QAbstractSpinBox(parent),
    otpProducer(otpProducer),
    axis(axis),
    moduleValue(moduleValue)
{
    setRange(VALUES::RANGES::getRange(moduleValue));

    setValue(m_value);

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
    m_value = val;
    lineEdit()->setText(textFromValue(val));

    if (!address.isValid()) return;
    emit valueChanged(m_value);
    switch (moduleValue) {
        case VALUES::POSITION:
        {
            auto position = otpProducer->getLocalPosition(address, axis);
            position.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            position.value = m_value;
            otpProducer->setLocalPosition(address, axis, position); break;
        }
        case VALUES::POSITION_VELOCITY:
        {
            auto positionVel = otpProducer->getLocalPositionVelocity(address, axis);
            positionVel.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            positionVel.value = m_value;
            otpProducer->setLocalPositionVelocity(address, axis, positionVel); break;
        }
        case VALUES::POSITION_ACCELERATION:
        {
            auto positionAccel = otpProducer->getLocalPositionAcceleration(address, axis);
            positionAccel.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            positionAccel.value = m_value;
            otpProducer->setLocalPositionAcceleration(address, axis, positionAccel); break;
        }
        case VALUES::ROTATION:
        {
            auto rotation = otpProducer->getLocalRotation(address, axis);
            rotation.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            rotation.value = m_value;
            otpProducer->setLocalRotation(address, axis, rotation); break;
        }
        case VALUES::ROTATION_VELOCITY:
        {
            auto rotationVel = otpProducer->getLocalRotationVelocity(address, axis);
            rotationVel.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            rotationVel.value = m_value;
            otpProducer->setLocalRotationVelocity(address, axis, rotationVel); break;
        }
        case VALUES::ROTATION_ACCELERATION:
        {
            auto rotationAccel = otpProducer->getLocalRotationAcceleration(address, axis);
            rotationAccel.timestamp = static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
            rotationAccel.value = m_value;
            otpProducer->setLocalRotationAcceleration(address, axis, rotationAccel); break;
        }
        case VALUES::SCALE:
            Q_ASSERT(moduleValue != VALUES::SCALE); // Invalid module for this class
            break;
        case VALUES::REFERENCE_FRAME:
           Q_ASSERT(moduleValue != VALUES::REFERENCE_FRAME); // Invalid module for this class
            break;
    }
}

void SpacialSpinBox::setAddress(OTP::address_t value)
{
    address = value;
    switch (moduleValue) {
        case VALUES::POSITION:
            setValue(otpProducer->getLocalPosition(address, axis).value); break;
        case VALUES::POSITION_VELOCITY:
            setValue(otpProducer->getLocalPositionVelocity(address, axis).value); break;
        case VALUES::POSITION_ACCELERATION:
            setValue(otpProducer->getLocalPositionAcceleration(address, axis).value); break;
        case VALUES::ROTATION:
            setValue(otpProducer->getLocalRotation(address, axis).value); break;
        case VALUES::ROTATION_VELOCITY:
            setValue(otpProducer->getLocalRotationVelocity(address, axis).value); break;
        case VALUES::ROTATION_ACCELERATION:
            setValue(otpProducer->getLocalRotationAcceleration(address, axis).value); break;
        case VALUES::SCALE:
            Q_ASSERT(moduleValue != VALUES::SCALE); // Invalid module for this class
            break;
        case VALUES::REFERENCE_FRAME:
           Q_ASSERT(moduleValue != VALUES::REFERENCE_FRAME);// Invalid module for this class
            break;
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
    switch (moduleValue) {
        case VALUES::POSITION:
            return QString("%1 %2").arg(
                        QString::number(val),
                        otpProducer->getUnitString(
                            otpProducer->getLocalPosition(address, axis).scale,
                            moduleValue));
        default:
            return QString("%1 %2").arg(
                        QString::number(val),
                        otpProducer->getUnitString(
                            moduleValue));
    }
}

QAbstractSpinBox::StepEnabled SpacialSpinBox::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled ret = StepNone;
    if (validate(value() + 1) == QValidator::Acceptable) ret |= StepUpEnabled;
    if (validate(value() - 1) == QValidator::Acceptable) ret |= StepDownEnabled;
    return ret;
}
