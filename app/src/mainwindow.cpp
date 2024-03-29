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
#include <QMdiSubWindow>
#include <QSettings>
#include "systemselectiondialog.h"
#include "models/componentsmodel.h"

using namespace OTP;

const auto componentSettingsGroup_CONSUMER = QStringLiteral("CONSUMER");
const auto componentSettingsGroup_PRODUCER = QStringLiteral("PRODUCER");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Window
    ui->setupUi(this);
    this->setWindowTitle(
                QString("%1 %2 - Main Window")
                .arg(QApplication::applicationName(), QApplication::applicationVersion()));

    // About
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, QString("About %1").arg(QApplication::applicationName()),
                           QString(
                               "<style> h1,h2,h3,h4,h5 { text-align: center; }</style>"
                               "<p><h1>%1</h1>"
                               "<h2>Version: %2</h2>"
                               "<h3>A QT Frontend for ANSI E1.59:2021</h3>"
                               "<h4>Entertainment Technology Object Transform Protocol (OTP)</h4>"
                               "<h5>%3<br>"
                               "<a href=\"https://github.com/marcusbirkin/OTPView\">https://github.com/marcusbirkin/OTPView</a></h5></p>")
                           .arg(QApplication::applicationName(), QApplication::applicationVersion(), VER_LEGALCOPYRIGHT_STR)
                           .append(
                               "<p>This program is free software: you can redistribute it and/or modify "
                               "it under the terms of the GNU Lesser General Public License as published by "
                               "the Free Software Foundation, either version 3 of the License, or "
                               "(at your option) any later version.</p>"
                               "<p>This program is distributed in the hope that it will be useful, "
                               "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                               "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                               "GNU General Public License for more details."
                               "<p>You should have received a copy of the GNU Lesser General Public License "
                               "along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>"
                               )
                           );
    });
    connect(ui->actionAbout_OTPLib, &QAction::triggered, this,
            [this]() {
                QMessageBox::about(this, QString("About OTPLib"),OTPLib::getAbout());
            });
    connect(ui->actionAbout_QT, &QAction::triggered, this,
            [this]() {
                QMessageBox::aboutQt(this);
            });

    // OTP Consumer Object
    otpConsumer.reset(new class Consumer(
                          Settings::getInstance().getNetworkInterface(),
                          Settings::getInstance().getNetworkTransport(),
                          QList<system_t>(),
                          Settings::getInstance().getComponentSettings(componentSettingsGroup_CONSUMER).CID,
                          Settings::getInstance().getComponentSettings(componentSettingsGroup_CONSUMER).Name,
                          this));

    // OTP Consumer Interface
    connect(&Settings::getInstance(), &Settings::newNetworkInterface, this,
            [this](QNetworkInterface interface) {
                otpConsumer->setNetworkInterface(interface);

                connect(otpConsumer.get(), &OTP::Consumer::stateChangedNetworkInterface, this,
                        [this]() {
                            updateStatusBar();
                });

                updateStatusBar();
            });
    updateStatusBar();

    // OTP Consumer Transport
    connect(&Settings::getInstance(), &Settings::newNetworkTransport, this,
            [this](QAbstractSocket::NetworkLayerProtocol transport) {
                otpConsumer->setNetworkTransport(transport);
                updateStatusBar();
            });
    updateStatusBar();

    // OTP Consumer Name
    connect(ui->leConsumerName, &QLineEdit::textChanged, this,
            [this](const QString &arg1) {
                otpConsumer->setLocalName(arg1);
                saveComponentDetails();
            });
    ui->leConsumerName->setText(otpConsumer->getLocalName());
    ui->leConsumerName->setMaxLength(PDU::NAME_LENGTH);

    // OTP Consumer CID
    connect(ui->pbConsumerNewCID, &QPushButton::clicked, this,
            [this]() {
                otpConsumer->setLocalCID(cid_t::createUuid());
            });
    connect(otpConsumer.get(), &Consumer::newLocalCID, this,
            [this](const cid_t CID) {
                ui->lblConsumerCID->setText(CID.toString());
                saveComponentDetails();
            });
    ui->lblConsumerCID->setText(otpConsumer->getLocalCID().toString());

    // OTP Components Table
    ui->tvComponents->setModel(new ComponentsModel(otpConsumer, this));
    connect(ui->tvComponents, &QTreeView::doubleClicked, this, &MainWindow::on_tvComponents_doubleClicked);

    // System requests
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,
        [this]() {
           otpConsumer->UpdateOTPMap();
        });
    timer->start(Settings::getInstance().getSystemRequestInterval());
    connect(&Settings::getInstance(), &Settings::newSystemRequestInterval, this,
            [timer](std::chrono::seconds value) {
                timer->setInterval(value);
            });
    otpConsumer->UpdateOTPMap();
}

MainWindow::~MainWindow()
{
    otpConsumer.get()->disconnect();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event) {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/state").toByteArray());
    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::on_tvComponents_doubleClicked(const QModelIndex &index)
{
    auto item = static_cast<ComponentsItem*>(index.internalPointer());
    switch(item->getType())
    {
        // Open consumer for selected system
        case ComponentsItem::ComponentSystemListItem:
        {
           openSystemWindow(system_t(item->data().toInt()));
        } return;

        default: return;
    }
}

void MainWindow::updateStatusBar()
{
    QString message = QObject::tr("Not connected");
    if (otpConsumer)
    {
        message = QObject::tr("Selected interface: %1").arg(
                    otpConsumer->getNetworkInterface().humanReadableName());

        if ((otpConsumer->getNetworkTransport() == QAbstractSocket::IPv4Protocol) ||
                (otpConsumer->getNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpConsumer->getNetworkinterfaceState(QAbstractSocket::IPv4Protocol);
            message.append(QString(" OTP-4 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }

        if ((otpConsumer->getNetworkTransport() == QAbstractSocket::IPv6Protocol) ||
                (otpConsumer->getNetworkTransport() == QAbstractSocket::AnyIPProtocol))
        {
            auto status = otpConsumer->getNetworkinterfaceState(QAbstractSocket::IPv6Protocol);
            message.append(QString(" OTP-6 (%1)").arg(status == QAbstractSocket::BoundState ? tr("OK") : tr("Error")));
        }
    }

    ui->statusbar->showMessage(message);
}

void MainWindow::saveComponentDetails()
{
    Settings::componentDetails_t details(otpConsumer->getLocalName(), otpConsumer->getLocalCID());
    Settings::getInstance().setComponentSettings(componentSettingsGroup_CONSUMER, details);
}

bool MainWindow::openSystemWindow(system_t system)
{
    if (!system.isValid()) return false;

    // Already open?
    if (otpConsumer->getLocalSystems().contains(system))
    {
        for (const auto &subWindow : ui->mdiArea->subWindowList())
        {
            auto systemWindow = reinterpret_cast<SystemWindow*>(subWindow->widget());
            if ( systemWindow->getSystem() == system)
            {
                qDebug() << this << "Set focus system window" << systemWindow->windowTitle();
                systemWindow->setFocus();
                systemWindow->setWindowState(Qt::WindowState::WindowActive);
                return true;
            }
        }
    }

    // Open new
    qDebug() << this << "Open system window" << system;
    auto systemWindow = new SystemWindow(otpConsumer, system, this);
    systemWindow->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(systemWindow);
    systemWindow->show();
    return true;
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
    auto dialog = new SystemSelectionDialog(otpConsumer->getLocalSystems(), this);
    if (dialog->exec() == QDialog::Rejected)
        return;

    openSystemWindow(dialog->getSystem());
}
