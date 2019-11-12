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
#ifndef GROUPSPINBOX_H
#define GROUPSPINBOX_H

#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QList>
#include "libs/OTPLib/const.hpp"

class GroupSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    GroupSpinBox(QWidget* parent = nullptr) : GroupSpinBox(QList<ACN::OTP::group_t>(), parent) {}
    GroupSpinBox(QList<ACN::OTP::group_t> usedGroups, QWidget* parent = nullptr);

    ACN::OTP::group_t value() const { return m_value; }

    ACN::OTP::group_t minimum() const { return range.first; }
    void setMinimum(ACN::OTP::group_t  min) { range.first = min; }

    ACN::OTP::group_t maximum() const { return range.second; }
    void setMaximum(ACN::OTP::group_t max) {  range.second = max; }

    void setRange(ACN::OTP::group_t min, ACN::OTP::group_t max)
    {
        setMinimum(min);
        setMaximum(max);
    }

public slots:
    void setValue(ACN::OTP::group_t val);

signals:
    void valueChanged(ACN::OTP::group_t value);
    void valueChanged(ACN::OTP::group_t oldValue, ACN::OTP::group_t newValue);

protected:
    void stepBy(int steps);

    QValidator::State validate(const ACN::OTP::group_t &input) const;
    QValidator::State validate(QString &input, int &pos) const;

    ACN::OTP::group_t valueFromText(const QString &text) const;
    QString textFromValue(ACN::OTP::group_t val) const;

    QAbstractSpinBox::StepEnabled stepEnabled() const {return StepUpEnabled | StepDownEnabled; }

private:
    QList<ACN::OTP::group_t> usedGroups;

    std::pair<ACN::OTP::group_t,ACN::OTP::group_t> range;
    ACN::OTP::group_t m_value;
};

#endif // GROUPSPINBOX_H
