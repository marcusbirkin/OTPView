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
        SystemPointDetailsReferenceFrameItem,
        SystemPointDetails_Last = SystemPointDetailsReferenceFrameItem,

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
        SystemPointRotationValueItem = SystemPointRotation_First,
        SystemPointRotationVelcocityItem,
        SystemPointRotationAccelItem,
        SystemPointRotation_Last = SystemPointRotationAccelItem,

        // Point Scale
        SystemPointScaleItem,

        // Axis
        SystemPointAxis_First,
        SystemPointAxis_X = SystemPointAxis_First,
        SystemPointAxis_Y,
        SystemPointAxis_Z,
        SystemPointAxis_Last = SystemPointAxis_Z,

        // Axis details
        SystemPointAxisDetails_First,
        SystemPointAxisDetails_Source = SystemPointAxisDetails_First,
        SystemPointAxisDetails_Priority,
        SystemPointAxisDetails_Timestamp,
        SystemPointAxisDetails_Last = SystemPointAxisDetails_Timestamp,
    } itemType_t;

    explicit SystemItem(std::shared_ptr<class OTP::Consumer> otpConsumer,
            OTP::address_t address = OTP::address_t(),
            itemType_t type = SystemRootItem,
            SystemItem *parentItem = nullptr);

    typedef unsigned long key_t;

    void insertChild(key_t key, SystemItem *item);
    void removeChild(key_t key);
    bool containsChildKey(key_t key);
    SystemItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column = 0, int roll = Qt::DisplayRole) const;
    int row() const;
    SystemItem *parentItem() const;
    static SystemItem* indexToItem(QModelIndex index)
    {
        return  static_cast<SystemItem*>(index.internalPointer());
    }

    itemType_t getType() const { return type; }

    OTP::system_t getSystem() const { return address.system; }
    OTP::group_t getGroup() const { return address.group; }
    OTP::point_t getPoint() const { return address.point; }
    OTP::address_t getAddress() const
        { return OTP::address_t(getSystem(), getGroup(), getPoint()); }

private:
    QString getDataPositionRotationString() const;

    std::shared_ptr<class OTP::Consumer> otpConsumer;
    OTP::address_t address;
    itemType_t type;

    QMap<key_t, SystemItem*> childItems; // SortIndex, SystemItem*

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
            std::shared_ptr<class OTP::Consumer> otpConsumer,
            OTP::system_t system,
            QObject *parent = nullptr);
     ~SystemModel() override;

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
    void newGroup(OTP::cid_t, OTP::system_t, OTP::group_t);
    void removedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

    void newPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
    void removedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
    void updatedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

    void newPointDetails(SystemItem *parent);
    void newPointPosition(SystemItem *parent);
    void newPointRotation(SystemItem *parent);
    void newPointScale(SystemItem *parent);

private:
    QModelIndex index(OTP::address_t, SystemItem::itemType_t) const;
    SystemItem *item(OTP::address_t, SystemItem::itemType_t) const;

    std::shared_ptr<class OTP::Consumer> otpConsumer;
    SystemItem *rootItem;
};

#endif // SYSTEMMODEL_H
