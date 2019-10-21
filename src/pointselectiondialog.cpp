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
#include "pointselectiondialog.h"
#include "ui_pointselectiondialog.h"

using namespace ACN::OTP;

PointSelectionDialog::PointSelectionDialog(QList<ACN::OTP::point_t> usedPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointSelectionDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    sbPoint = new PointSpinBox(usedPoint, this);
    ui->groupBox->layout()->addWidget(sbPoint);
}

PointSelectionDialog::~PointSelectionDialog()
{
    delete ui;
}

point_t PointSelectionDialog::getPoint()
{
    if (!sbPoint) return static_cast<point_t>(RANGES::Point.getMin() - 1);

    return sbPoint->value();
}
