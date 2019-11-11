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
#include "systemwindow.h"
#include "ui_systemwindow.h"
#include <QTimer>
#include "models/systemmodel.h"

using namespace ACN::OTP;

SystemWindow::SystemWindow(
        std::shared_ptr<class ACN::OTP::Consumer> otpConsumer,
        ACN::OTP::system_t system,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemWindow),
    otpConsumer(otpConsumer),
    system(system)
{
    // Window
    ui->setupUi(this);
    this->setWindowTitle(
                QString(tr("System %1"))
                .arg(system));

    // Create system
    otpConsumer->addConsumerSystem(system);

    // Tree
    ui->tvDetails->setModel(new SystemModel(otpConsumer, system, this));
    ui->tvDetails->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Setup Updates
//    connect(otpConsumer.get(), &Consumer::newGroup, this, &SystemWindow::newGroup);
//    connect(otpConsumer.get(), &Consumer::newPoint, this, &SystemWindow::newPoint);
//    connect(otpConsumer.get(), &Consumer::updatedPoint, this, &SystemWindow::updatedPoint);
//    connect(otpConsumer.get(), &Consumer::expiredPoint, this, &SystemWindow::expiredPoint);
//    connect(otpConsumer.get(), &Consumer::updatedPosition, this, &SystemWindow::updatedPosition);
//    connect(otpConsumer.get(), &Consumer::updatedPositionVelocity, this, &SystemWindow::updatedPositionVelocity);
//    connect(otpConsumer.get(), &Consumer::updatedPositionAcceleration, this, &SystemWindow::updatedPositionAcceleration);
//    connect(otpConsumer.get(), &Consumer::updatedRotation, this, &SystemWindow::updatedRotation);
//    connect(otpConsumer.get(), &Consumer::updatedRotationVelocity, this, &SystemWindow::updatedRotationVelocity);
//    connect(otpConsumer.get(), &Consumer::updatedRotationAcceleration, this, &SystemWindow::updatedRotationAcceleration);

//    // Create currently known points
//    for (auto group : otpConsumer->getGroups(system))
//    {
//        newGroup(cid_t(), system, group);
//        for (auto point : otpConsumer->getPoints(system, group))
//            newPoint(cid_t(), system, group, point);
//    }

//    // Slow background refresh
//    auto refresh = new QTimer(this);
//    refresh->start(1000);
//    connect(refresh, &QTimer::timeout,
//        [this, otpConsumer, system]() {
//            if (!otpConsumer) return;
//            for (auto cid : otpConsumer->getComponents())
//            {
//                for (auto group : otpConsumer->getGroups(m_system))
//                {
//                    for (auto point : otpConsumer->getPoints(cid, m_system, group))
//                    {
//                         updatedPoint(cid, system, group, point);
//                         auto address = address_t(system, group, point);
//                         for (auto axis = axis_t::first; axis < axis_t::count; axis++)
//                         {
//                             updatedPosition(cid, address, axis);
//                             updatedPositionVelocity(cid, address, axis);
//                             updatedPositionAcceleration(cid, address, axis);
//                             updatedRotation(cid, address, axis);
//                             updatedRotationVelocity(cid, address, axis);
//                             updatedRotationAcceleration(cid, address, axis);
//                         }
//                    }
//                }
//            }
//        });
}

SystemWindow::~SystemWindow()
{
    otpConsumer->removeConsumerSystem(system);
    delete ui;
}

//QTreeWidgetItem* SystemWindow::getGroupItem(group_t group)
//{
//    auto groupItems = ui->twDetails->findItems(QString(PointsHeaders[PointsHeaders_e::Group]).arg(group), Qt::MatchFlag::MatchExactly, 0);
//    if (groupItems.isEmpty()) return nullptr;
//    return groupItems.first();
//}

