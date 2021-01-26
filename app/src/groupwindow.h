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
#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <memory>
#include "OTPLib.hpp"

namespace Ui {
class GroupWindow;
}

class GroupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GroupWindow(
            std::shared_ptr<class OTP::Producer> otpProducer,
            OTP::system_t system,
            OTP::group_t group,
            QWidget *parent = nullptr);
    ~GroupWindow();

public slots:
    void setSystem(OTP::system_t newSystem);

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

    void on_pbAddPoint_clicked();
    void on_pbRemovePoint_clicked();

    void on_tablePoints_itemSelectionChanged();

    void on_leName_textChanged(const QString &arg1);

    void on_sbParent_valueChanged();

    void on_sbParentSystem_valueChanged(int arg1);

    void on_sbParentGroup_valueChanged(int arg1);

    void on_sbParentPoint_valueChanged(int arg1);

    void on_cbParentDisable_stateChanged(int arg1);

private:
    Ui::GroupWindow *ui;

    std::shared_ptr<class OTP::Producer> otpProducer;
    OTP::system_t system;
    OTP::group_t group;

    QStringList Axes {QStringLiteral("X"),QStringLiteral("Y"),QStringLiteral("Z")};

    enum ScaleHeaders_e
    {
        Scale
    };
    QStringList ScaleHeaders = {
        tr("Scale")
    };

    enum PointsDetailsHeaders_e
    {
        Value,
        Velocity,
        Acceleration
    };
    QStringList PointsDetailsHeaders = {
        tr("Value"),
        tr("Velocity"),
        tr("Acceleration")
    };

    QList<OTP::address_t> getSelectedAddress();
};

#endif // GROUPWINDOW_H
