#include "GraphWidget.h"
#include <QDebug>
#include <QPainter>
#include <QFont>
#include <cmath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <optional>

GraphWidget::GraphWidget(QWidget *parent)
    : QOpenGLWidget(parent), minBounds(-1, -1), maxBounds(1, 1), translation(0, 0), zoomLevel(1.0f) {}

GraphWidget::~GraphWidget() {}

void GraphWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0f);
}

void GraphWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    updateTranslationToCenter();
}

void GraphWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
  
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    drawAxisLabels(painter);

     // Highlight the hovered point if any
    if (hoveredPoint) {
        QVector2D screenPoint = mapToScreen(hoveredPoint.value());
        painter.setBrush(Qt::yellow);  // Highlight color
        painter.drawEllipse(QPointF(screenPoint.x(), screenPoint.y()), 7, 7);
        painter.drawText(screenPoint.x() + 10, screenPoint.y() - 10,
                         QString("(%1, %2)").arg(hoveredPoint->x()).arg(hoveredPoint->y()));
    }
}

void GraphWidget::drawAxisLabels(QPainter &painter, int tickLength, int numTicks) {
    float xRange = maxBounds.x() - 0;  // Start from 0 instead of minBounds.x()
    float yRange = maxBounds.y() - 0;  // Start from 0 instead of minBounds.y()

    float xTickSpacing = xRange / numTicks;
    float yTickSpacing = yRange / numTicks;

    painter.setPen(Qt::white);

    // Draw X-axis labels, line, and corresponding points
    float xAxisYPos = height() - margin;  // Y position of X-axis at the bottom
    float xAxisStartX = margin;
    float xAxisEndX = width();

    painter.drawLine(xAxisStartX, xAxisYPos, xAxisEndX, xAxisYPos);  // Draw the X-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float xValue = 0 + i * xTickSpacing;  // Start from 0 and scale up
        float screenX = mapToScreen(QVector2D(xValue, 0)).x();  // Use mapToScreen

        if (screenX >= margin && screenX <= width() - margin) {
            // Draw tick marks
            painter.drawLine(screenX, xAxisYPos, screenX, xAxisYPos - tickLength); 
            
            // Draw X-axis label
            painter.drawText(screenX, xAxisYPos + 15, QString::number(xValue, 'f', 2));
        }
    }

    // Draw Y-axis labels, line, and corresponding points
    float yAxisXPos = margin;  // X position of Y-axis at the left
    float yAxisStartY = height() - margin;
    float yAxisEndY = margin;

    painter.drawLine(yAxisXPos, yAxisStartY, yAxisXPos, yAxisEndY);  // Draw the Y-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float yValue = 0 + i * yTickSpacing;  // Start from 0 and scale up
        float screenY = mapToScreen(QVector2D(0, yValue)).y();  // Use mapToScreen

        if (screenY >= margin && screenY <= height() - margin) {
            // Draw tick marks
            painter.drawLine(yAxisXPos, screenY, yAxisXPos + tickLength, screenY); 
            
            // Draw Y-axis label
            painter.drawText(yAxisXPos + 5, screenY, QString::number(yValue, 'f', 2));
        }
    }

    // Draw data points with correct mapping
    for (const auto& graph : graphs) {
        for (const auto& point : graph) {
            QVector2D dataPoint = mapToScreen(point);
            if (dataPoint.x() >= margin && dataPoint.x() <= width() - margin &&
                dataPoint.y() >= margin && dataPoint.y() <= height() - margin) {
                painter.setBrush(Qt::red);
                painter.drawEllipse(QPointF(dataPoint.x(), dataPoint.y()), 5, 5);
                painter.drawText(dataPoint.x() + 10, dataPoint.y() - 10,
                         QString("(%1, %2)").arg(point.x()).arg(point.y()));
            }
        }
    }
}

QVector2D GraphWidget::mapToScreen(const QVector2D& point) const {
    float x = margin + (point.x() - minBounds.x()) * zoomLevel + translation.x();
    float y = height() - margin - (point.y() - minBounds.y()) * zoomLevel - translation.y();
    return QVector2D(x, y);
}


void GraphWidget::addDataPoint(const QVector2D& point) {
    if (graphs.isEmpty()) {
        graphs.append(QVector<QVector2D>());
    }
    graphs[0].append(point);

    updateBounds();

    adjustZoomAndTranslation();

    update();  // Trigger a redraw
}

void GraphWidget::addDataPoints(const std::vector<QVector2D>& points) {
    if (graphs.isEmpty()) {
        graphs.append(QVector<QVector2D>());
    }
    bool requiresZoomOut = false;
    for (const auto& point : points) {
        graphs[0].append(point);
        updateBounds();
    }

   
    adjustZoomAndTranslation();
   
    update();  // Trigger a redraw
}

void GraphWidget::addGraph(const std::vector<QVector2D>& points) {
    QVector<QVector2D> qtData(points.begin(), points.end());
    graphs.push_back(qtData);
    updateBounds();
    rescaleAxes();
    update();
}

void GraphWidget::adjustZoomAndTranslation() {
    float xRange = maxBounds.x() - minBounds.x();
    float yRange = maxBounds.y() - minBounds.y();

    float xZoomFactor = (width() - 2 * margin) / xRange;
    float yZoomFactor = (height() - 2 * margin) / yRange;

    // The zoom level should ensure the entire data fits within the view
    zoomLevel = qMin(xZoomFactor, yZoomFactor);

    // After adjusting the zoom level, update the translation
    updateTranslationToCenter();
}

void GraphWidget::updateTranslationToCenter() {
    QVector2D dataCenter = (minBounds + maxBounds) / 2.0f;
    QVector2D screenCenter(width() / 2.0f, height() / 2.0f);

    // Adjust translation based on the zoom level
    translation = screenCenter - (dataCenter * zoomLevel);
}

void GraphWidget::rescaleAxes() {
    updateBounds();
    updateTranslationToCenter();
    update();
}

void GraphWidget::clearGraphs() {
    graphs.clear();
    minBounds = QVector2D(-1, -1);
    maxBounds = QVector2D(1, 1);
    translation = QVector2D(0, 0);
    zoomLevel = 1.0f;
    update();
}

void GraphWidget::updateBounds() {
    if (graphs.isEmpty()) {
        minBounds = QVector2D(-1, -1);
        maxBounds = QVector2D(1, 1);
        return;
    }

    minBounds = QVector2D(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    maxBounds = QVector2D(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (const auto& graph : graphs) {
        for (const auto& point : graph) {
            if (point.x() < minBounds.x()) minBounds.setX(point.x());
            if (point.y() < minBounds.y()) minBounds.setY(point.y());
            if (point.x() > maxBounds.x()) maxBounds.setX(point.x());
            if (point.y() > maxBounds.y()) maxBounds.setY(point.y());
        }
    }
}
