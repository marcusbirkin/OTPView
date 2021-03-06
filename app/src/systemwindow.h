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
#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "OTPLib.hpp"

namespace Ui {
class SystemWindow;
}

class SystemWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SystemWindow(
            std::shared_ptr<class OTP::Consumer> otpConsumer,
            OTP::system_t system,
            QWidget *parent = nullptr);
    ~SystemWindow();

    OTP::system_t getSystem() const { return system; }

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::SystemWindow *ui;

    std::shared_ptr<class OTP::Consumer> otpConsumer;
    OTP::system_t system;
};

#endif // SYSTEMWINDOW_H
