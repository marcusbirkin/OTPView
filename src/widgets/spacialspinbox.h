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
#ifndef SPACIALSPINBOX_H
#define SPACIALSPINBOX_H

#include <QAbstractSpinBox>
#include "libs/OTPLib/otp.hpp"
#include "libs/OTPLib/network/modules/modules.hpp"

class SpacialSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    typedef qint32 value_t;

    SpacialSpinBox(
            std::shared_ptr<class ACN::OTP::Producer> otpProducer,
            ACN::OTP::axis_t axis,
            ACN::OTP::MODULES::STANDARD::VALUES::moduleValue_t moduleValue,
            QWidget* parent = nullptr);

    value_t value() const { return m_value; }

    value_t minimum() const { return range.first; }
    void setMinimum(value_t min) { range.first = min; }

    value_t maximum() const { return range.second; }
    void setMaximum(value_t max) {  range.second = max; }

    void setRange(value_t min, value_t max)
    {
        setMinimum(min);
        setMaximum(max);
    }

    void setRange(ACN::OTP::range_t range)
    {
        setRange(range.getMin(), range.getMax());
    }


public slots:
    void setValue(value_t val);
    void setAddress(ACN::OTP::address_t);

signals:
    void valueChanged(value_t value);

protected:
    void stepBy(int steps);

    QValidator::State validate(const value_t &input) const;
    QValidator::State validate(QString &input, int &pos) const;

    value_t valueFromText(const QString &text) const;
    value_t valueFromText(const QString &text, bool *ok) const;
    QString textFromValue(value_t val) const;

    QAbstractSpinBox::StepEnabled stepEnabled() const;

private slots:
        void processInput();

private:
    std::shared_ptr<class ACN::OTP::Producer> otpProducer;
    ACN::OTP::address_t address;
    ACN::OTP::axis_t axis;
    ACN::OTP::MODULES::STANDARD::VALUES::moduleValue_t moduleValue;

    std::pair<value_t,value_t> range = {std::numeric_limits<value_t>::min(),std::numeric_limits<value_t>::max()};
    value_t m_value = 0;
    ACN::OTP::MODULES::STANDARD::PositionModule_t::scale_t scale;
};

#endif // SPACIALSPINBOX_H
