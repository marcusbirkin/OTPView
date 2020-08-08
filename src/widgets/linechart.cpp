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
#include "linechart.h"
#include "settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QLegendMarker>

using namespace QtCharts;
using namespace OTP;
using namespace MODULES::STANDARD::VALUES;

#define displayRange 60 // Number of seconds to show

LineChart::LineChart(std::shared_ptr<class OTP::Consumer> otpConsumer,
                     OTP::address_t address,
                     QWidget *parent) :
    chartView(new QChartView(new QChart(), parent)),
    buttonGroup(new QButtonGroup(this)),
    otpConsumer(otpConsumer),
    address(address)
{
    // Layout
    this->setLayout(new QVBoxLayout(this));

    // Line types
    auto buttonLayoutPosition = new QHBoxLayout(this);
    auto buttonLayoutRotation = new QHBoxLayout(this);
    this->layout()->addItem(buttonLayoutPosition);
    this->layout()->addItem(buttonLayoutRotation);
    buttonGroup->setExclusive(true);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(buttonGroup, &QButtonGroup::idToggled, this, &LineChart::buttonToggled);
    #else
    connect(buttonGroup, qOverload<int, bool>(&QButtonGroup::buttonToggled), this, &LineChart::buttonToggled);
    #endif

    setupLineSeries(POSITION, "Position", *buttonLayoutPosition);
    setupLineSeries(POSITION_VELOCITY, "Position Velocity", *buttonLayoutPosition);
    setupLineSeries(POSITION_ACCELERATION, "Position Acceleration", *buttonLayoutPosition);
    setupLineSeries(ROTATION, "Rotation", *buttonLayoutRotation);
    setupLineSeries(ROTATION_VELOCITY, "Rotation Velocity", *buttonLayoutRotation);
    setupLineSeries(ROTATION_ACCELERATION, "Rotation Acceleration", *buttonLayoutRotation);
    buttonGroup->buttons().first()->setChecked(true);

    // Chartview
    this->layout()->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);

    // X Axis Formating
    auto *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("HH:mm:ss");
    axisX->setLabelsAngle(-90);
    axisX->setTitleText("Time");
    chartView->chart()->addAxis(axisX, Qt::AlignBottom);
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
        for (auto series : lineSeries[axis])
            series->attachAxis(axisX);

    // Y Axis Formating
    auto *axisY = new QValueAxis;
    chartView->chart()->addAxis(axisY, Qt::AlignLeft);
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
        for (auto series : lineSeries[axis])
            series->attachAxis(axisY);

    // Legend markers
    for (auto marker : chartView->chart()->legend()->markers())
        QObject::connect(marker, &QLegendMarker::clicked, this, &LineChart::markerClicked);

    // Updates
    auto updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &LineChart::addPoint);
    updateTimer->start(1000);
    addPoint();

    connect(otpConsumer.get(), &OTP::Consumer::updatedPosition, this, &LineChart::updatedPosition);
    connect(otpConsumer.get(), &OTP::Consumer::updatedPositionVelocity, this, &LineChart::updatedPositionVelocity);
    connect(otpConsumer.get(), &OTP::Consumer::updatedPositionAcceleration, this, &LineChart::updatedPositionAcceleration);
    connect(otpConsumer.get(), &OTP::Consumer::updatedRotation, this, &LineChart::updatedRotation);
    connect(otpConsumer.get(), &OTP::Consumer::updatedRotationVelocity, this, &LineChart::updatedRotationVelocity);
    connect(otpConsumer.get(), &OTP::Consumer::updatedRotationAcceleration, this, &LineChart::updatedRotationAcceleration);
}

void LineChart::setupLineSeries(moduleValue_t type, QString label, QHBoxLayout& hl)
{
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
    {
        QString strAxis;
        switch (axis)
        {
            case axis_t::X: strAxis = "X"; break;
            case axis_t::Y: strAxis = "Y"; break;
            case axis_t::Z: strAxis = "Z"; break;
            default: strAxis = "??"; break;
        }

        auto series = new QLineSeries(this);
        series->setUseOpenGL(true);
        series->setName(QString("%1").arg(strAxis));
        lineSeries[axis].insert(type, series);

        series->setVisible(false);

        chartView->chart()->addSeries(series);
    }

    auto button = new QRadioButton(label, this);
    buttonGroup->addButton(button, type);
    hl.addWidget(button, 0, Qt::AlignCenter);
}

void LineChart::markerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    if (!marker) return;

    // Toggle series
    auto series = marker->series();
    series->setVisible(!series->isVisible());
    marker->setVisible(true);

    // Dim the marker, if series is not visible
    qreal alpha = marker->series()->isVisible() ? 1.0 : 0.5;

    QColor color;
    QBrush brush = marker->labelBrush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setLabelBrush(brush);

    brush = marker->brush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setBrush(brush);

    QPen pen = marker->pen();
    color = pen.color();
    color.setAlphaF(alpha);
    pen.setColor(color);
    marker->setPen(pen);
}

