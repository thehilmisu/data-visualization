#include "GraphWidget.h"
#include <QFont>
#include <QPainter>
#include <QDebug>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent), minBounds(-1, -1), maxBounds(1, 1), margin(50.0f), zoomLevel(1.0f) {
        setAttribute(Qt::WA_Hover, true);
        setMouseTracking(true);
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    // float zoomFactor = 1.0f + event->angleDelta().y() / 2400.0f;
    // zoomLevel *= zoomFactor;

    // if (zoomLevel < 0.1f) zoomLevel = 0.1f;
    // if (zoomLevel > 100.0f) zoomLevel = 100.0f;

    // QVector2D mousePos(event->position());

    // update();
}

void GraphWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw axis lines
    drawAxisLines(painter);

    // Draw tick marks
    drawTickMarks(painter);
   
    // Draw points
    drawPoints(painter);
    
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateBounds();  
    update();  // Repaint the widget to show updated lines and points
}

void GraphWidget::addPoint(float x, float y) {
   
    points.append(QVector2D(x,y));
    adjustZoomAndTranslation();
    update();

    qDebug() << x << "," << y;
}
void GraphWidget::addPoint(QVector2D point)
{
    points.append(point);
    adjustZoomAndTranslation();
    update();

    qDebug() << point.x() << "," << point.y();
}

void GraphWidget::drawPoints(QPainter &painter) {
    int pointsToShow = 10;  // Number of points to show
    int startIndex = qMax(0, points.size() - pointsToShow);  // Start index for the last 10 points

    for (int i = startIndex; i < points.size(); ++i) {
        QVector2D dataPoint = mapToScreen(points[i]);
        if (dataPoint.x() >= margin && dataPoint.x() <= width() - margin &&
            dataPoint.y() >= margin && dataPoint.y() <= height() - margin) {
            painter.setBrush(Qt::red);
            painter.drawEllipse(QPointF(dataPoint.x(), dataPoint.y()), 5, 5);
        }
    }
}


void GraphWidget::drawAxisLines(QPainter &painter) {
    painter.setPen(Qt::black);

    // Draw line from bottom left to top left (Y-axis)
    painter.drawLine(QPointF(margin, height()-margin), QPointF(margin, margin));

    // Draw line from bottom left to bottom right (X-axis)
    painter.drawLine(QPointF(margin, height()-margin), QPointF(width()-margin, height()-margin));
}

void GraphWidget::drawTickMarks(QPainter &painter)
{
    updateBounds();

    int numTicks = 20;
    int tickLength = 5;
    float xRange = maxBounds.x() ;//- minBounds.x();
    float xTickSpacing = xRange / numTicks;

    float xAxisYPos = height() - margin;

    QFont font = painter.font();
    font.setPointSize(8);  
    painter.setFont(font);

    for (int i = 0; i <= numTicks; ++i) {
        float xValue = i * xTickSpacing;
        float screenX = margin + (xValue * zoomLevel) ;

        if (screenX >= margin && screenX <= width() - margin) {
            painter.drawLine(screenX, xAxisYPos, screenX, xAxisYPos + tickLength);
            
            // Save the current painter state
            painter.save();
            
            // Move the painter's origin to the text position and rotate
            painter.translate(screenX, xAxisYPos + 35);
            painter.rotate(-90);

            // Draw the rotated text
            painter.drawText(0, 0, QString::number(xValue, 'f', 1));
            
            // Restore the painter state to what it was before the rotation
            painter.restore();
        }
    }

    float yRange = maxBounds.y() ;//- minBounds.y();
    float yTickSpacing = yRange / numTicks;

    float yAxisXPos = margin;

    font.setPointSize(8);  
    painter.setFont(font);

    for (int i = 0; i <= numTicks; ++i) {
        float yValue = i * yTickSpacing;
        float screenY = height() - (margin + (yValue * zoomLevel));

        if (screenY >= margin && screenY <= height() - margin) {
            painter.drawLine(yAxisXPos - tickLength, screenY, yAxisXPos, screenY);
            painter.drawText(yAxisXPos - 35, screenY + 5, QString::number(yValue, 'f', 1));
        }
    }
}

void GraphWidget::updateBounds() {
    if (points.isEmpty()) {
        minBounds = QVector2D(-1, -1);
        maxBounds = QVector2D(1, 1);
        return;
    }

    minBounds = QVector2D(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    maxBounds = QVector2D(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (const auto &point : points) {
        if (point.x() < minBounds.x())
            minBounds.setX(point.x());
        if (point.y() < minBounds.y())
            minBounds.setY(point.y());
        if (point.x() > maxBounds.x())
            maxBounds.setX(point.x());
        if (point.y() > maxBounds.y())
            maxBounds.setY(point.y());
    }
}

QVector2D GraphWidget::mapToScreen(const QVector2D &point) const {
    float x = margin + (point.x() * zoomLevel) ;//+ translation.x();
    float y = margin + (point.y() * zoomLevel) ;//+ translation.y();
    return QVector2D(x, height() - y);
}

void GraphWidget::adjustZoomAndTranslation() {
    float xRange = maxBounds.x() - minBounds.x();
    float yRange = maxBounds.y() - minBounds.y();

    float xZoomFactor = (width() - 2 * margin) / xRange;
    float yZoomFactor = (height() - 2 * margin) / yRange;

    // Ensure the entire data fits within the view
    zoomLevel = qMin(xZoomFactor, yZoomFactor);

}

