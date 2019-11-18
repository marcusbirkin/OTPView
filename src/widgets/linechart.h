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
#ifndef LINECHART_H
#define LINECHART_H

#include <QWidget>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "libs/OTPLib/otp.hpp"

using namespace QtCharts;

class LineChart : public QWidget
{
    Q_OBJECT
public:
    explicit LineChart(
            std::shared_ptr<class ACN::OTP::Consumer> otpConsumer,
            ACN::OTP::address_t address,
            QWidget *parent = nullptr);

signals:

private slots:
    void markerClicked();
    void buttonToggled(int, bool);

    void redraw();
    void addPoint();
    void updatedPosition(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
    void updatedPositionVelocity(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
    void updatedPositionAcceleration(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
    void updatedRotation(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
    void updatedRotationVelocity(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
    void updatedRotationAcceleration(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);

private:
    void setupLineSeries(ACN::OTP::MODULES::STANDARD::VALUES::moduleValue_t, QString, QHBoxLayout&);

    QChartView *chartView;
    QMap<ACN::OTP::MODULES::STANDARD::VALUES::moduleValue_t, QLineSeries*> lineSeries[ACN::OTP::axis_t::count];
    std::pair<int, int> yRange;

    QButtonGroup *buttonGroup;

    std::shared_ptr<class ACN::OTP::Consumer> otpConsumer;
    ACN::OTP::address_t address;
};

#endif // LINECHART_H
