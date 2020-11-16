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
#include "models/pointstablemodel.h"
#include "widgets/systemspinbox.h"
#include "widgets/groupspinbox.h"
#include "widgets/pointspinbox.h"
#include "widgets/scalespinbox.h"
#include "widgets/priorityspinbox.h"
#include <QSettings>
#include <cstring>

using namespace OTP;

GroupWindow::GroupWindow(
        std::shared_ptr<class OTP::Producer> otpProducer,
        OTP::system_t system,
        OTP::group_t group,
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
    otpProducer->addLocalGroup(system, group);

    // Points
    ui->tablePoints->setModel(new PointsTableModel(otpProducer, system, group, this));
    ui->tablePoints->verticalHeader()->hide();
    ui->tablePoints->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tablePoints->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tablePoints->model(), &QAbstractItemModel::rowsInserted, this, [=](const QModelIndex, int start, int end)
    {
        // Automatically select newly inserted row
        if (start == end)
            ui->tablePoints->selectRow(start);
    });

    // Producer Points Details
    on_tablePoints_itemSelectionChanged();
    connect(ui->tablePoints->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &GroupWindow::on_tablePoints_itemSelectionChanged);

    // - Name
    ui->leName->setMaxLength(name_t::maxSize());
    connect(otpProducer.get(), &Producer::updatedLocalPointName, this, [=](address_t address) {
        if (this->getSelectedAddress().contains(address)) {
            this->on_tablePoints_itemSelectionChanged();
        }
    });

    // - Priority
    auto sbPriority = new PrioritySpinBox(otpProducer, this);
    ui->gbPriority->layout()->addWidget(sbPriority);

    // - Parent
    ui->cbParentDisable->setChecked(true);
    connect(otpProducer.get(), &Producer::updatedReferenceFrame, this, [=](address_t address) {
        if (this->getSelectedAddress().contains(address)) {
            this->on_tablePoints_itemSelectionChanged();
        }
    });

    // - Scale
    ui->tableScale->setColumnCount(ScaleHeaders.count());
    ui->tableScale->setHorizontalHeaderLabels(ScaleHeaders);
    ui->tableScale->setRowCount(Axes.count());
    ui->tableScale->setVerticalHeaderLabels(Axes);
    ui->tableScale->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableScale->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
    {
        auto scaleWidget = new ScaleSpinBox(otpProducer, axis, ui->tableScale);
        ui->tableScale->setCellWidget(axis, ScaleHeaders_e::Scale, scaleWidget);
    }

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
    otpProducer->removeLocalGroup(system, group);
    delete ui;
}

QObjectList findSpliters(const QObjectList &objList) {
    QObjectList ret;
    for (auto child : objList) {
        if (std::strcmp(child->metaObject()->className(), "QSplitter") == 0)
            ret.append(child);
        ret.append(findSpliters(child->children()));
    }

    return ret;
}

void GroupWindow::showEvent(QShowEvent *event) {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    parentWidget()->restoreGeometry(settings.value("GroupWindow/geometry").toByteArray());
    // Restore all splitter sizes
    for (auto obj : findSpliters(this->ui->gbPoints->children())) {
        auto splitter = qobject_cast<QSplitter*>(obj);
        splitter->setSizes(
            settings.value(
                QString("GroupWindow/spliter_%1").arg(splitter->objectName()))
            .value<QList<int>>());
    }
    QWidget::showEvent(event);
}

void GroupWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("GroupWindow/geometry", parentWidget()->saveGeometry());
    // Save all splitter sizes
    for (const auto obj : findSpliters(this->ui->gbPoints->children())) {
        auto splitter = qobject_cast<const QSplitter*>(obj);
        settings.setValue(
            QString("GroupWindow/spliter_%1").arg(splitter->objectName()),
            QVariant::fromValue(splitter->sizes()));
    }
    QWidget::closeEvent(event);
}

void GroupWindow::setSystem(OTP::system_t newSystem)
{
    auto pointsList = otpProducer->getLocalPoints(system, group);
    otpProducer->removeLocalGroup(system, group);

    system = newSystem;
    otpProducer->addLocalSystem(newSystem);
    otpProducer->addLocalGroup(newSystem, group);
    for (auto point : pointsList)
        otpProducer->addLocalPoint(newSystem, group, point, priority_t());

    qobject_cast<PointsTableModel*>(ui->tablePoints->model())->setSystem(newSystem);
}

