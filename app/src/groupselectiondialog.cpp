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
#include "groupselectiondialog.h"
#include "ui_groupselectiondialog.h"

using namespace OTP;

GroupSelectionDialog::GroupSelectionDialog(QList<OTP::group_t> usedGroups, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupSelectionDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    sbGroup = new GroupSpinBox(usedGroups, this);
    ui->groupBox->layout()->addWidget(sbGroup);
}

GroupSelectionDialog::~GroupSelectionDialog()
{
    delete ui;
}

group_t GroupSelectionDialog::getGroup()
{
    if (!sbGroup) return static_cast<group_t>(RANGES::Group.getMin() - 1);

    return sbGroup->value();
}
