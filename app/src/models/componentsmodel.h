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
#ifndef COMPONENTSMODEL_H
#define COMPONENTSMODEL_H
#include <QAbstractItemModel>
#include "OTPLib.hpp"

class ComponentsItem : public QObject
{
    Q_OBJECT

public:
    typedef enum itemType_e {
        ComponentRootItem,
        ComponentSystemListItem,
        ComponentModuleListItem,

        ComponentFirst,
        ComponentCID = ComponentFirst,
        ComponentName,
        ComponentType,
        ComponentIP,
        ComponentSystemList,
        ComponentModuleList,
        ComponentLast = ComponentModuleList
    } itemType_t;

    explicit ComponentsItem(
            std::shared_ptr<class OTP::Consumer> otpConsumer,
            OTP::cid_t CID = OTP::cid_t(),
            itemType_t type = ComponentRootItem,
            ComponentsItem *parentItem = nullptr);

    void appendChild(ComponentsItem *item);
    ComponentsItem *child(int row);
    int childCount() const;
    int columnCount() const  { return 1; }
    QVariant data(int column = 0) const;
    int row() const;
    ComponentsItem *parentItem() const;

    itemType_t getType() const { return type; };

signals:
    void dataChanged();
    void layoutChanged();

private:
    std::shared_ptr<class OTP::Consumer> otpConsumer;
    OTP::cid_t CID;
    itemType_t type;

    QVector<ComponentsItem*> childItems;
};

class ComponentsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ComponentsModel(
            std::shared_ptr<class OTP::Consumer> otpConsumer,
            QObject *parent = nullptr);

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
    void newComponent(OTP::cid_t cid);

private:
    std::shared_ptr<class OTP::Consumer> otpConsumer;
    ComponentsItem *rootItem;

};

#endif // COMPONENTSITEMMODEL_H
