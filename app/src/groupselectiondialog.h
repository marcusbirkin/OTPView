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
#ifndef GROUPSELECTIONDIALOG_H
#define GROUPSELECTIONDIALOG_H

#include <QDialog>
#include <QList>
#include "OTPLib.hpp"
#include "widgets/groupspinbox.h"

namespace Ui {
class GroupSelectionDialog;
}

class GroupSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupSelectionDialog(QList<OTP::group_t> usedGroups, QWidget *parent = nullptr);
    ~GroupSelectionDialog();

    OTP::group_t getGroup();

private slots:

private:
    Ui::GroupSelectionDialog *ui;
    GroupSpinBox *sbGroup = nullptr;
};

#endif // GROUPSELECTIONDIALOG_H
