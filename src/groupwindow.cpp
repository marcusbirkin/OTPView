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
#include "groupwindow.h"
#include "ui_groupwindow.h"
#include "producerwindow.h"
#include "pointselectiondialog.h"
#include "widgets/spacialspinbox.h"

using namespace ACN::OTP;

GroupWindow::GroupWindow(
        std::shared_ptr<class ACN::OTP::Producer> otpProducer,
        ACN::OTP::system_t system,
        ACN::OTP::group_t group,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupWindow),
    otpProducer(otpProducer),
    system(system),
    group(group)
{
    // Window
    ui->setupUi(this);
    this->setWindowTitle(
                QString(tr("Group %1"))
                .arg(group));

    // Create group
    otpProducer->addProducerGroup(system, group);

    // Points
    ui->tablePoints->setColumnCount(1);
    ui->tablePoints->setHorizontalHeaderLabels(QStringList(tr("Point")));
    ui->tablePoints->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tablePoints->horizontalHeader()->setStretchLastSection(true);
    connect(otpProducer.get(), &Producer::newPoint, this, &GroupWindow::newPoint);
    connect(otpProducer.get(), &Producer::removedPoint, this, &GroupWindow::removedPoint);

    // Producer Points Details
    on_tablePoints_itemSelectionChanged();
    // - Name
    ui->leName->setMaxLength(name_t::maxSize());

    // - Position
    ui->tablePointsPosition->setColumnCount(PointsDetailsHeaders.count());
    ui->tablePointsPosition->setHorizontalHeaderLabels(PointsDetailsHeaders);
    ui->tablePointsPosition->setRowCount(Axes.count());
    ui->tablePointsPosition->setVerticalHeaderLabels(Axes);
    ui->tablePointsPosition->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePointsPosition->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
    {
        auto valueWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::POSITION, ui->tablePointsPosition);
        ui->tablePointsPosition->setCellWidget(axis, PointsDetailsHeaders_e::Value, valueWidget);

        auto velocityWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::POSITION_VELOCITY, ui->tablePointsPosition);
        ui->tablePointsPosition->setCellWidget(axis, PointsDetailsHeaders_e::Velocity, velocityWidget);

        auto accelerationWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::POSITION_ACCELERATION, ui->tablePointsPosition);
        ui->tablePointsPosition->setCellWidget(axis, PointsDetailsHeaders_e::Acceleration, accelerationWidget);
    }

    // - Rotation
    ui->tablePointsRotation->setColumnCount(PointsDetailsHeaders.count());
    ui->tablePointsRotation->setHorizontalHeaderLabels(PointsDetailsHeaders);
    ui->tablePointsRotation->setRowCount(Axes.count());
    ui->tablePointsRotation->setVerticalHeaderLabels(Axes);
    ui->tablePointsRotation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePointsRotation->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tablePointsRotation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
    {
        auto valueWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::ROTATION, ui->tablePointsPosition);
        ui->tablePointsRotation->setCellWidget(axis, PointsDetailsHeaders_e::Value, valueWidget);

        auto velocityWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::ROTATION_VELOCITY, ui->tablePointsPosition);
        ui->tablePointsRotation->setCellWidget(axis, PointsDetailsHeaders_e::Velocity, velocityWidget);

        auto accelerationWidget = new SpacialSpinBox(otpProducer, axis, MODULES::STANDARD::VALUES::ROTATION_ACCELERATION, ui->tablePointsPosition);
        ui->tablePointsRotation->setCellWidget(axis, PointsDetailsHeaders_e::Acceleration, accelerationWidget);
    }
}

GroupWindow::~GroupWindow()
{
    otpProducer->removeProducerGroup(system, group);
    delete ui;
}

void GroupWindow::setSystem(ACN::OTP::system_t newSystem)
{
    auto pointsList = otpProducer->getProducerPoints(system, group);
    for (auto point : pointsList)
        removedPoint(otpProducer->getProducerCID(), system, group, point);
    otpProducer->removeProducerGroup(system, group);

    system = newSystem;
    otpProducer->addProducerSystem(newSystem);
    otpProducer->addProducerGroup(newSystem, group);
    for (auto point : pointsList)
        otpProducer->addProducerPoint(newSystem, group, point);
}

