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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "settings.h"
#include <QMessageBox>
#include <QTimer>
#include "systemselectiondialog.h"

using namespace ACN::OTP;

const static QString componentSettingsGroup_CONSUMER = "CONSUMER";
const static QString componentSettingsGroup_PRODUCER = "PRODUCER";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Window
    ui->setupUi(this);
    this->setWindowTitle(
                QString("%1 %2 - Main Window")
                .arg(QApplication::applicationName())
                .arg(QApplication::applicationVersion()));

    // About
    connect(ui->actionAbout, &QAction::triggered, [this]() {
        QMessageBox::about(this, QString("About %1").arg(QApplication::applicationName()),
                           QString(
                               "<style> h1,h2,h3,h4 { text-align: center; }</style>"
                               "<p><h1>%1</h1>"
                               "<h2>Version: %2</h2>"
                               "<h3>A QT Frontend for E1.59 (Entertainment Technology Object Transform Protocol (OTP))</h3>"
                               "<h4>Marcus Birkin 2019</h4>"
                               "<h4><a href=\"https://github.com/marcusbirkin/OTPView\">https://github.com/marcusbirkin/OTPView</a></h4></p>")
                           .arg(QApplication::applicationName())
                           .arg(QApplication::applicationVersion())
                           .append(
                               "<p>This program is free software: you can redistribute it and/or modify "
                               "it under the terms of the GNU Lesser General Public License as published by "
                               "the Free Software Foundation, either version 3 of the License, or "
                               "(at your option) any later version.</p>"
                               "<p>This program is distributed in the hope that it will be useful, "
                               "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                               "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                               "<p>You should have received a copy of the GNU Lesser General Public License "
                               "along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>"
                               )
                           );
    });
    connect(ui->actionAbout_OTPLib, &QAction::triggered,
            [this]() {
                QMessageBox::about(this, QString("About OTPLib"),OTPLib::getAbout());
            });
    connect(ui->actionAbout_QT, &QAction::triggered,
            [this]() {
                QMessageBox::aboutQt(this);
            });

    // OTP Consumer Object
    otpConsumer.reset(new class Consumer(
                          Settings::getInstance().getNetworkInterface(),
                          Settings::getInstance().getNetworkTransport(),
                          QList<system_t>(),
                          Settings::getInstance().getComponentSettings(componentSettingsGroup_CONSUMER).Name,
                          Settings::getInstance().getComponentSettings(componentSettingsGroup_CONSUMER).CID,
                          this));

    // OTP Consumer Interface
    connect(&Settings::getInstance(), &Settings::newNetworkInterface,
            [this](QNetworkInterface interface) {
                otpConsumer->setConsumerNetworkInterface(interface);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Consumer Transport
    connect(&Settings::getInstance(), &Settings::newNetworkTransport,
            [this](QAbstractSocket::NetworkLayerProtocol transport) {
                otpConsumer->setConsumerNetworkTransport(transport);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Consumer Name
    connect(ui->leConsumerName, &QLineEdit::textChanged,
            [this](const QString &arg1) {
                otpConsumer->setConsumerName(arg1);
                saveComponentDetails();
            });
    ui->leConsumerName->setText(otpConsumer->getConsumerName());
    ui->leConsumerName->setMaxLength(PDU::NAME_LENGTH);

    // OTP Consumer CID
    connect(ui->pbConsumerNewCID, &QPushButton::clicked,
            [this]() {
                otpConsumer->setConsumerCID(cid_t::createUuid());
            });
    connect(otpConsumer.get(), &Consumer::newConsumerCID,
            [this](const cid_t CID) {
                ui->lblConsumerCID->setText(CID.toString());
                saveComponentDetails();
            });
    ui->lblConsumerCID->setText(otpConsumer->getConsumerCID().toString());

    // OTP Components Table
    connect(otpConsumer.get(), &Consumer::newComponent,
            [this](cid_t cid) {
                auto component = otpConsumer->getComponent(cid);
                QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->twComponents, QStringList(cid.toString()));
                updatedComponent(cid, component.getType());
                Q_UNUSED(rootItem)
            });
    connect(otpConsumer.get(), qOverload<const cid_t&, const name_t&>(&Consumer::updatedComponent),
            this, qOverload<const cid_t&, const name_t&>(&MainWindow::updatedComponent));
    connect(otpConsumer.get(), qOverload<const cid_t&, const QHostAddress&>(&Consumer::updatedComponent),
            this, qOverload<const cid_t&, const QHostAddress&>(&MainWindow::updatedComponent));
    connect(otpConsumer.get(), qOverload<const cid_t&, const moduleList_t&>(&Consumer::updatedComponent),
            this, qOverload<const cid_t&, const moduleList_t&>(&MainWindow::updatedComponent));
    connect(otpConsumer.get(), qOverload<const cid_t&, component_t::type_t>(&Consumer::updatedComponent),
            this, qOverload<const cid_t&, const component_t::type_t>(&MainWindow::updatedComponent));
    connect(otpConsumer.get(), qOverload<cid_t, system_t>(&Consumer::newSystem),
            this, qOverload<cid_t, system_t>(&MainWindow::newSystem));
    connect(otpConsumer.get(), qOverload<cid_t, system_t>(&Consumer::removedSystem),
            this, qOverload<cid_t, system_t>(&MainWindow::removedSystem));

    {
        // Keep table updated
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout,
            [this]() {
               otpConsumer->UpdateOTPMap();
            });
        timer->start(static_cast<int>(Settings::getInstance().getSystemRequestInterval()) * 1000);
        connect(&Settings::getInstance(), &Settings::newSystemRequestInterval,
                [timer](uint value) {
                    timer->setInterval(static_cast<int>(value) * 1000);
                });
        otpConsumer->UpdateOTPMap();
    }
}

MainWindow::~MainWindow()
{
    otpConsumer.get()->disconnect();
    delete ui;
}

void MainWindow::updateStatusBar()
{
    QString message = QObject::tr("Not connected");
    if (otpConsumer)
    {
        message = QObject::tr("Selected interface: %1").arg(
                    otpConsumer->getConsumerNetworkInterface().humanReadableName());

        if ((otpConsumer->getConsumerNetworkTransport() == QAbstractSocket::IPv4Protocol) ||
                (otpConsumer->getConsumerNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpConsumer->getConsumerNetworkinterfaceState(QAbstractSocket::IPv4Protocol);
            message.append(QString(" IPv4 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }

        if ((otpConsumer->getConsumerNetworkTransport() == QAbstractSocket::IPv6Protocol) ||
                (otpConsumer->getConsumerNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpConsumer->getConsumerNetworkinterfaceState(QAbstractSocket::IPv6Protocol);
            message.append(QString(" IPv6 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }
    }

    ui->statusbar->showMessage(message);
}

void MainWindow::saveComponentDetails()
{
    Settings::componentDetails_t details(otpConsumer->getConsumerName(), otpConsumer->getConsumerCID());
    Settings::getInstance().setComponentSettings(componentSettingsGroup_CONSUMER, details);
}


void MainWindow::updatedComponent(const cid_t &cid, const name_t &name)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    auto string = QString("Name: %1").arg(name.toString());
    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("Name: "))
        {
            rootItem->child(idx)->setText(0, string);
            return;
        }
    }
    rootItem->addChild(new QTreeWidgetItem(QStringList(string)));
}

void MainWindow::updatedComponent(const cid_t &cid, const QHostAddress &IPAddr)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    auto string = QString("IP: %1").arg(IPAddr.toString());
    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("IP: "))
        {
            rootItem->child(idx)->setText(0, string);
            return;
        }
    }
    rootItem->addChild(new QTreeWidgetItem(QStringList(string)));
}

void MainWindow::updatedComponent(const cid_t &cid, const moduleList_t &moduleList)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("Modules"))
            rootItem->removeChild(rootItem->child(idx));
    }

    auto childItem = new QTreeWidgetItem(rootItem, QStringList(QString("Modules")));

    for (auto module : moduleList)
    {
        childItem->addChild(
                    new QTreeWidgetItem(QStringList(QString("%1").arg(ACN::OTP::component_s::getModuleString(module)))));
    }
}