void LineChart::buttonToggled(int type, bool checked)
{
    // Markers
    for (auto marker : chartView->chart()->legend()->markers())
    {
        marker->setVisible(false);
        marker->setLabelBrush(QBrush());
        marker->setBrush(QBrush());
        marker->setPen(QPen());
    }

    // Lineseries
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
        lineSeries[axis].value(static_cast<moduleValue_t>(type))->setVisible(checked);
}

void LineChart::redraw()
{
    auto now = QDateTime::currentDateTime();
    if (!chartView->chart()->axes(Qt::Vertical).count()) return;
    auto axisY = static_cast<QValueAxis*>(chartView->chart()->axes(Qt::Vertical).front());
    Q_ASSERT(axisY);
    auto axisX = chartView->chart()->axes(Qt::Horizontal).front();
    Q_ASSERT(axisX);

    // Displayed type
    moduleValue_t type = static_cast<moduleValue_t>(buttonGroup->checkedId());

    // X Axis range
    auto xMin = QDateTime::fromMSecsSinceEpoch(now.addSecs(-displayRange).toMSecsSinceEpoch());
    auto xMax = QDateTime::fromMSecsSinceEpoch(now.addSecs(1).toMSecsSinceEpoch());
    axisX->setRange(xMin, xMax);

    // Y Axis unit
    axisY->setLabelFormat(QString("%g%1").arg(otpConsumer->getUnitString(type, true)));
    axisY->applyNiceNumbers();

    // Y Axis range and prune
    qreal yMin = 0;
    qreal yMax = 0;
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
    {
        for (auto series : lineSeries[axis])
        {
            for (auto point : series->pointsVector())
            {
                // Find Y axis limits
                if (point.x() > xMin.toMSecsSinceEpoch() && point.x() < xMax.toMSecsSinceEpoch())
                {
                    if (!series->isVisible()) continue;
                    yMin = std::min(yMin, point.y());
                    yMax = std::max(yMax, point.y());
                }

                // Prune old (older than 10 seconds out of view)
                if (point.x() < now.addSecs(-displayRange - 10).toMSecsSinceEpoch())
                    series->remove(point);
            }
        }
    }
    auto headRoomPercent = 10;
    if (qFuzzyIsNull(yMin) && qFuzzyIsNull(yMax))
    {
        axisY->setRange(-0.1, 0.1);
    } else {
        axisY->setRange(
                    yMin + (yMin / 100) * headRoomPercent,
                    yMax + (yMax / 100) * headRoomPercent);
    }
}

void LineChart::addPoint()
{
    for (auto axis = axis_t::first; axis < axis_t::count; ++axis)
    {
        updatedPosition(cid_t(), address, axis);
        updatedPositionVelocity(cid_t(), address, axis);
        updatedPositionAcceleration(cid_t(), address, axis);
        updatedRotation(cid_t(), address, axis);
        updatedRotationVelocity(cid_t(), address, axis);
        updatedRotationAcceleration(cid_t(), address, axis);
    }

    redraw();
}

void LineChart::updatedPosition(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][POSITION];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getPosition(
                this->address,
                updatedAxis);
    qreal y = pos.value;
    if (pos.scale == MODULES::STANDARD::PositionModule_t::scale_e::um) y = y / 1000; // to millimeters
    if (pos.scale == MODULES::STANDARD::PositionModule_t::scale_e::mm) y = y / 1000; // to meters

    series->append(x,y);
}

void LineChart::updatedPositionVelocity(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][POSITION_ACCELERATION];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getPositionAcceleration(
                this->address,
                updatedAxis);
    qreal y = pos.value;
    y = y / 1000; // to meters

    series->append(x,y);
}

void LineChart::updatedPositionAcceleration(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][POSITION_VELOCITY];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getPositionVelocity(
                this->address,
                updatedAxis);
    qreal y = pos.value;
    y = y / 1000; // to meters

    series->append(x,y);
}

void LineChart::updatedRotation(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][ROTATION];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getRotation(
                this->address,
                updatedAxis);
    auto y = pos.value;

    series->append(x,y);
}

void LineChart::updatedRotationVelocity(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][ROTATION_VELOCITY];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getRotationVelocity(
                this->address,
                updatedAxis);
    auto y = pos.value;

    series->append(x,y);
}

void LineChart::updatedRotationAcceleration(cid_t cid, address_t updatedAddress, axis_t updatedAxis)
{
    Q_UNUSED(cid)
    if (updatedAddress != address) return;
    auto series = lineSeries[updatedAxis][ROTATION_ACCELERATION];
    auto now = QDateTime::currentDateTime();
    auto x = now.toMSecsSinceEpoch();

    auto pos = this->otpConsumer->getRotationAcceleration(
                this->address,
                updatedAxis);
    auto y = pos.value;

    series->append(x,y);
}
