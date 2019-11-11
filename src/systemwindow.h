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
#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "libs/OTPLib/otp.hpp"

namespace Ui {
class SystemWindow;
}

class SystemWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SystemWindow(
            std::shared_ptr<class ACN::OTP::Consumer> otpConsumer,
            ACN::OTP::system_t system,
            QWidget *parent = nullptr);
    ~SystemWindow();

    ACN::OTP::system_t getSystem() const { return system; }

//public slots:
//    void newGroup(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t);
//    void newPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);
//    void updatedPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);
//    void expiredPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);

//    void updatedPosition(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedPositionVelocity(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedPositionAcceleration(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedRotation(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedRotationVelocity(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedRotationAcceleration(ACN::OTP::cid_t, ACN::OTP::address_t, ACN::OTP::axis_t);
//    void updatedName(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t, ACN::OTP::name_t);
//    void updatedLastSeen(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t, QDateTime);

private:
    Ui::SystemWindow *ui;

    std::shared_ptr<class ACN::OTP::Consumer> otpConsumer;
    ACN::OTP::system_t system;

//    enum PointsHeaders_e
//    {
//        Group,
//        Point,
//        Position,
//        Rotation,
//        Details
//    };
//    QStringList PointsHeaders = {
//        QString("Group %1"),
//        QString("Point %1"),
//        QString("Position"),
//        QString("Rotation"),
//        QString("Details"),
//    };

//    enum PointsPosRotDetailHeaders_e
//    {
//        Value,
//        Velocity,
//        Acceleration
//    };
//    QStringList PointsPosRotDetailHeaders = {
//        tr("Value"),
//        tr("Velocity"),
//        tr("Acceleration")
//    };

//    enum PointsDetailHeaders_e
//    {
//        Name,
//        LastSeen
//    };
//    QStringList PointsDetailHeaders = {
//        tr("Name"),
//        tr("Last Seen")
//    };

//    enum PointsColumns_e
//    {
//        Name_column,
//        Detail_column,
//        AxisX_column = Detail_column,
//        AxisY_column,
//        AxisZ_column,
//        Timestamp_column
//    };

//    QTreeWidgetItem* getGroupItem(ACN::OTP::group_t);
//    QTreeWidgetItem* getPointItem(ACN::OTP::group_t, ACN::OTP::point_t);
//    QTreeWidgetItem* getPositionItem(ACN::OTP::group_t, ACN::OTP::point_t);
//    QTreeWidgetItem* getPositionDetailItem(ACN::OTP::group_t, ACN::OTP::point_t, PointsPosRotDetailHeaders_e);
//    QTreeWidgetItem* getRotationItem(ACN::OTP::group_t, ACN::OTP::point_t);
//    QTreeWidgetItem* getRotationDetailItem(ACN::OTP::group_t, ACN::OTP::point_t, PointsPosRotDetailHeaders_e);
//    QTreeWidgetItem* getDetailItem(ACN::OTP::group_t, ACN::OTP::point_t);
//    QTreeWidgetItem* getDetailDetailItem(ACN::OTP::group_t, ACN::OTP::point_t, PointsDetailHeaders_e);
};

#endif // SYSTEMWINDOW_H
