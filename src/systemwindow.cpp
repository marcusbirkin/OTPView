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
#include "models/systemmodel.h"
#include "widgets/linechart.h"
#include <QSettings>

using namespace OTP;

SystemWindow::SystemWindow(
        std::shared_ptr<class OTP::Consumer> otpConsumer,
        OTP::system_t system,
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
    connect(ui->tvDetails, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
        if (ui->tvDetails->selectionModel()->currentIndex().column() != 1) return;
        auto address = SystemItem::indexToItem(index)->getAddress();
        if (!address.isValid()) return;

        // Open and switch to
        auto idx = ui->tabWidget->addTab(new LineChart(this->otpConsumer, address, this), address.toString());
        ui->tabWidget->setCurrentIndex(idx);
    });

    // Tabs
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);
    // - Remove close button from first tab
    QTabBar *tabBar = ui->tabWidget->findChild<QTabBar *>();
    tabBar->setTabButton(0, QTabBar::RightSide, nullptr);
    tabBar->setTabButton(0, QTabBar::LeftSide, nullptr);
}

SystemWindow::~SystemWindow()
{
    otpConsumer->removeConsumerSystem(system);
    delete ui;
}

void SystemWindow::showEvent(QShowEvent *event) {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    parentWidget()->restoreGeometry(settings.value(QString("SystemWindow_%1/geometry").arg(system)).toByteArray());
    QWidget::showEvent(event);
}

void SystemWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue(QString("SystemWindow_%1/geometry").arg(system), parentWidget()->saveGeometry());
    QWidget::closeEvent(event);
}

void SystemWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->widget(index)->deleteLater();
    ui->tabWidget->removeTab(index);
}
