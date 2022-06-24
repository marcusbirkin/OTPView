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
#ifndef PRIORITYSPINBOX_H
#define PRIORITYSPINBOX_H

#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QList>
#include "OTPLib.hpp"

class PrioritySpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    typedef OTP::priority_t value_t;

    explicit PrioritySpinBox(
            std::shared_ptr<class OTP::Producer> otpProducer,
            QWidget* parent = nullptr);

    value_t value() const { return m_value; }

    value_t minimum() const { return range.first; }
    void setMinimum(value_t min) {
        range.first = std::clamp(
                    min,
                    static_cast<value_t>(OTP::RANGES::Priority.getMin()),
                    static_cast<value_t>(OTP::RANGES::Priority.getMax())
                    );
    }

    value_t maximum() const { return range.second; }
    void setMaximum(value_t max) {
        range.second = std::clamp(
                    max,
                    static_cast<value_t>(OTP::RANGES::Priority.getMin()),
                    static_cast<value_t>(OTP::RANGES::Priority.getMax())
                    );
    }

    void setRange(value_t min, value_t max)
    {
        setMinimum(min);
        setMaximum(max);
    }

    void setRange(OTP::range_t range)
    {
        setRange(range.getMin(), range.getMax());
    }

public slots:
    void setValue(PrioritySpinBox::value_t val);
    void setAddress(OTP::address_t);

signals:
    void valueChanged(PrioritySpinBox::value_t value);
    void valueChanged(PrioritySpinBox::value_t oldValue, PrioritySpinBox::value_t newValue);

protected:
    void stepBy(int steps);

    QValidator::State validate(const value_t &input) const;
    QValidator::State validate(QString &input, int &pos) const;

    value_t valueFromText(const QString &text) const;
    value_t valueFromText(const QString &text, bool *ok) const;
    QString textFromValue(value_t val) const;

    QAbstractSpinBox::StepEnabled stepEnabled() const {return StepUpEnabled | StepDownEnabled; }

private slots:
    void processInput();

private:
    std::shared_ptr<class OTP::Producer> otpProducer;
    OTP::address_t address;
    OTP::MODULES::STANDARD::VALUES::moduleValue_t moduleValue;

    std::pair<value_t,value_t> range = {std::numeric_limits<value_t>::min(),std::numeric_limits<value_t>::max()};
    value_t m_value;
};

#endif // PRIORITYSPINBOX_H
