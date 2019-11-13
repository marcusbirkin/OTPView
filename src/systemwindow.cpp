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
}

SystemWindow::~SystemWindow()
{
    otpConsumer->removeConsumerSystem(system);
    delete ui;
}
