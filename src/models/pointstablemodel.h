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
#ifndef POINTSTABLEMODEL_H
#define POINTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "libs/OTPLib/otp.hpp"

class PointsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PointsTableModel(
            std::shared_ptr<class ACN::OTP::Producer> otpProducer,
            ACN::OTP::system_t system,
            ACN::OTP::group_t group,
            QObject *parent);
    void setGroup(ACN::OTP::group_t value) { group = value; }
    void setSystem(ACN::OTP::system_t value) { system = value; }

    ACN::OTP::address_t getAddress(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    std::shared_ptr<class ACN::OTP::Producer> otpProducer;
    ACN::OTP::system_t system;
    ACN::OTP::group_t group;
};

#endif // POINTSTABLEMODEL_H
