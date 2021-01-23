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
#include "componentsmodel.h"

using namespace OTP;

ComponentsItem::ComponentsItem(
        std::shared_ptr<class Consumer> otpConsumer,
        cid_t CID,
        itemType_t type,
        ComponentsItem *parentItem)
    : QObject(parentItem), otpConsumer(otpConsumer), CID(CID), type(type)
{
    if (type == ComponentFirst)
    {
        auto firstType = ComponentFirst + 1;
        childItems.resize((ComponentLast - firstType) + 1);
        for (auto newType = firstType; newType <= ComponentLast; newType++)
        {
            auto childItem = new ComponentsItem(
                            this->otpConsumer,
                            CID,
                            static_cast<itemType_t>(newType),
                            this);
            connect(childItem, &ComponentsItem::dataChanged, this, &ComponentsItem::dataChanged);
            connect(childItem, &ComponentsItem::layoutChanged, this, &ComponentsItem::layoutChanged);
            childItems[newType - firstType] = childItem;
        }
    }

    switch (type)
    {
        case ComponentRootItem: break;
        case ComponentSystemListItem: break;
        case ComponentCID: break;
        case ComponentName:
        {
            connect(otpConsumer.get(), qOverload<const cid_t&, const name_t&>(&Consumer::updatedComponent),
                    this, &ComponentsItem::dataChanged);
        } break;

        case ComponentType:
        {
            connect(otpConsumer.get(), qOverload<const cid_t&, component_t::type_t>(&Consumer::updatedComponent),
                    this, &ComponentsItem::dataChanged);
        } break;

        case ComponentIP:
        {
            connect(otpConsumer.get(), qOverload<const cid_t&, const QHostAddress&>(&Consumer::updatedComponent),
                    this, &ComponentsItem::dataChanged);
        } break;

        case ComponentSystemList:
        {
            connect(otpConsumer.get(), &Consumer::newSystem, [this](cid_t cid)
            {
                if (cid == this->CID)
                {
                    if (this->otpConsumer->getComponent(cid).getType() == component_s::consumer)
                        return; // Consumers don't send system lists

                    this->childItems.append(
                                new ComponentsItem(this->otpConsumer, this->CID, ComponentSystemListItem, this));
                    emit layoutChanged();
                }
            });
            connect(otpConsumer.get(), &Consumer::removedSystem, [this, CID](cid_t cid)
            {
                if (cid == CID)
                {
                    if (childItems.size()) {
                        this->childItems.resize(childItems.size() - 1);
                        emit layoutChanged();
                    }
                }
            });
        } break;
    }
}

void ComponentsItem::appendChild(ComponentsItem *item)
{
    childItems.append(item);
}

ComponentsItem *ComponentsItem::child(int row)
{
    if (row < 0 || row > childCount())
        return nullptr;
    return childItems.at(row);
}

int ComponentsItem::childCount() const
{
    return childItems.count();
}

QVariant ComponentsItem::data(int column) const
{
    if (column < 0 || column >= columnCount())
        return QVariant();
    switch (type)
    {
        case ComponentRootItem:
            return QString("Components");
        case ComponentCID:
            return QString("%1").arg(CID.toString());
        case ComponentName:
            return QString("Name: %1").arg(otpConsumer->getComponent(CID).getName().toString());
        case ComponentIP:
            return QString("IP: %1").arg(otpConsumer->getComponent(CID).getIPAddr().toString());
        case ComponentType:
            return QString("Type: %1").arg(
                        otpConsumer->getComponent(CID).getType() == component_s::consumer ? QString("Consumer") : QString("Producer"));
        case ComponentSystemList:
            if (otpConsumer->getComponent(CID).getType() == component_s::consumer)
                return "Systems: N/A";
            else
                return QString("Systems%1").arg(otpConsumer->getSystems(CID).isEmpty() ? ": None" : "");
        case ComponentSystemListItem:
            return QString::number(otpConsumer->getSystems(CID).value(row()));
        default:
            return QString("TODO");
    }
}

ComponentsItem *ComponentsItem::parentItem() const
{
    return qobject_cast<ComponentsItem*>(parent());
}

int ComponentsItem::row() const
{
    if (parentItem())
        return parentItem()->childItems.indexOf(const_cast<ComponentsItem*>(this));

    return 0;
}

ComponentsModel::ComponentsModel(
        std::shared_ptr<class Consumer> otpConsumer,
        QObject *parent)
    : QAbstractItemModel(parent),
      otpConsumer(otpConsumer)
{
    rootItem = new ComponentsItem(otpConsumer);

    // New Components
    connect(otpConsumer.get(), &Consumer::newComponent, this, &ComponentsModel::newComponent);

    // Existing Components
    for (const auto &cid : otpConsumer->getComponents())
        emit otpConsumer.get()->newComponent(cid);
}

void ComponentsModel::newComponent(OTP::cid_t cid)
{
    auto newItem = new ComponentsItem(
                this->otpConsumer,
                cid,
                ComponentsItem::ComponentFirst,
                rootItem);
    connect(newItem, &ComponentsItem::dataChanged, [this]() {
        emit dataChanged(QModelIndex(),QModelIndex());
    });
    connect(newItem, &ComponentsItem::layoutChanged, [this]() {
        emit layoutChanged();
    });
    beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
    rootItem->appendChild(newItem);
    endInsertRows();
}

QVariant ComponentsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    ComponentsItem *item = static_cast<ComponentsItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags ComponentsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant ComponentsModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex ComponentsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ComponentsItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ComponentsItem*>(parent.internalPointer());

    ComponentsItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex ComponentsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ComponentsItem *childItem = static_cast<ComponentsItem*>(index.internalPointer());
    ComponentsItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ComponentsModel::rowCount(const QModelIndex &parent) const
{
    ComponentsItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ComponentsItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int ComponentsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ComponentsItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}