void GroupWindow::on_pbAddPoint_clicked()
{
    auto dialog = new PointSelectionDialog(otpProducer->getProducerPoints(system, group), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    otpProducer->addProducerPoint(system, group, dialog->getPoint());
    otpProducer->setProducerPointName(
                address_t{system, group, dialog->getPoint()},
                QString("Point %1/%2/%3").arg(system).arg(group).arg(dialog->getPoint()));
}

void GroupWindow::on_pbRemovePoint_clicked()
{
    if (!ui->tablePoints->selectedItems().count()) return;
    for (auto index : ui->tablePoints->selectionModel()->selectedRows())
    {
        auto item = ui->tablePoints->takeItem(index.row(), index.column());
        ui->tablePoints->removeRow(index.row());
        point_t point = item->text().toUInt();
        otpProducer->removeProducerPoint(system, group, point);
        delete item;
    }
}

void GroupWindow::newPoint(cid_t cid, system_t system, group_t group, point_t point)
{
    if (!(cid == otpProducer->getProducerCID() &&
        (system == this->system) &&
        (group == this->group))) { return; }

    ui->tablePoints->setUpdatesEnabled(false);

    auto row = ui->tablePoints->rowCount();
    ui->tablePoints->insertRow(row);
    auto item = new QTableWidgetItem();
    item->setText(QString("%1").arg(point));
    ui->tablePoints->setItem(row, 0, item);

    ui->tablePoints->sortItems(0);

    ui->tablePoints->setUpdatesEnabled(true);
}

void GroupWindow::removedPoint(cid_t cid, system_t system, group_t group, point_t point)
{
    if (!(cid == otpProducer->getProducerCID() &&
        (system == this->system) &&
        (group == this->group))) { return; }

    ui->tablePoints->setUpdatesEnabled(false);
    for (int row = 0; row < ui->tablePoints->rowCount(); row++)
    {
        if (ui->tablePoints->item(row,0)->text() == QString("%1").arg(point))
        {
            ui->tablePoints->removeRow(row);
        }
    }

    ui->tablePoints->sortItems(0);

    ui->tablePoints->setUpdatesEnabled(true);
}

void GroupWindow::on_tablePoints_itemSelectionChanged()
{
    // Only show details for a single point
    if (ui->tablePoints->selectedItems().count() != 1)
    {
        ui->frameDetails->setDisabled(true);
        return;
    }

    ui->frameDetails->setDisabled(false);

    // Address
    if (!getSelectedAddress().isValid())
    {
        ui->frameDetails->setDisabled(true);
        return;
    }
    auto address = getSelectedAddress();

    // - Name
    ui->leName->setText(otpProducer->getProducerPointName(address));

    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
    {
        // - Position
        {
            auto valueWidget = static_cast<SpacialSpinBox*>(ui->tablePointsPosition->cellWidget(axis, PointsDetailsHeaders_e::Value));
            if (valueWidget)
                valueWidget->setAddress(address);

            auto velocityWidget = static_cast<SpacialSpinBox*>(ui->tablePointsPosition->cellWidget(axis, PointsDetailsHeaders_e::Velocity));
            if (velocityWidget)
                velocityWidget->setAddress(address);

            auto accelerationWidget = static_cast<SpacialSpinBox*>(ui->tablePointsPosition->cellWidget(axis, PointsDetailsHeaders_e::Acceleration));
            if (accelerationWidget)
                accelerationWidget->setAddress(address);
        }

        // - Rotation
        {
            auto valueWidget = static_cast<SpacialSpinBox*>(ui->tablePointsRotation->cellWidget(axis, PointsDetailsHeaders_e::Value));
            if (valueWidget)
                valueWidget->setAddress(address);

            auto velocityWidget = static_cast<SpacialSpinBox*>(ui->tablePointsRotation->cellWidget(axis, PointsDetailsHeaders_e::Velocity));
            if (velocityWidget)
                velocityWidget->setAddress(address);

            auto accelerationWidget = static_cast<SpacialSpinBox*>(ui->tablePointsRotation->cellWidget(axis, PointsDetailsHeaders_e::Acceleration));
            if (accelerationWidget)
                accelerationWidget->setAddress(address);
        }
    }
}

ACN::OTP::address_t GroupWindow::getSelectedAddress()
{
    auto point = point_t(ui->tablePoints->selectedItems().first()->text().toUInt());
    return address_t(system, group, point);
}

void GroupWindow::on_leName_textChanged(const QString &arg1)
{
    if (!ui->tablePoints->selectedItems().count()) return;
    auto point = point_t(ui->tablePoints->selectedItems().first()->text().toUInt());
    auto address = address_t(system, group, point);
    if (!address.isValid()) return;

    otpProducer->setProducerPointName(address, arg1);
}