//QTreeWidgetItem* SystemWindow::getPointItem(ACN::OTP::group_t group, ACN::OTP::point_t point)
//{
//    auto groupItem = getGroupItem(group);
//    if (!groupItem) return nullptr;

//    QTreeWidgetItem* pointItem = nullptr;
//    for (int idx = 0; idx < groupItem->childCount(); idx++)
//    {
//        if (groupItem->child(idx)->text(Name_column).contains(PointsHeaders[PointsHeaders_e::Point].arg(point)))
//        {
//            pointItem = groupItem->child(idx);
//            break;
//        }
//    }
//    return pointItem;
//}

//QTreeWidgetItem* SystemWindow::getPositionItem(group_t group, point_t point)
//{
//    auto pointItem = getPointItem(group, point);
//    if (!pointItem) return nullptr;

//    QTreeWidgetItem* positionItem = nullptr;
//    for (int idx = 0; idx < pointItem->childCount(); idx++)
//    {
//        if (pointItem->child(idx)->text(Name_column).contains(PointsHeaders[PointsHeaders_e::Position]))
//        {
//            positionItem = pointItem->child(idx);
//            break;
//        }
//    }
//    if (!positionItem)
//    {
//        positionItem = new QTreeWidgetItem(pointItem, QStringList(PointsHeaders[PointsHeaders_e::Position]));
//        for (auto string : PointsPosRotDetailHeaders)
//            new QTreeWidgetItem(positionItem, QStringList(string));
//    }

//    return positionItem;
//}

//QTreeWidgetItem* SystemWindow::getPositionDetailItem(group_t group, point_t point, PointsPosRotDetailHeaders_e detailType)
//{
//    auto positionItem = getPositionItem(group, point);
//    if (!positionItem) return nullptr;

//    QTreeWidgetItem* valueItem = nullptr;
//    for (int idx = 0; idx < positionItem->childCount(); idx++)
//    {
//        if (positionItem->child(idx)->text(Name_column).contains(PointsPosRotDetailHeaders[detailType]))
//        {
//            valueItem = positionItem->child(idx);
//            break;
//        }
//    }
//    return valueItem;
//}

//QTreeWidgetItem* SystemWindow::getRotationItem(group_t group, point_t point)
//{
//    auto pointItem = getPointItem(group, point);
//    if (!pointItem) return nullptr;

//    QTreeWidgetItem* rotationItem = nullptr;
//    for (int idx = 0; idx < pointItem->childCount(); idx++)
//    {
//        if (pointItem->child(idx)->text(Name_column).contains(PointsHeaders[PointsHeaders_e::Rotation]))
//        {
//            rotationItem = pointItem->child(idx);
//            break;
//        }
//    }
//    if (!rotationItem)
//    {
//        rotationItem = new QTreeWidgetItem(pointItem, QStringList(PointsHeaders[PointsHeaders_e::Rotation]));
//        for (auto string : PointsPosRotDetailHeaders)
//            new QTreeWidgetItem(rotationItem, QStringList(string));
//    }

//    return rotationItem;
//}

//QTreeWidgetItem* SystemWindow::getRotationDetailItem(group_t group, point_t point, PointsPosRotDetailHeaders_e detailType)
//{
//    auto rotation = getRotationItem(group, point);
//    if (!rotation) return nullptr;

//    QTreeWidgetItem* valueItem = nullptr;
//    for (int idx = 0; idx < rotation->childCount(); idx++)
//    {
//        if (rotation->child(idx)->text(Name_column).contains(PointsPosRotDetailHeaders[detailType]))
//        {
//            valueItem = rotation->child(idx);
//            break;
//        }
//    }
//    return valueItem;
//}

//QTreeWidgetItem* SystemWindow::getDetailItem(group_t group, point_t point)
//{
//    auto pointItem = getPointItem(group, point);
//    if (!pointItem) return nullptr;

