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
#include <QSettings>

using namespace OTP;

ProducerWindow::ProducerWindow(const QString &componentSettingsGroup, QMainWindow *parent) :
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
                          Settings::getInstance().getComponentSettings(componentSettingsGroup).CID,
                          Settings::getInstance().getComponentSettings(componentSettingsGroup).Name,
                          Settings::getInstance().getTransformMessageRate(),
                          this));

    // OTP Producer Interface
    connect(&Settings::getInstance(), &Settings::newNetworkInterface,
            this, [this](QNetworkInterface interface) {
                otpProducer->setNetworkInterface(interface);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Producer Transport
    connect(&Settings::getInstance(), &Settings::newNetworkTransport,
            this, [this](QAbstractSocket::NetworkLayerProtocol transport) {
                otpProducer->setNetworkTransport(transport);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Producer Name
    connect(ui->leProducerName, &QLineEdit::textChanged,
            this, [this](const QString &arg1) {
                otpProducer->setLocalName(arg1);
                saveComponentDetails();
                updateWindowTitle();
            });
    ui->leProducerName->setText(otpProducer->getLocalName());
    ui->leProducerName->setMaxLength(PDU::NAME_LENGTH);

    // OTP Producer CID
    connect(ui->pbProducerNewCID, &QPushButton::clicked,
            this, [this]() {
                otpProducer->setLocalCID(cid_t::createUuid());
            });
    connect(otpProducer.get(), &Producer::newLocalCID, this,
            [this](const cid_t CID) {
                ui->lblProducerCID->setText(CID.toString());
                saveComponentDetails();
            });
    ui->lblProducerCID->setText(otpProducer->getLocalCID().toString());

    // OTP Producer System
    otpProducer->addLocalSystem(static_cast<system_t>(ui->sbSystem->value()));
    connect(ui->sbSystem, qOverload<OTP::system_t, OTP::system_t>(&SystemSpinBox::valueChanged),
            this, [this](OTP::system_t oldValue, OTP::system_t newValue) {
                otpProducer->addLocalSystem(newValue);
                for (const auto &subWindow : ui->mdiArea->subWindowList())
                    static_cast<GroupWindow*>(subWindow->widget())->setSystem(newValue);
                otpProducer->removeLocalSystem(oldValue);
            });

    // OTP Producer Transform Message Rate
    connect(&Settings::getInstance(), &Settings::newTransformMessageRate,
            this, [this](std::chrono::milliseconds value) {
                otpProducer->setTransformMsgRate(value);
            });
}

ProducerWindow::~ProducerWindow()
{
    delete ui;
}

void ProducerWindow::showEvent(QShowEvent *event) {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    restoreGeometry(settings.value("ProducerWindow/geometry").toByteArray());
    restoreState(settings.value("ProducerWindow/state").toByteArray());
    QMainWindow::showEvent(event);
}

void ProducerWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("ProducerWindow/geometry", saveGeometry());
    settings.setValue("ProducerWindow/state", saveState());
    QMainWindow::closeEvent(event);
}

void ProducerWindow::updateStatusBar()
{
    QString message = QObject::tr("Not connected");
    if (otpProducer)
    {
        message = QObject::tr("Selected interface: %1").arg(
                    otpProducer->getNetworkInterface().humanReadableName());

        if ((otpProducer->getNetworkTransport() == QAbstractSocket::IPv4Protocol) ||
                (otpProducer->getNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpProducer->getNetworkinterfaceState(QAbstractSocket::IPv4Protocol);
            message.append(QString(" OTP-4 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }

        if ((otpProducer->getNetworkTransport() == QAbstractSocket::IPv6Protocol) ||
                (otpProducer->getNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpProducer->getNetworkinterfaceState(QAbstractSocket::IPv6Protocol);
            message.append(QString(" OTP-6 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }
    }

    ui->statusbar->showMessage(message);
}

void ProducerWindow::updateWindowTitle()
{
this->setWindowTitle(
            QString("%1 %2 - Producer - (%3)").arg(
                    QApplication::applicationName(),
                    QApplication::applicationVersion(),
                    ui->leProducerName->text()));
}

void ProducerWindow::saveComponentDetails()
{
    Settings::componentDetails_t details(otpProducer->getLocalName(), otpProducer->getLocalCID());
    Settings::getInstance().setComponentSettings(componentSettingsGroup, details);
}

void ProducerWindow::on_actionNew_Group_triggered()
{
    auto system = static_cast<system_t>(ui->sbSystem->value());
    auto dialog = new GroupSelectionDialog(otpProducer->getLocalGroups(system), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    auto group = dialog->getGroup();
    auto mdiWindow = new GroupWindow(otpProducer, system, group, this);
    mdiWindow->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(mdiWindow);
    mdiWindow->show();
}
