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
#include "OTPLib.hpp"

class GroupSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    GroupSpinBox(QWidget* parent = nullptr) : GroupSpinBox(QList<OTP::group_t>(), parent) {}
    GroupSpinBox(QList<OTP::group_t> usedGroups, QWidget* parent = nullptr);

    OTP::group_t value() const { return m_value; }

    OTP::group_t minimum() const { return range.first; }
    void setMinimum(OTP::group_t  min) { range.first = min; }

    OTP::group_t maximum() const { return range.second; }
    void setMaximum(OTP::group_t max) {  range.second = max; }

    void setRange(OTP::group_t min, OTP::group_t max)
    {
        setMinimum(min);
        setMaximum(max);
    }

public slots:
    void setValue(OTP::group_t val);

signals:
    void valueChanged(int value);
    void valueChanged(OTP::group_t value);
    void valueChanged(OTP::group_t oldValue, OTP::group_t newValue);

protected:
    void stepBy(int steps);

    QValidator::State validate(const OTP::group_t &input) const;
    QValidator::State validate(QString &input, int &pos) const;

    OTP::group_t valueFromText(const QString &text) const;
    QString textFromValue(OTP::group_t val) const;

    QAbstractSpinBox::StepEnabled stepEnabled() const {return StepUpEnabled | StepDownEnabled; }

private:
    QList<OTP::group_t> usedGroups;

    std::pair<OTP::group_t,OTP::group_t> range;
    OTP::group_t m_value;
};

#endif // GROUPSPINBOX_H
