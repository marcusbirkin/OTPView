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
#include "systemselectiondialog.h"
#include "ui_systemselectiondialog.h"

using namespace OTP;

SystemSelectionDialog::SystemSelectionDialog(const QList<OTP::system_t> &usedSystems, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemSelectionDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    sbSystem = new SystemSpinBox(usedSystems, this);
    ui->systemBox->layout()->addWidget(sbSystem);
}

SystemSelectionDialog::~SystemSelectionDialog()
{
    delete ui;
}

system_t SystemSelectionDialog::getSystem()
{
    if (!sbSystem) return static_cast<system_t>(RANGES::System.getMin() - 1);

    return sbSystem->value();
}
