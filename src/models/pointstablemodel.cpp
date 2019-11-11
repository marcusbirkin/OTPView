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

#include "pointstablemodel.h"
#include <QDebug>

using namespace ACN::OTP;

PointsTableModel::PointsTableModel(
        std::shared_ptr<class ACN::OTP::Producer> otpProducer,
        ACN::OTP::system_t system,
        ACN::OTP::group_t group,
        QObject *parent) : QAbstractTableModel(parent),
    otpProducer(otpProducer),
    system(system),
    group(group)
{
    connect(otpProducer.get(), &Producer::newPoint, [this]() { emit layoutChanged(); });
    connect(otpProducer.get(), &Producer::removedPoint, [this]() { emit layoutChanged(); });
}

ACN::OTP::address_t PointsTableModel::getAddress(const QModelIndex &index) const
{
    if (index.row() > rowCount(index)) return address_t();
    auto pointList = otpProducer->getProducerPoints(system, group);
    std::sort(pointList.begin(), pointList.end());
    return address_t(system, group, pointList.at(index.row()));
}

int PointsTableModel::rowCount(const QModelIndex & /*parent*/) const
{
    return otpProducer->getProducerPoints(system, group).count();
}

int PointsTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QVariant PointsTableModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > rowCount(index)) return QVariant();
    if (role == Qt::DisplayRole)
    {
        return QString("%1/%2/%3")
                .arg(system)
                .arg(group)
                .arg(getAddress(index).point);
    }
    return QVariant();
}

QVariant PointsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Address");
            }
        }
    }
    return QVariant();
}
