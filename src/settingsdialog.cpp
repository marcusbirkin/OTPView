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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include <QNetworkInterface>
#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // Interface Protocol
    QObject::connect<void(QComboBox::*)(int)>(ui->cbProtocol, &QComboBox::currentIndexChanged, [=]() { checkSaveAllowed(); });
    populateProtocolList();

    // Interface list
    QObject::connect<void(QComboBox::*)(int)>(ui->cbInterface, &QComboBox::currentIndexChanged, [=]() { checkSaveAllowed(); });
    populateInterfaceList();

    // System Request Interval
    ui->sbSystemRequestInterval->setValue(static_cast<int>(Settings::getInstance().getSystemRequestInterval().count()));

    // Transform message interval
    ui->sbTransformTXRate->setRange(OTP::OTP_TRANSFORM_TIMING_MIN.count(), OTP::OTP_TRANSFORM_TIMING_MAX.count());
    ui->sbTransformTXRate->setValue(static_cast<int>(Settings::getInstance().getTransformMessageRate().count()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    Settings &instance = Settings::getInstance();
    instance.setNetworkInterface(
                QNetworkInterface::interfaceFromName(ui->cbInterface->currentData().toString()));

    instance.setNetworkTransport(
                static_cast<QAbstractSocket::NetworkLayerProtocol>(ui->cbProtocol->itemData(ui->cbProtocol->currentIndex()).toInt()));

    instance.setSystemRequestInterval(
                std::chrono::seconds(ui->sbSystemRequestInterval->value()));

    instance.setTransformMessageRate(
                std::chrono::milliseconds(ui->sbTransformTXRate->value()));

    this->close();
}

void SettingsDialog::on_buttonBox_rejected()
{
    this->close();
}

void SettingsDialog::checkSaveAllowed()
{
    bool saveAllowed = true;

    if (ui->cbInterface->currentIndex() == -1) saveAllowed = false;
    if (ui->cbProtocol->currentIndex() == -1) saveAllowed = false;

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(saveAllowed);
}

void SettingsDialog::populateInterfaceList()
{
    ui->cbInterface->clear();
    int currentIndex = -1;
    for (auto interface : QNetworkInterface::allInterfaces())
    {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) && interface.flags().testFlag(QNetworkInterface::CanMulticast))
        {
            QString ipAddrString;
            auto protocol = static_cast<QAbstractSocket::NetworkLayerProtocol>(ui->cbProtocol->itemData(ui->cbProtocol->currentIndex()).toInt());
            for (auto addr : interface.addressEntries())
            {
                if ((addr.ip().protocol() == protocol) || (protocol == QAbstractSocket::AnyIPProtocol))
                {
                    if (!ipAddrString.isEmpty())
                        ipAddrString.append(" / ");
                    ipAddrString.append(addr.ip().toString());
                }
            }

            if (!ipAddrString.isEmpty())
            {
                if (Settings::getInstance().getNetworkInterface().name() == interface.name())
                    currentIndex = ui->cbInterface->count();

                ui->cbInterface->addItem(
                            QString("%1 (%2)")
                            .arg(interface.humanReadableName())
                            .arg(ipAddrString),
                            interface.name());
            }
        }
    }

    ui->cbInterface->setCurrentIndex(currentIndex);
}

void SettingsDialog::populateProtocolList()
{
    ui->cbProtocol->clear();
    ui->cbProtocol->addItem("IPv4", QAbstractSocket::IPv4Protocol);
    ui->cbProtocol->addItem("IPv6", QAbstractSocket::IPv6Protocol);
    ui->cbProtocol->addItem("IPv4 & IPv6", QAbstractSocket::AnyIPProtocol);

    for (int idx = 0; idx < ui->cbProtocol->count(); idx++)
    {
       if (static_cast<QAbstractSocket::NetworkLayerProtocol>(ui->cbProtocol->itemData(idx).toInt())
               == Settings::getInstance().getNetworkTransport())
           ui->cbProtocol->setCurrentIndex(idx);
    }
}

void SettingsDialog::on_cbProtocol_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    populateInterfaceList();
}