//    QTreeWidgetItem* detailItem = nullptr;
//    for (int idx = 0; idx < pointItem->childCount(); idx++)
//    {
//        if (pointItem->child(idx)->text(Name_column).contains(PointsHeaders[PointsHeaders_e::Details]))
//        {
//            detailItem = pointItem->child(idx);
//            break;
//        }
//    }
//    if (!detailItem)
//    {
//        detailItem = new QTreeWidgetItem(pointItem, QStringList(PointsHeaders[PointsHeaders_e::Details]));
//        detailItem->setFirstColumnSpanned(true);
//        for (auto string : PointsDetailHeaders)
//            new QTreeWidgetItem(detailItem, QStringList(string));
//    }

//    return detailItem;
//}

//QTreeWidgetItem* SystemWindow::getDetailDetailItem(group_t group, point_t point, PointsDetailHeaders_e detailType)
//{
//    auto detail = getDetailItem(group, point);
//    if (!detail) return nullptr;

//    QTreeWidgetItem* valueItem = nullptr;
//    for (int idx = 0; idx < detail->childCount(); idx++)
//    {
//        if (detail->child(idx)->text(Name_column).contains(PointsDetailHeaders[detailType]))
//        {
//            valueItem = detail->child(idx);
//            break;
//        }
//    }
//    return valueItem;
//}

//void SystemWindow::newGroup(cid_t cid, system_t system, group_t group)
//{
//    Q_UNUSED(cid)
//    if (m_system != system) return;
//    if (getGroupItem(group)) return;
//    QTreeWidgetItem *groupItem = new QTreeWidgetItem(ui->twDetails, QStringList(PointsHeaders[PointsHeaders_e::Group].arg(group)));
//    groupItem->setFirstColumnSpanned(true);
//    Q_UNUSED(groupItem)
//}

//void SystemWindow::newPoint(cid_t cid, system_t system, group_t group, point_t point)
//{
//    Q_UNUSED(cid)
//    if (m_system != system) return;
//    if (getPointItem(group, point)) return;
//    auto groupItem = getGroupItem(group);
//    if (!groupItem) return;
//    QTreeWidgetItem *pointItem = new QTreeWidgetItem(groupItem, QStringList(PointsHeaders[PointsHeaders_e::Point].arg(point)));
//    pointItem->setFirstColumnSpanned(true);
//    Q_UNUSED(pointItem)
//}

//void SystemWindow::updatedPoint(cid_t cid, system_t system, group_t group, point_t point)
//{
//    updatedName(cid, system, group, point, otpConsumer->getPointName(system, group, point));
//    updatedLastSeen(cid, system, group, point, otpConsumer->getPointLastSeen(system, group, point));
//}

//void SystemWindow::expiredPoint(cid_t cid, system_t system, group_t group, point_t point)
//{
//    updatedLastSeen(cid, system, group, point, otpConsumer->getPointLastSeen(system, group, point));
//}

