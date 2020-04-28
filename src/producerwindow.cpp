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
#include "producerwindow.h"
#include "ui_producerwindow.h"
#include "settings.h"
#include "groupselectiondialog.h"
#include <QMdiSubWindow>

using namespace OTP;

ProducerWindow::ProducerWindow(QString componentSettingsGroup, QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::ProducerWindow),
    componentSettingsGroup(componentSettingsGroup)
{
    // Window
    ui->setupUi(this);
    updateWindowTitle();

    // OTP Producer Object
    otpProducer.reset(new class Producer(
                          Settings::getInstance().getNetworkInterface(),
                          Settings::getInstance().getNetworkTransport(),
                          Settings::getInstance().getComponentSettings(componentSettingsGroup).Name,
                          Settings::getInstance().getComponentSettings(componentSettingsGroup).CID,
                          Settings::getInstance().getTransformMessageRate(),
                          this));

    // OTP Producer Interface
    connect(&Settings::getInstance(), &Settings::newNetworkInterface,
            [this](QNetworkInterface interface) {
                otpProducer->setProducerNetworkInterface(interface);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Producer Transport
    connect(&Settings::getInstance(), &Settings::newNetworkTransport,
            [this](QAbstractSocket::NetworkLayerProtocol transport) {
                otpProducer->setProducerNetworkTransport(transport);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Producer Name
    connect(ui->leProducerName, &QLineEdit::textChanged,
            [this](const QString &arg1) {
                otpProducer->setProducerName(arg1);
                saveComponentDetails();
                updateWindowTitle();
            });
    ui->leProducerName->setText(otpProducer->getProducerName());
    ui->leProducerName->setMaxLength(PDU::NAME_LENGTH);

    // OTP Producer CID
    connect(ui->pbProducerNewCID, &QPushButton::clicked,
            [this]() {
                otpProducer->setProducerCID(cid_t::createUuid());
            });
    connect(otpProducer.get(), &Producer::newCID,
            [this](const cid_t CID) {
                ui->lblProducerCID->setText(CID.toString());
                saveComponentDetails();
            });
    ui->lblProducerCID->setText(otpProducer->getProducerCID().toString());

    // OTP Producer System
    otpProducer->addProducerSystem(static_cast<system_t>(ui->sbSystem->value()));
    connect(ui->sbSystem, qOverload<OTP::system_t, OTP::system_t>(&SystemSpinBox::valueChanged),
        [this](OTP::system_t oldValue, OTP::system_t newValue) {
            otpProducer->addProducerSystem(newValue);
            for (auto subWindow : ui->mdiArea->subWindowList())
                static_cast<GroupWindow*>(subWindow->widget())->setSystem(newValue);
            otpProducer->removeProducerSystem(oldValue);
        });

    // OTP Producer Transform Message Rate
    connect(&Settings::getInstance(), &Settings::newTransformMessageRate,
            [this](std::chrono::milliseconds value) {
                otpProducer->setProducerTransformMsgRate(value);
            });
}

ProducerWindow::~ProducerWindow()
{
    delete ui;
}

void ProducerWindow::updateStatusBar()
{
    QString message = QObject::tr("Not connected");
    if (otpProducer)
    {
        message = QObject::tr("Selected interface: %1").arg(
                    otpProducer->getProducerNetworkInterface().humanReadableName());

        if ((otpProducer->getProducerNetworkTransport() == QAbstractSocket::IPv4Protocol) ||
                (otpProducer->getProducerNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpProducer->getProducerNetworkinterfaceState(QAbstractSocket::IPv4Protocol);
            message.append(QString(" IPv4 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }

        if ((otpProducer->getProducerNetworkTransport() == QAbstractSocket::IPv6Protocol) ||
                (otpProducer->getProducerNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpProducer->getProducerNetworkinterfaceState(QAbstractSocket::IPv6Protocol);
            message.append(QString(" IPv6 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }
    }

    ui->statusbar->showMessage(message);
}

void ProducerWindow::updateWindowTitle()
{
this->setWindowTitle(
            QString("%1 %2 - Producer - (%3)")
                .arg(QApplication::applicationName())
                .arg(QApplication::applicationVersion())
                .arg(ui->leProducerName->text()));
}

void ProducerWindow::saveComponentDetails()
{
    Settings::componentDetails_t details(otpProducer->getProducerName(), otpProducer->getProducerCID());
    Settings::getInstance().setComponentSettings(componentSettingsGroup, details);
}

void ProducerWindow::on_actionNew_Group_triggered()
{
    auto system = static_cast<system_t>(ui->sbSystem->value());
    auto dialog = new GroupSelectionDialog(otpProducer->getProducerGroups(system), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    auto group = dialog->getGroup();
    auto mdiWindow = new GroupWindow(otpProducer, system, group, this);
    mdiWindow->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(mdiWindow);
    mdiWindow->show();
}
