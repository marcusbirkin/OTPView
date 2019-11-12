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
#include "systemmodel.h"
#include "settings.h"

using namespace ACN::OTP;

static QFont italic()
{
    QFont font;
    font.setItalic(true);
    return font;
}

SystemItem::SystemItem(
        std::shared_ptr<class Consumer> otpConsumer,
        address_t address,
        itemType_t type,
        SystemItem *parentItem)
    : QObject(parentItem), otpConsumer(otpConsumer), address(address), type(type)
{}

void SystemItem::insertChild(int key, SystemItem *item)
{
    childItems.insert(key, item);
}

void SystemItem::removeChild(int key)
{
    childItems.remove(key);
}

bool SystemItem::containsChildKey(int key)
{
    return childItems.contains(key);
}

SystemItem *SystemItem::child(int row)
{
    if (row < 0 || row >= childCount())
        return nullptr;
    return childItems.values().at(row);
}

int SystemItem::childCount() const
{
    return childItems.count();
}

int SystemItem::columnCount() const
{
    return columnLast + 1;
}

int getAxis(SystemItem::itemType_t type)
{
    switch (type)
    {
        case SystemItem::SystemPointAxis_X: return axis_t::X;
        case SystemItem::SystemPointAxis_Y: return axis_t::Y;
        case SystemItem::SystemPointAxis_Z:  return axis_t::Z;
        default: return -1;
    }
}

QString SystemItem::getDataPositionRotationString() const
{
    auto resolution = Settings::getInstance().getSourceResolution();

    auto axis = getAxis(this->getType());
    if (axis == -1) axis = getAxis(this->parentItem()->getType());
    if (axis == -1) return QString("???");

    auto position = otpConsumer->getPosition(getAddress(), axis_t(axis), resolution);
    auto positionVelocity = otpConsumer->getPositionVelocity(getAddress(), axis_t(axis), resolution);
    auto positionAccel = otpConsumer->getPositionAcceleration(getAddress(), axis_t(axis), resolution);
    auto rotation = otpConsumer->getRotation(getAddress(), axis_t(axis), resolution);
    auto rotationVelocity = otpConsumer->getRotationVelocity(getAddress(), axis_t(axis), resolution);
    auto rotationAccel = otpConsumer->getRotationAcceleration(getAddress(), axis_t(axis), resolution);

    switch (type)
    {
        case SystemPointAxisDetails_Source:
        {
            switch (this->parentItem()->parentItem()->getType())
            {
                case SystemPointPositionValueItem: return position.sourceCID.toString();
                case SystemPointPositionVelcocityItem: return positionVelocity.sourceCID.toString();
                case SystemPointPositionAccelItem: return positionAccel.sourceCID.toString();
                case SystemPointRotationValueItem: return rotation.sourceCID.toString();
                case SystemPointRotationVelcocityItem: return rotationVelocity.sourceCID.toString();
                case SystemPointRotationAccelItem:return rotationAccel.sourceCID.toString();
                default: return QString("???");
            }
        }

        case SystemPointAxisDetails_Timestamp:
        {
            switch (this->parentItem()->parentItem()->getType())
            {
                case SystemPointPositionValueItem: return position.timestamp;
                case SystemPointPositionVelcocityItem: return positionVelocity.timestamp;
                case SystemPointPositionAccelItem: return positionAccel.timestamp;
                case SystemPointRotationValueItem: return rotation.timestamp;
                case SystemPointRotationVelcocityItem: return rotationVelocity.timestamp;
                case SystemPointRotationAccelItem:return rotationAccel.timestamp;
                default: return QString("???");
            }
        }

        case SystemPointAxis_X:
        case SystemPointAxis_Y:
        case SystemPointAxis_Z:
        {
            switch (this->parentItem()->getType())
            {
                case SystemPointPositionValueItem:
                    return QString("%1 %2")
                        .arg(position.value)
                        .arg(position.unit);

                case SystemPointPositionVelcocityItem:
                    return QString("%1 %2")
                        .arg(positionVelocity.value)
                        .arg(positionVelocity.unit);

                case SystemPointPositionAccelItem:
                    return QString("%1 %2")
                            .arg(positionAccel.value)
                            .arg(positionAccel.unit);

                case SystemPointRotationValueItem:
                    return QString("%1 %2")
                            .arg(rotation.value)
                            .arg(rotation.unit);

                case SystemPointRotationVelcocityItem:
                    return QString("%1 %2")
                            .arg(rotationVelocity.value)
                            .arg(rotationVelocity.unit);

                case SystemPointRotationAccelItem:
                    return QString("%1 %2")
                            .arg(rotationAccel.value)
                            .arg(rotationAccel.unit);

                default: return QString("???");
            }
        }

        default: return QString("???");
    }
}

