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
#include "settings.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("OTPView");
    QApplication::setOrganizationName("Marcus Birkin");
    QApplication::setApplicationVersion(
                QString("%1 (%2 %3 %4)")
                .arg(GIT_CURRENT_SHA1, GIT_DATE_DATE, GIT_DATE_MONTH, GIT_DATE_YEAR));

    // Warning
    QMessageBox msgBox;
    msgBox.setText(QApplication::applicationName());
    msgBox.setInformativeText("Data transmitted is intended to coordinate visual and audio elements of a production and should not be used for safety critical applications.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();

    // Valid and up NIC
    while (!OTP::SocketManager::isValid(Settings::getInstance().getNetworkInterface()))
    {
        auto dialog = new SettingsDialog();
        if (dialog->exec() == QDialog::Rejected)
            return 0;
    }

    // Main Consumer Window
    MainWindow w;
    w.show();

    return a.exec();
}
