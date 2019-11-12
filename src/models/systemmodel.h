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
#ifndef SYSTEMMODEL_H
#define SYSTEMMODEL_H
#include <QAbstractItemModel>
#include "libs/OTPLib/otp.hpp"

class SystemItem : public QObject
{
    Q_OBJECT

public:
    typedef enum itemType_e {
        // Root
        SystemRootItem,

        // Group
        SystemGroupItem,

        // Point
        SystemPointItem,

        // Point Details
        SystemPointDetailsItem,
        SystemPointDetails_Frist,
        SystemPointDetailsNameItem = SystemPointDetails_Frist,
        SystemPointDetailsLastSeenItem,
        SystemPointDetails_Last = SystemPointDetailsLastSeenItem,

        // Point Position
        SystemPointPositionItem,
        SystemPointPosition_First,
        SystemPointPositionValueItem = SystemPointPosition_First,
        SystemPointPositionVelcocityItem,
        SystemPointPositionAccelItem,
        SystemPointPosition_Last = SystemPointPositionAccelItem,


        // Point Rotation
        SystemPointRotationItem,
        SystemPointRotation_First,
        SystemPointRotationValueItem  = SystemPointRotation_First,
        SystemPointRotationVelcocityItem,
        SystemPointRotationAccelItem,
        SystemPointRotation_Last = SystemPointRotationAccelItem,

        // Axis
        SystemPointAxis_First,
        SystemPointAxis_X = SystemPointAxis_First,
        SystemPointAxis_Y,
        SystemPointAxis_Z,
        SystemPointAxis_Last = SystemPointAxis_Z,

        // Axis details
        SystemPointAxisDetails_First,
        SystemPointAxisDetails_Source = SystemPointAxisDetails_First,
        SystemPointAxisDetails_Timestamp,
        SystemPointAxisDetails_Last = SystemPointAxisDetails_Timestamp,
    } itemType_t;

    explicit SystemItem(std::shared_ptr<class ACN::OTP::Consumer> otpConsumer,
            ACN::OTP::address_t address = ACN::OTP::address_t(),
            itemType_t type = SystemRootItem,
            SystemItem *parentItem = nullptr);

    void insertChild(int key, SystemItem *item);
    void removeChild(int key);
    bool containsChildKey(int key);
    SystemItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column = 0, int roll = Qt::DisplayRole) const;
    int row() const;
    SystemItem *parentItem() const;

    itemType_t getType() const { return type; };

    ACN::OTP::system_t getSystem() const { return address.system; }
    ACN::OTP::group_t getGroup() const { return address.group; }
    ACN::OTP::point_t getPoint() const { return address.point; }
    ACN::OTP::address_t getAddress() const
        { return ACN::OTP::address_t(getSystem(), getGroup(), getPoint()); }

private:
    QString getDataPositionRotationString() const;

    std::shared_ptr<class ACN::OTP::Consumer> otpConsumer;
    ACN::OTP::address_t address;
    itemType_t type;

    QMap<int, SystemItem*> childItems; // SortIndex, SystemItem*

    enum column_e {
        columnFirst,
        columnDetails,

        columnLast = columnDetails
    };
};

class SystemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SystemModel(
            std::shared_ptr<class ACN::OTP::Consumer> otpConsumer,
            ACN::OTP::system_t system,
            QObject *parent = nullptr);
    ~SystemModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private slots:
    void newGroup(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t);
    void removedGroup(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t);

    void newPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);
    void removedPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);
    void updatedPoint(ACN::OTP::cid_t, ACN::OTP::system_t, ACN::OTP::group_t, ACN::OTP::point_t);

    void newPointDetails(SystemItem *parent);
    void newPointPosition(SystemItem *parent);
    void newPointRotation(SystemItem *parent);

private:
    QModelIndex index(ACN::OTP::address_t, SystemItem::itemType_t) const;
    SystemItem *item(ACN::OTP::address_t, SystemItem::itemType_t) const;

    std::shared_ptr<class ACN::OTP::Consumer> otpConsumer;
    SystemItem *rootItem;
};

#endif // SYSTEMMODEL_H