void MainWindow::updatedComponent(const cid_t& cid, component_t::type_t type)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    QString typeString = (type == component_t::type_t::consumer) ? "Consumer" : "Producer";
    auto string = QString("Type: %1").arg(typeString);
    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("Type: "))
        {
            rootItem->child(idx)->setText(0, string);
            return;
        }
    }
    rootItem->addChild(new QTreeWidgetItem(QStringList(string)));
}

void MainWindow::newSystem(cid_t cid, system_t system)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    QTreeWidgetItem *childItem = Q_NULLPTR;
    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("System Numbers:"))
            childItem = rootItem->child(idx);
    }
    if (!childItem) childItem = new QTreeWidgetItem(rootItem, QStringList(QString("System Numbers:")));

    childItem->addChild(
                new QTreeWidgetItem(QStringList(QString("%1").arg(system))));

    childItem->sortChildren(0, Qt::SortOrder::AscendingOrder);
}

void MainWindow::removedSystem(cid_t cid, system_t system)
{
    auto rootList = ui->twComponents->findItems(cid.toString(), Qt::MatchExactly);
    if (rootList.isEmpty()) return;

    auto rootItem = rootList.first();

    for (int idx = 0; idx < rootItem->childCount(); idx++)
    {
        if (rootItem->child(idx)->text(0).startsWith("System Numbers:"))
            for (auto sysIdx = 0; sysIdx < rootItem->child(idx)->childCount(); sysIdx++)
            {
                if (rootItem->child(idx)->child(sysIdx)->text(0) == QString("%1").arg(system))
                    rootItem->child(idx)->removeChild(rootItem->child(idx)->child(sysIdx));
            }
    }
}

void MainWindow::on_actionSettings_triggered()
{
    auto settings = new SettingsDialog(this);
    settings->exec();
}

void MainWindow::on_actionNew_Producer_triggered()
{
    auto producer = new ProducerWindow(QString("%1_%2").arg(componentSettingsGroup_PRODUCER).arg(producerWindows.size()), this);
    producerWindows.append(producer);
    producer->setAttribute(Qt::WA_DeleteOnClose);
    producer->show();
}

void MainWindow::on_actionNew_Consumer_triggered()
{
    auto dialog = new SystemSelectionDialog(otpConsumer->getConsumerSystems(), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    auto system = dialog->getSystem();
    auto mdiWindow = new SystemWindow(otpConsumer, system, this);
    mdiWindow->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(mdiWindow);
    mdiWindow->show();
}
