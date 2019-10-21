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
#ifndef SYSTEMSPINBOX_H
#define SYSTEMSPINBOX_H

#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QList>
#include "libs/OTPLib/const.hpp"

class SystemSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    SystemSpinBox(QWidget* parent = Q_NULLPTR) : SystemSpinBox(QList<ACN::OTP::system_t>(), parent) {}
    SystemSpinBox(QList<ACN::OTP::system_t> usedSystems, QWidget* parent = Q_NULLPTR);

    ACN::OTP::system_t value() const { return m_value; }

    ACN::OTP::system_t minimum() const { return range.first; }
    void setMinimum(ACN::OTP::system_t  min) { range.first = min; }

    ACN::OTP::system_t maximum() const { return range.second; }
    void setMaximum(ACN::OTP::system_t max) {  range.second = max; }

    void setRange(ACN::OTP::system_t min, ACN::OTP::system_t max)
    {
        setMinimum(min);
        setMaximum(max);
    }

public slots:
    void setValue(ACN::OTP::system_t val);

signals:
    void valueChanged(ACN::OTP::system_t value);
    void valueChanged(ACN::OTP::system_t oldValue, ACN::OTP::system_t newValue);

protected:
    void stepBy(int steps);

    QValidator::State validate(const ACN::OTP::system_t &input) const;
    QValidator::State validate(QString &input, int &pos) const;

    ACN::OTP::system_t valueFromText(const QString &text) const;
    QString textFromValue(ACN::OTP::system_t val) const;

    QAbstractSpinBox::StepEnabled stepEnabled() const {return StepUpEnabled | StepDownEnabled; }

private:
    QList<ACN::OTP::system_t> usedSystems;

    std::pair<ACN::OTP::system_t,ACN::OTP::system_t> range;
    ACN::OTP::system_t m_value;
};

#endif // SYSTEMSPINBOX_H
