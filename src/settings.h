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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QNetworkInterface>
#include "libs/OTPLib/otp.hpp"

class Settings final : public QObject
{
    Q_OBJECT

public:
    static Settings& getInstance();

    void setNetworkInterface(QNetworkInterface interface);
    QNetworkInterface getNetworkInterface();

    void setNetworkTransport(QAbstractSocket::NetworkLayerProtocol transport);
    QAbstractSocket::NetworkLayerProtocol getNetworkTransport();

    typedef struct componentDetails_s
    {
        componentDetails_s() :
            CID(OTP::cid_t::createUuid())
        {
            Name = QString("%1 %2").arg(QApplication::applicationName()).arg(QApplication::applicationVersion());
        }
        componentDetails_s(QString name, OTP::cid_t CID) :
            Name(name),
            CID(CID) {}
        QString Name;
        OTP::cid_t CID;
    } componentDetails_t;
    void setComponentSettings(QString settingsGroup, componentDetails_t details);
    componentDetails_t getComponentSettings(QString settingsGroup);

    void setSystemRequestInterval(std::chrono::seconds interval);
    std::chrono::seconds getSystemRequestInterval();

    void setTransformMessageRate(std::chrono::milliseconds interval);
    std::chrono::milliseconds getTransformMessageRate();

signals:
    void newNetworkInterface(QNetworkInterface);
    void newNetworkTransport(QAbstractSocket::NetworkLayerProtocol);
    void newSystemRequestInterval(std::chrono::seconds);
    void newTransformMessageRate(std::chrono::milliseconds);

private:
    Settings();
    ~Settings() = default;


    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = delete;
};

#endif // SETTINGS_H
