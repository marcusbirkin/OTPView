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
#ifndef SYSTEMSELECTIONDIALOG_H
#define SYSTEMSELECTIONDIALOG_H

#include <QDialog>
#include <QList>
#include "libs/OTPLib/otp.hpp"
#include "widgets/systemspinbox.h"

namespace Ui {
class SystemSelectionDialog;
}

class SystemSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSelectionDialog(QList<ACN::OTP::system_t> usedSystems, QWidget *parent = nullptr);
    ~SystemSelectionDialog();

    ACN::OTP::system_t getSystem();

private slots:

private:
    Ui::SystemSelectionDialog *ui;
    SystemSpinBox *sbSystem = nullptr;
};

#endif // GROUPSELECTIONDIALOG_H
