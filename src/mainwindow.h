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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <memory>
#include "libs/OTPLib/otp.hpp"
#include "producerwindow.h"
#include "systemwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_actionSettings_triggered();
    void on_actionNew_Producer_triggered();
    void on_actionNew_Consumer_triggered();
    void on_tvComponents_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    void updateStatusBar();
    void saveComponentDetails();

    bool openSystemWindow(OTP::system_t system);

    std::shared_ptr<class OTP::Consumer> otpConsumer;
    QList<ProducerWindow*> producerWindows;
};

#endif // MAINWINDOW_H
