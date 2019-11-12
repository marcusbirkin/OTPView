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
#ifndef POINTSELECTIONDIALOG_H
#define POINTSELECTIONDIALOG_H

#include <QDialog>
#include <QList>
#include "libs/OTPLib/otp.hpp"
#include "widgets/pointspinbox.h"

namespace Ui {
class PointSelectionDialog;
}

class PointSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PointSelectionDialog(QList<ACN::OTP::point_t> usedPoint, QWidget *parent = nullptr);
    ~PointSelectionDialog();

    ACN::OTP::point_t getPoint();

private slots:

private:
    Ui::PointSelectionDialog *ui;
    PointSpinBox *sbPoint = nullptr;
};

#endif // POINTSELECTIONDIALOG_H
