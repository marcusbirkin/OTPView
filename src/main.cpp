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
    QApplication::setOrganizationName("Marcus Birkin");
    QApplication::setApplicationVersion(QString("%1 (%2 %3 %4)")
                                        .arg(GIT_CURRENT_SHA1)
                                        .arg(GIT_DATE_DATE)
                                        .arg(GIT_DATE_MONTH)
                                        .arg(GIT_DATE_YEAR));

    // Warning
    QMessageBox msgBox;
    msgBox.setText("Based upon the ESTA 15 June 2020 Draft<br>Document Number: CP/2018-1034r4");
    msgBox.setInformativeText("Data transmitted is intended to coordinate visual and audio elements of a production and should not be used for safety critical applications.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();

    // Valid and up NIC
    while (!Settings::getInstance().getNetworkInterface().isValid() && Settings::getInstance().getNetworkInterface().IsUp)
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