QVariant SystemItem::data(int column, int roll) const
{
    if (column < 0 || column >= columnCount())
        return QVariant();
    switch (type)
    {
        case SystemRootItem:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("");
                    case columnDetails: return QString("");
                    default: return QString("???");
                }
            return QVariant();

        // Group
        case SystemGroupItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Group %1").arg(getGroup());
            if (otpConsumer->isGroupExpired(getSystem(), getGroup()))
            {
                if (roll == Qt::DisplayRole && column == columnDetails) return QString("(Expired)");
                if (roll == Qt::FontRole) return italic();
            }
            return QVariant();

        // Point
        case SystemPointItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Point %1").arg(getPoint());
            if (otpConsumer->isPointExpired(getSystem(), getGroup(), getPoint()))
            {
                if (roll == Qt::DisplayRole && column == columnDetails) return QString("(Expired)");
                if (roll == Qt::FontRole) return italic();
            }
            return QVariant();

        // Point Details
        case SystemPointDetailsItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Details");
            if (otpConsumer->isPointExpired(getSystem(), getGroup(), getPoint()))
            {
                if (roll == Qt::DisplayRole && column == columnDetails) return QString("(Expired)");
                if (roll == Qt::FontRole) return italic();
            }
            return QVariant();
        case SystemPointDetailsLastSeenItem:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Last Seen");
                    case columnDetails: return otpConsumer->getPointLastSeen(getAddress()).toString(Qt::DateFormat::ISODateWithMs);
                    default: return QString("???");
                }
            if (otpConsumer->isPointExpired(getSystem(), getGroup(), getPoint()))
            {
                if (roll == Qt::FontRole) return italic();
                if (roll == Qt::BackgroundRole) return QColor(Qt::red);
            }
            return QVariant();
        case SystemPointDetailsNameItem:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Name");
                    case columnDetails: return otpConsumer->getPointName(getAddress());
                    default: return QString("???");
                }
            if (otpConsumer->isPointExpired(getSystem(), getGroup(), getPoint()))
            {
                if (roll == Qt::FontRole) return italic();
            }
            return QVariant();

        // Point Position/Rotation
        case SystemPointPositionItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Position");
            return QVariant();
        case SystemPointRotationItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Rotation");
            return QVariant();
        case SystemPointPositionValueItem:
        case SystemPointRotationValueItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Value");
            return QVariant();
        case SystemPointPositionVelcocityItem:
        case SystemPointRotationVelcocityItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Velcocity");
            return QVariant();
        case SystemPointPositionAccelItem:
        case SystemPointRotationAccelItem:
            if (roll == Qt::DisplayRole && column == columnFirst) return QString("Acceleration");
            return QVariant();
        return QVariant();

        case SystemPointAxis_X:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("X");
                    case columnDetails: return getDataPositionRotationString();
                    default: return QString("???");
                }
            return QVariant();
        case SystemPointAxis_Y:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Y");
                    case columnDetails: return getDataPositionRotationString();
                    default: return QString("???");
                }
            return QVariant();
        case SystemPointAxis_Z:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Z");
                    case columnDetails: return getDataPositionRotationString();
                    default: return QString("???");
                }
            return QVariant();

        case SystemPointAxisDetails_Source:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Source");
                    case columnDetails: return getDataPositionRotationString();
                    default: return QString("???");
                }
            return QVariant();

        case SystemPointAxisDetails_Timestamp:
            if (roll == Qt::DisplayRole)
                switch (column)
                {
                    case columnFirst: return QString("Timestamp");
                    case columnDetails: return getDataPositionRotationString();
                    default: return QString("???");
                }
            return QVariant();
    }

    return QVariant();
}

