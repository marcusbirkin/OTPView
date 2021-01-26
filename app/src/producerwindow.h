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
#ifndef PRODUCERWINDOW_H
#define PRODUCERWINDOW_H

#include <QMainWindow>
#include <map>
#include "OTPLib.hpp"
#include "groupwindow.h"

namespace Ui {
class ProducerWindow;
}

class ProducerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProducerWindow(QString componentSettingsGroup = "", QMainWindow *parent = nullptr);
    ~ProducerWindow();

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_actionNew_Group_triggered();

private:
    Ui::ProducerWindow *ui;
    void updateStatusBar();
    void updateWindowTitle();

    QString componentSettingsGroup;
    void saveComponentDetails();

    std::shared_ptr<class OTP::Producer> otpProducer;
};

#endif // PRODUCERWINDOW_H