QList<address_t> GroupWindow::getSelectedAddress()
{
    QList<address_t> ret;
    for (auto index : ui->tablePoints->selectionModel()->selectedRows())
    {
        auto model = qobject_cast<PointsTableModel*>(ui->tablePoints->model());
        if (model->getAddress(index).isValid())
            ret << model->getAddress(index);
    }
    return ret;
}

void GroupWindow::on_pbAddPoint_clicked()
{
    auto dialog = new PointSelectionDialog(otpProducer->getLocalPoints(system, group), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    otpProducer->addLocalPoint(system, group, dialog->getPoint(), priority_t());
    otpProducer->setLocalPointName(
                address_t{system, group, dialog->getPoint()},
                QString("Point %1/%2/%3").arg(system).arg(group).arg(dialog->getPoint()));
}

void GroupWindow::on_pbRemovePoint_clicked()
{
    for (auto address : getSelectedAddress())
        otpProducer->removeLocalPoint(address);

    on_tablePoints_itemSelectionChanged();
}

void GroupWindow::on_tablePoints_itemSelectionChanged()
{
    // Only show details for a single point
    ui->frameDetails->setDisabled(getSelectedAddress().count() != 1);
    if (getSelectedAddress().count() != 1) return;

    // Address
    auto address = getSelectedAddress().first();

    // - Name
    ui->leName->setText(otpProducer->getLocalPointName(address));

    // - Priority
    auto priorityWidget = static_cast<PrioritySpinBox*>(ui->gbPriority->layout()->itemAt(0)->widget());
    if (priorityWidget)
        priorityWidget->setAddress(address);

    // - Reference Frame
    auto parent = otpProducer->getLocalReferenceFrame(address);
    ui->cbParentDisable->setChecked(parent.value == address);
    ui->sbParentSystem->setValue(parent.value.system);
    ui->sbParentGroup->setValue(parent.value.group);
    ui->sbParentPoint->setValue(parent.value.point);

    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
    {
        // - Scale
        {
            auto scaleWidget = static_cast<ScaleSpinBox*>(ui->tableScale->cellWidget(axis, ScaleHeaders_e::Scale));
            if (scaleWidget)
                scaleWidget->setAddress(address);
        }

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

void GroupWindow::on_leName_textChanged(const QString &arg1)
{
    if (getSelectedAddress().count() != 1) return;
    otpProducer->setLocalPointName(getSelectedAddress().first(), arg1);
}

void GroupWindow::on_sbParent_valueChanged()
{
    if (getSelectedAddress().count() != 1) return;
    auto referenceFrame = otpProducer->getLocalReferenceFrame(getSelectedAddress().first());
    referenceFrame.timestamp = ui->cbParentDisable->isChecked() ? 0 :
            static_cast<OTP::timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
    if (ui->cbParentDisable->isChecked()) {
        referenceFrame.value = getSelectedAddress().first();
    } else {
        referenceFrame.value = {ui->sbParentSystem->value(), ui->sbParentGroup->value(), ui->sbParentPoint->value()};
    }

    otpProducer->setLocalReferenceFrame(getSelectedAddress().first(), referenceFrame);
}

void GroupWindow::on_sbParentSystem_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    on_sbParent_valueChanged();
}

void GroupWindow::on_sbParentGroup_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    on_sbParent_valueChanged();
}

void GroupWindow::on_sbParentPoint_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    on_sbParent_valueChanged();
}

void GroupWindow::on_cbParentDisable_stateChanged(int arg1)
{
    // Disable all widgets in Parent groupbox, except the disable checkbox!
    for (int ln = 0; ln < ui->gbParent->layout()->count(); ln++) {
        auto layout = ui->gbParent->layout()->itemAt(ln)->layout();
        for (int wn = 0; wn < layout->count(); wn++) {
            auto widget = layout->itemAt(wn)->widget();
            if (widget && (widget != ui->cbParentDisable))
                widget->setEnabled(!static_cast<bool>(arg1));
        }
    }

    on_sbParent_valueChanged();
}
