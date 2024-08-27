#include "OpenGLWidget2D.h"
#include <QDebug>
#include <QPainter>
#include <QFont>
#include <cmath>  // Include this to use fabs

OpenGLWidget2D::OpenGLWidget2D(QWidget *parent)
    : QOpenGLWidget(parent), minBounds(-1, -1), maxBounds(1, 1), translation(0, 0), zoomLevel(1.0f) {}

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
}

void OpenGLWidget2D::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
}

void OpenGLWidget2D::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QVector2D delta = QVector2D(event->pos() - lastMousePosition);
        translation += delta / zoomLevel;  // Adjust translation based on zoom level
        lastMousePosition = event->pos();
        update();
    }
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
    graphs.push_back(data);
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
    // Calculate the midpoint of the data bounds
    QVector2D dataCenter = (minBounds + maxBounds) / 2.0f;

    // Calculate the screen center
    QVector2D screenCenter(width() / 2.0f, height() / 2.0f);

    // Adjust the translation to center the graph
    translation = screenCenter - mapToScreen(dataCenter);
}

void OpenGLWidget2D::drawAxisLabels(QPainter &painter, int tickLength, int numTicks) {
    const int margin = 75;  
    float xRange = maxBounds.x() - minBounds.x();
    float yRange = maxBounds.y() - minBounds.y();

    float xTickSpacing = xRange / numTicks;
    float yTickSpacing = yRange / numTicks;

    painter.setPen(Qt::white);

    // Draw X-axis labels, line, and corresponding points
    float xAxisYPos = height() - margin + translation.y() * zoomLevel;  // Y position of X-axis considering translation
    float xAxisStartX = margin + translation.x() * zoomLevel;
    float xAxisEndX = width() - margin + translation.x() * zoomLevel;

    painter.drawLine(xAxisStartX, xAxisYPos, xAxisEndX, xAxisYPos);  // Draw the X-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float xValue = minBounds.x() + i * xTickSpacing;
        float screenX = margin + (xValue - minBounds.x()) / xRange * (width() - 2 * margin) * zoomLevel + translation.x();

        if (screenX >= margin && screenX <= width() - margin) {
            // Draw tick marks
            painter.drawLine(screenX, xAxisYPos, screenX, xAxisYPos - tickLength); 
            
            // Draw X-axis label
            painter.drawText(screenX, xAxisYPos + 15, QString::number(xValue, 'f', 2));
        }
    }

    // Draw Y-axis labels, line, and corresponding points
    float yAxisXPos = margin + translation.x() * zoomLevel;  // X position of Y-axis considering translation
    float yAxisStartY = margin + translation.y() * zoomLevel;
    float yAxisEndY = height() - margin + translation.y() * zoomLevel;

    painter.drawLine(yAxisXPos, yAxisStartY, yAxisXPos, yAxisEndY);  // Draw the Y-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float yValue = minBounds.y() + i * yTickSpacing;
        float screenY = height() - margin - (yValue - minBounds.y()) / yRange * (height() - 2 * margin) * zoomLevel + translation.y();

        if (screenY >= margin && screenY <= height() - margin) {
            // Draw tick marks
            painter.drawLine(yAxisXPos, screenY, yAxisXPos + tickLength, screenY); 
            
            // Draw Y-axis label
            painter.drawText(yAxisXPos + 5, screenY, QString::number(yValue, 'f', 2));
        }
    }

    // Draw data points
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
    const int margin = 75;  
    float x = margin + ((point.x() - minBounds.x()) / (maxBounds.x() - minBounds.x())) * (width() - 2 * margin) * zoomLevel + translation.x();
    float y = height() - margin - ((point.y() - minBounds.y()) / (maxBounds.y() - minBounds.y())) * (height() - 2 * margin) * zoomLevel + translation.y();
    return QVector2D(x, y);
}