SystemItem *SystemItem::parentItem() const
{
    return qobject_cast<SystemItem*>(parent());
}

int SystemItem::row() const
{
    if (parentItem())
        return parentItem()->childItems.values().indexOf(const_cast<SystemItem*>(this));

    return 0;
}

SystemModel::SystemModel(
        std::shared_ptr<class Consumer> otpConsumer,
        system_t system,
        QObject *parent)
    : QAbstractItemModel(parent),
      otpConsumer(otpConsumer)
{
    rootItem = new SystemItem(otpConsumer, address_t(system, group_t(), point_t()));

    /* Groups */
    // On new/removed groups
    connect(otpConsumer.get(), &Consumer::newGroup, this, &SystemModel::newGroup);
    connect(otpConsumer.get(), &Consumer::removedGroup, this, &SystemModel::removedGroup);

    // Add existing
    for (auto group : otpConsumer->getGroups(system))
        newGroup(cid_t(), system, group);

    /* Points */
    // On new/removed/updated points
    connect(otpConsumer.get(), &Consumer::newPoint, this, &SystemModel::newPoint);
    connect(otpConsumer.get(), &Consumer::removedPoint, this, &SystemModel::removedPoint);
    connect(otpConsumer.get(), &Consumer::updatedPoint, this, &SystemModel::updatedPoint);

    // Add existing
    for (auto group : otpConsumer->getGroups(system))
        for (auto point : otpConsumer->getPoints(system, group))
            newPoint(cid_t(), system, group, point);
}

SystemModel::~SystemModel()
{
    rootItem->deleteLater();
}

QVariant SystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    SystemItem *item = static_cast<SystemItem*>(index.internalPointer());

    return item->data(index.column(), role);
}

