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
#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <memory>
#include "libs/OTPLib/otp.hpp"

namespace Ui {
class GroupWindow;
}

class GroupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GroupWindow(
            std::shared_ptr<class ACN::OTP::Producer> otpProducer,
            ACN::OTP::system_t system,
            ACN::OTP::group_t group,
            QWidget *parent = nullptr);
    ~GroupWindow();

public slots:
    void setSystem(ACN::OTP::system_t newSystem);

private slots:
    void on_pbAddPoint_clicked();
    void on_pbRemovePoint_clicked();

    void newPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);
    void removedPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);

    void on_tablePoints_itemSelectionChanged();

    void on_leName_textChanged(const QString &arg1);

private:
    Ui::GroupWindow *ui;

    std::shared_ptr<class ACN::OTP::Producer> otpProducer;
    ACN::OTP::system_t system;
    ACN::OTP::group_t group;

    QStringList Axes {QStringLiteral("X"),QStringLiteral("Y"),QStringLiteral("Z")};

    enum PointsDetailsHeaders_e
    {
        Value,
        Velocity,
        Acceleration
    };
    QStringList PointsDetailsHeaders = {
        tr("Value"),
        tr("Velocity"),
        tr("Acceleration")
    };

    ACN::OTP::address_t getSelectedAddress();
};

#endif // GROUPWINDOW_H