//void SystemWindow::updatedPosition(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto positionValueItem = getPositionDetailItem(address.group, address.point, Value);
//    if (positionValueItem)
//    {
//        positionValueItem->setText(
//                AxisX_column + axis,
//                QString("%1 %2")
//                    .arg(otpConsumer->getPositionLocation(cid, address, axis))
//                    .arg(otpConsumer->getPositionLocationUnit(cid, address)));
//        positionValueItem->setText(
//                Timestamp_column,
//                otpConsumer->getPositionTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedPositionVelocity(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto positionVelocityItem = getPositionDetailItem(address.group, address.point, Velocity);
//    if (positionVelocityItem)
//    {
//        positionVelocityItem->setText(
//                AxisX_column + axis,
//                    QString("%1 %2")
//                        .arg(otpConsumer->getPositionVelocity(cid, address, axis))
//                        .arg(otpConsumer->getPositionVelocitynUnit(cid, address)));
//        positionVelocityItem->setText(
//                Timestamp_column,
//                otpConsumer->getPositionVelocityTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedPositionAcceleration(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto positionAccelItem = getPositionDetailItem(address.group, address.point, Acceleration);
//    if (positionAccelItem)
//    {
//        positionAccelItem->setText(
//                AxisX_column + axis,
//                    QString("%1 %2")
//                        .arg(otpConsumer->getPositionAcceleration(cid, address, axis))
//                        .arg(otpConsumer->getPositionAccelerationUnit(cid, address)));
//        positionAccelItem->setText(
//                Timestamp_column,
//                otpConsumer->getPositionAccelerationTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedRotation(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto rotationValueItem = getRotationDetailItem(address.group, address.point, Value);
//    if (rotationValueItem)
//    {
//        rotationValueItem->setText(
//                AxisX_column + axis,
//                    QString("%1 %2")
//                        .arg(otpConsumer->getRotation(cid, address, axis))
//                        .arg(otpConsumer->getRotationUnit(cid, address)));
//        rotationValueItem->setText(
//                Timestamp_column,
//                otpConsumer->getRotationTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedRotationVelocity(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto rotationVelocityItem = getRotationDetailItem(address.group, address.point, Velocity);
//    if (rotationVelocityItem)
//    {
//        rotationVelocityItem->setText(
//                AxisX_column + axis,
//                    QString("%1 %2")
//                        .arg(otpConsumer->getRotationVelocity(cid, address, axis))
//                        .arg(otpConsumer->getRotationVelocityUnit(cid, address)));
//        rotationVelocityItem->setText(
//                Timestamp_column,
//                otpConsumer->getRotationVelocityTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedRotationAcceleration(cid_t cid, address_t address, axis_t axis)
//{
//    if (m_system != address.system) return;
//    auto rotationAccelItem = getRotationDetailItem(address.group, address.point, Acceleration);
//    if (rotationAccelItem)
//    {
//        rotationAccelItem->setText(
//                AxisX_column + axis,
//                    QString("%1 %2")
//                        .arg(otpConsumer->getRotationAcceleration(cid, address, axis))
//                        .arg(otpConsumer->getRotationAccelerationUnit(cid, address)));
//        rotationAccelItem->setText(
//                Timestamp_column,
//                otpConsumer->getRotationAccelerationTimestamp(cid, address));
//    }
//}

//void SystemWindow::updatedName(cid_t cid, system_t system, group_t group, point_t point, name_t name)
//{
//    Q_UNUSED(cid)
//    if (m_system != system) return;
//    auto rotationAccelItem = getDetailDetailItem(group, point, Name);
//    if (rotationAccelItem)
//        rotationAccelItem->setText(
//                    Detail_column,
//                    name);
//}

//void SystemWindow::updatedLastSeen(cid_t cid, system_t system, group_t group, point_t point, QDateTime lastseen)
//{
//    Q_UNUSED(cid)
//    if (m_system != system) return;
//    auto rotationAccelItem = getDetailDetailItem(group, point, LastSeen);
//    if (rotationAccelItem)
//    {
//        rotationAccelItem->setText(
//                    Detail_column,
//                    lastseen.toString(Qt::DateFormat::ISODateWithMs));

//        auto forground = (otpConsumer->isPointExpired(system, group, point)) ? QColor(Qt::red) : QBrush();
//        rotationAccelItem->setForeground(Detail_column, forground);
//    }

//    auto pointItem = getPointItem(group, point);
//    if (pointItem)
//    {
//        auto font = pointItem->font(Name_column);
//        font.setItalic(otpConsumer->isPointExpired(system, group, point));
//        pointItem->setFont(Name_column, font);
//    }

//    auto groupItem = getGroupItem(group);
//    if (groupItem)
//    {
//        auto font = groupItem->font(Name_column);
//        font.setItalic(otpConsumer->isGroupExpired(system, group));
//        groupItem->setFont(Name_column, font);
//    }

//    ui->twDetails->repaint();
//}
