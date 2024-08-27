#include "OpenGLWidget2D.h"
#include <QDebug>
#include <QPainter>
#include <QFont>
#include <cmath>  
#include <iostream>

OpenGLWidget2D::OpenGLWidget2D(QWidget *parent)
    : QOpenGLWidget(parent), minBounds(-1, -1), maxBounds(1, 1), translation(0, 0), zoomLevel(1.0f), margin(75) {
    setMouseTracking(true); 
}

OpenGLWidget2D::~OpenGLWidget2D() {}

void OpenGLWidget2D::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0f);
}

void OpenGLWidget2D::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    updateTranslationToCenter();
}

void OpenGLWidget2D::paintGL() {
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


void OpenGLWidget2D::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
}

bool OpenGLWidget2D::event(QEvent *event) {
    if (event->type() == QEvent::HoverMove) {
        QHoverEvent *hoverEvent = static_cast<QHoverEvent*>(event);
        QVector2D mousePos(hoverEvent->pos());

        bool pointHovered = false;

        // Iterate through the graphs and check if the mouse is near any point
        for (const auto& graph : graphs) {
            for (const auto& point : graph) {
                QVector2D screenPoint = mapToScreen(point);
                float distance = (mousePos - screenPoint).length();

                if (distance < 5.0f) {  // Threshold for detecting hover
                    pointHovered = true;
                    hoveredPoint = point;
                    update();  // Trigger repaint to highlight the point
                    break;
                }
            }
            if (pointHovered) break;
        }

        // If no point is hovered, reset the hovered point and update
        if (!pointHovered) {
            hoveredPoint.reset();
            update();  // Trigger repaint to remove highlight
        }

        return true;  // Indicate that the event has been handled
    }

    // Pass the event on to the base class
    return QOpenGLWidget::event(event);
}


void OpenGLWidget2D::mouseMoveEvent(QMouseEvent *event) {
    QVector2D mousePos(event->pos());

    bool pointHovered = false;

    // Iterate through the graphs and check if the mouse is near any point
    for (const auto& graph : graphs) {
        for (const auto& point : graph) {
            QVector2D screenPoint = mapToScreen(point);
            float distance = (mousePos - screenPoint).length();

            if (distance < 5.0f) {  // Threshold for detecting hover
                pointHovered = true;
                hoveredPoint = point;
                update();  // Trigger repaint to highlight the point
                break;
            }
        }
        if (pointHovered) break;
    }

    // If no point is hovered, reset the hovered point and update
    if (!pointHovered) {
        hoveredPoint.reset();
        update();  // Trigger repaint to remove highlight
    }

    // No need to update `lastMousePosition` unless used elsewhere for dragging or other interactions
}



void OpenGLWidget2D::wheelEvent(QWheelEvent *event) {
    float zoomFactor = 1.0f + event->angleDelta().y() / 2400.0f;
    QVector2D mousePos(event->position());  // Use position() instead of pos()
    
    QVector2D beforeScale = (mousePos - translation) / zoomLevel;
    zoomLevel *= zoomFactor;

    if (zoomLevel < 0.1f) zoomLevel = 0.1f;
    if (zoomLevel > 100.0f) zoomLevel = 100.0f;

    QVector2D afterScale = (mousePos - translation) / zoomLevel;
    translation += (beforeScale - afterScale) * zoomLevel;

    update();
}

void OpenGLWidget2D::addGraph(const std::vector<QVector2D>& data) {
    QVector<QVector2D> qtData(data.begin(), data.end());
    graphs.push_back(qtData);
    updateBounds();
    updateTranslationToCenter();
    update();
}

void OpenGLWidget2D::clearGraphs() {
    graphs.clear();
    minBounds = QVector2D(-1, -1);
    maxBounds = QVector2D(1, 1);
    translation = QVector2D(0, 0);
    zoomLevel = 1.0f;
    update();
}

void OpenGLWidget2D::rescaleAxes() {
    updateBounds();
    updateTranslationToCenter();
    update();
}

void OpenGLWidget2D::updateBounds() {
    if (graphs.empty()) {
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

void OpenGLWidget2D::updateTranslationToCenter() {
    // Set translation to position (0, 0) at the bottom-left corner
    translation = QVector2D(margin, height() - margin);
}

void OpenGLWidget2D::drawAxisLabels(QPainter &painter, int tickLength, int numTicks) {
    float xRange = maxBounds.x() - 0;  // Start from 0 instead of minBounds.x()
    float yRange = maxBounds.y() - 0;  // Start from 0 instead of minBounds.y()

    float xTickSpacing = xRange / numTicks;
    float yTickSpacing = yRange / numTicks;

    painter.setPen(Qt::white);

    // Draw X-axis labels, line, and corresponding points
    float xAxisYPos = height() - margin;  // Y position of X-axis at the bottom
    float xAxisStartX = margin;
    float xAxisEndX = width() - margin;

    painter.drawLine(xAxisStartX, xAxisYPos, xAxisEndX, xAxisYPos);  // Draw the X-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float xValue = 0 + i * xTickSpacing;  // Start from 0 and scale up
        float screenX = margin + (xValue / xRange) * (width() - 2 * margin) * zoomLevel;

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
        float screenY = height() - margin - (yValue / yRange) * (height() - 2 * margin) * zoomLevel;

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
            }
        }
    }
}


QVector2D OpenGLWidget2D::mapToScreen(const QVector2D& point) const {
    // X-axis: map from data point to screen space
    float x = margin + (point.x() / maxBounds.x()) * (width() - 2 * margin) * zoomLevel;
    // Y-axis: invert the Y-axis mapping because the origin is now at the bottom-left
    float y = height() - margin - (point.y() / maxBounds.y()) * (height() - 2 * margin) * zoomLevel;
    return QVector2D(x, y);
}