Qt::ItemFlags SystemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant SystemModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex SystemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SystemItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SystemItem*>(parent.internalPointer());

    SystemItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SystemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SystemItem *childItem = static_cast<SystemItem*>(index.internalPointer());
    SystemItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SystemModel::rowCount(const QModelIndex &parent) const
{
    SystemItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SystemItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SystemModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<SystemItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QModelIndex SystemModel::index(address_t address, SystemItem::itemType_t type) const
{
    auto system = address.system;
    auto group = address.group;
    auto point = address.point;

    switch (type) {
        case SystemItem::SystemRootItem:
        {
            if (rootItem->getSystem() == system)
                return index(0,0);
        } break;

        case SystemItem::SystemGroupItem:
        {
            auto parentIndex = index(address, SystemItem::SystemRootItem);
            auto parentItem = item(address, SystemItem::SystemRootItem);
            if (!parentIndex.isValid() || !parentItem) break;
            for (int row = 0; row < parentItem->childCount(); ++row)
                if (parentItem->child(row)->getGroup() == group)
                    return parentIndex.siblingAtRow(row);
        } break;

        case SystemItem::SystemPointItem:
        {
            auto parentIndex = index(address, SystemItem::SystemGroupItem);
            auto parentItem = item(address, SystemItem::SystemGroupItem);
            if (!parentIndex.isValid() || !parentItem) break;
            for (int row = 0; row < parentItem->childCount(); ++row)
            if (parentItem->child(row)->getPoint() == point)
                return parentIndex.siblingAtRow(row);
        } break;

        default: break;
    }

    return QModelIndex();
}

SystemItem *SystemModel::item(address_t address, SystemItem::itemType_t type) const
{
    auto system = address.system;
    auto group = address.group;
    auto point = address.point;

    switch (type) {
        case SystemItem::SystemRootItem:
        {
            if (rootItem->getSystem() == system)
                return rootItem;
        } break;

        case SystemItem::SystemGroupItem:
        {
            auto parentItem = item(address, SystemItem::SystemRootItem);
            if (!parentItem) break;
            for (int row = 0; row < parentItem->childCount(); ++row)
                if (parentItem->child(row)->getGroup() == group)
                    return parentItem->child(row);
        } break;

        case SystemItem::SystemPointItem:
        {
            auto parentItem = item(address, SystemItem::SystemGroupItem);
            if (!parentItem) break;
            for (int row = 0; row < parentItem->childCount(); ++row)
                if (parentItem->child(row)->getPoint() == point)
                    return parentItem->child(row);
        } break;

        default: break;
    }

    return nullptr;
}

void SystemModel::newGroup(cid_t cid, system_t system, group_t group)
{
    Q_UNUSED(cid);
    auto address = address_t(system, group, point_t());
    auto rootItem = item(address, SystemItem::SystemRootItem);
    if (!rootItem) return;
    if (system != rootItem->getSystem()) return;

    if (rootItem->containsChildKey(group)) return;
    auto groupItem = new SystemItem(this->otpConsumer, address_t(system, group, point_t()), SystemItem::SystemGroupItem, rootItem);
    auto newRow = rootItem->childCount() + 1;
    beginInsertRows(QModelIndex(), newRow, newRow);
    rootItem->insertChild(group, groupItem);
    endInsertRows();
}

void SystemModel::removedGroup(cid_t cid, system_t system, group_t group)
{
    Q_UNUSED(cid);
    auto address = address_t(system, group, point_t());
    auto rootItem = item(address, SystemItem::SystemRootItem);
    if (!rootItem) return;
    if (system != rootItem->getSystem()) return;
    for (auto row = 0; row < rootItem->childCount(); ++row)
        if (item(address, SystemItem::SystemRootItem)->child(row)->getGroup() == group)
        {
            beginRemoveRows(QModelIndex(), row, row);
            rootItem->removeChild(row);
            endRemoveRows();
        }
}

void SystemModel::newPoint(cid_t cid, system_t system, group_t group, point_t point)
{
    Q_UNUSED(cid);
    auto address = address_t(system, group, point);
    auto rootItem = item(address, SystemItem::SystemRootItem);
    if (!rootItem) return;
    if (system != rootItem->getSystem()) return;
    auto groupItem = item(address, SystemItem::SystemGroupItem);
    if (!groupItem) return;
    auto groupIndex = index(address, SystemItem::SystemGroupItem);
    if (!groupIndex.isValid()) return;

    if (groupItem->containsChildKey(point)) return;
    auto pointItem = new SystemItem(this->otpConsumer, address_t(system, group, point), SystemItem::SystemPointItem, groupItem);
    newPointDetails(pointItem);
    newPointPosition(pointItem);
    newPointRotation(pointItem);

    auto newRow = groupItem->childCount() + 1;
    beginInsertRows(groupIndex, newRow, newRow);
    groupItem->insertChild(point, pointItem);
    endInsertRows();
}

void SystemModel::removedPoint(cid_t cid, system_t system, group_t group, point_t point)
{
    Q_UNUSED(cid);
    auto address = address_t(system, group, point);
    auto rootItem = item(address, SystemItem::SystemRootItem);
    if (!rootItem) return;
    if (system != rootItem->getSystem()) return;
    auto groupItem = item(address, SystemItem::SystemGroupItem);
    if (!groupItem) return;
    auto groupIndex = index(address, SystemItem::SystemGroupItem);
    if (!groupIndex.isValid()) return;

    for (auto row = 0; row < groupItem->childCount(); ++row)
        if (groupItem->child(row)->getPoint() == point)
        {
            beginRemoveRows(groupIndex, row, row);
            groupItem->removeChild(row);
            endRemoveRows();
        }
}

void SystemModel::updatedPoint(cid_t cid, system_t system, group_t group, point_t point)
{
    Q_UNUSED(cid)
    auto address = address_t(system, group, point);
    auto rootItem = item(address, SystemItem::SystemRootItem);
    if (!rootItem) return;
    if (system != rootItem->getSystem()) return;
    auto startIndex = index(address, SystemItem::SystemPointItem);
    if (!startIndex.isValid()) return;
    auto startItem = item(address, SystemItem::SystemPointItem);
    if (!startItem) return;
    auto endIndex = startIndex.siblingAtRow(startItem->childCount() - 1);
    emit dataChanged(startIndex, endIndex);
}

void SystemModel::newPointDetails(SystemItem *parent)
{
    auto pointDetailsItem =
            new SystemItem(this->otpConsumer, parent->getAddress(), SystemItem::SystemPointDetailsItem, parent);
    parent->insertChild(SystemItem::SystemPointDetailsItem, pointDetailsItem);

    // Add Details
    for (int type = SystemItem::SystemPointDetails_Frist; type <= SystemItem::SystemPointDetails_Last; type++)
    {
        auto detailItem =
                new SystemItem(this->otpConsumer,  parent->getAddress(), static_cast<SystemItem::itemType_t>(type), pointDetailsItem);
        pointDetailsItem->insertChild(type, detailItem);
    }
}

void SystemModel::newPointPosition(SystemItem *parent)
{
    auto pointPositionItem =
            new SystemItem(this->otpConsumer, parent->getAddress(), SystemItem::SystemPointPositionItem, parent);
    parent->insertChild(SystemItem::SystemPointPositionItem, pointPositionItem);

    // Add Position types
    for (int type = SystemItem::SystemPointPosition_First; type <= SystemItem::SystemPointPosition_Last; type++)
    {
        auto PositionItem =
                new SystemItem(this->otpConsumer, parent->getAddress(), static_cast<SystemItem::itemType_t>(type), pointPositionItem);
        pointPositionItem->insertChild(type, PositionItem);

        // Axis
        for (int axisType = SystemItem::SystemPointAxis_First; axisType <= SystemItem::SystemPointAxis_Last; axisType++)
        {
            auto AxisItem =
                    new SystemItem(this->otpConsumer,  parent->getAddress(), static_cast<SystemItem::itemType_t>(axisType), PositionItem);
            PositionItem->insertChild(axisType, AxisItem);

            // Axis Details
            for (int axisType = SystemItem::SystemPointAxisDetails_First; axisType <= SystemItem::SystemPointAxisDetails_Last; axisType++)
            {
                auto AxisDetailItem =
                        new SystemItem(this->otpConsumer,  parent->getAddress(), static_cast<SystemItem::itemType_t>(axisType), AxisItem);
                AxisItem->insertChild(axisType, AxisDetailItem);
            }
        }
    }
}

void SystemModel::newPointRotation(SystemItem *parent)
{
    auto pointRotationItem =
            new SystemItem(this->otpConsumer, parent->getAddress(), SystemItem::SystemPointRotationItem, parent);
    parent->insertChild(SystemItem::SystemPointRotationItem, pointRotationItem);

    // Add Rotation types
    for (int type = SystemItem::SystemPointRotation_First; type <= SystemItem::SystemPointRotation_Last; type++)
    {
        auto RotationItem =
                new SystemItem(this->otpConsumer, parent->getAddress(), static_cast<SystemItem::itemType_t>(type), pointRotationItem);
        pointRotationItem->insertChild(type, RotationItem);

        // Axis
        for (int axisType = SystemItem::SystemPointAxis_First; axisType <= SystemItem::SystemPointAxis_Last; axisType++)
        {
            auto AxisItem =
                    new SystemItem(this->otpConsumer,  parent->getAddress(), static_cast<SystemItem::itemType_t>(axisType), RotationItem);
            RotationItem->insertChild(axisType, AxisItem);

            // Axis Details
            for (int axisType = SystemItem::SystemPointAxisDetails_First; axisType <= SystemItem::SystemPointAxisDetails_Last; axisType++)
            {
                auto AxisDetailItem =
                        new SystemItem(this->otpConsumer,  parent->getAddress(), static_cast<SystemItem::itemType_t>(axisType), AxisItem);
                AxisItem->insertChild(axisType, AxisDetailItem);
            }
        }
    }
}
