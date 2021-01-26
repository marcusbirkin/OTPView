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
#include "settings.h"
#include <QSettings>


// Settings Strings
static const QString S_GENERAL("GENERAL");
static const QString S_GENERAL_SYSTEMREQUESTINTERVAL("SYSTEMREQUESTINTERVAL");
static const QString S_GENERAL_SOURCERESOLUTION("RESOLUTION");
static const QString S_GENERAL_TRANSFORM_RATE("TRANSFORMRATE");

static const QString S_NETWORK("NETWORK");
static const QString S_NETWORK_HARDWAREADDRESS("HARDWAREADDRESS");
static const QString S_NETWORK_NAME("NAME");
static const QString S_NETWORK_TRANSPORT("TRANSPORT");

static const QString S_COMPONENT_NAME("NAME");
static const QString S_COMPONENT_CID("CID");

Settings::Settings() : QObject() {};

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}

void Settings::setNetworkInterface(QNetworkInterface interface)
{
    QSettings settings;
    settings.beginGroup(S_NETWORK);
    settings.setValue(S_NETWORK_HARDWAREADDRESS, interface.hardwareAddress());
    settings.setValue(S_NETWORK_NAME, interface.name());
    settings.sync();

    emit newNetworkInterface(interface);
}
QNetworkInterface Settings::getNetworkInterface()
{
    QSettings settings;
    settings.beginGroup(S_NETWORK);
    return QNetworkInterface::interfaceFromName(settings.value(S_NETWORK_NAME).toString());
}

void Settings::setNetworkTransport(QAbstractSocket::NetworkLayerProtocol transport)
{
    QSettings settings;
    settings.beginGroup(S_NETWORK);
    settings.setValue(S_NETWORK_TRANSPORT, static_cast<int>(transport));
    settings.sync();

    emit newNetworkTransport(transport);
}
QAbstractSocket::NetworkLayerProtocol Settings::getNetworkTransport()
{
    QSettings settings;
    settings.beginGroup(S_NETWORK);
    return static_cast<QAbstractSocket::NetworkLayerProtocol>(settings.value(S_NETWORK_TRANSPORT, QAbstractSocket::AnyIPProtocol).toInt());
}

void Settings::setComponentSettings(QString settingsGroup, componentDetails_t details)
{
    QSettings settings;
    settings.beginGroup(settingsGroup);
    settings.setValue(S_COMPONENT_NAME, details.Name);
    settings.setValue(S_COMPONENT_CID, details.CID.toByteArray());
    settings.sync();
}
Settings::componentDetails_t Settings::getComponentSettings(QString settingsGroup)
{
    componentDetails_t details;
    QSettings settings;
    settings.beginGroup(settingsGroup);
    details.Name = settings.value(S_COMPONENT_NAME, details.Name).toString();
    details.CID = OTP::cid_t(settings.value(S_COMPONENT_CID, details.CID.toByteArray()).toByteArray());
    return details;
}

void Settings::setSystemRequestInterval(std::chrono::seconds interval)
{
    QSettings settings;
    settings.beginGroup(S_GENERAL);
    settings.setValue(S_GENERAL_SYSTEMREQUESTINTERVAL, static_cast<qlonglong>(interval.count()));
    settings.sync();

    emit newSystemRequestInterval(interval);
}
std::chrono::seconds Settings::getSystemRequestInterval()
{
    QSettings settings;
    settings.beginGroup(S_GENERAL);
    return std::chrono::seconds(
            settings.value(S_GENERAL_SYSTEMREQUESTINTERVAL, static_cast<qlonglong>(std::chrono::seconds(10).count())).toLongLong());
}

void Settings::setTransformMessageRate(std::chrono::milliseconds interval)
{
    QSettings settings;
    settings.beginGroup(S_GENERAL);
    settings.setValue(S_GENERAL_TRANSFORM_RATE, static_cast<qlonglong>(interval.count()));
    settings.sync();

    emit newTransformMessageRate(interval);
}

std::chrono::milliseconds Settings::getTransformMessageRate()
{
    QSettings settings;
    settings.beginGroup(S_GENERAL);
    return std::chrono::milliseconds(
                settings.value(S_GENERAL_TRANSFORM_RATE, static_cast<qlonglong>(OTP::OTP_TRANSFORM_TIMING_MAX.count())).toLongLong());
}
