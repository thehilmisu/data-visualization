#include "GraphWidget.h"
#include <QDebug>
#include <QFont>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QOpenGLWidget(parent), minBounds(-1, -1), maxBounds(1, 1), translation(0, 0), zoomLevel(1.0f) {
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
}

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
   // updateTranslationToCenter();
}

void GraphWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    drawDataPoints(painter);

    /// Highlight the hovered point if any
    if (hoveredPoint) {
        QVector2D screenPoint = mapToScreen(hoveredPoint.value());
        painter.setBrush(Qt::yellow);  // Highlight color
        painter.drawEllipse(QPointF(screenPoint.x(), screenPoint.y()), 7, 7);
        painter.drawText(screenPoint.x() + 10, screenPoint.y() - 10,
                         QString("(%1, %2)").arg(hoveredPoint->x()).arg(hoveredPoint->y()));
    }
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    float zoomFactor = 1.0f + event->angleDelta().y() / 2400.0f;
    zoomLevel *= zoomFactor;

    if (zoomLevel < 0.1f) zoomLevel = 0.1f;
    if (zoomLevel > 100.0f) zoomLevel = 100.0f;

    QVector2D mousePos(event->position());

    update();
}

bool GraphWidget::event(QEvent *event) {
    if (event->type() == QEvent::HoverMove) {
        QHoverEvent *hoverEvent = static_cast<QHoverEvent*>(event);
        QVector2D mousePos(hoverEvent->pos());

        bool pointHovered = false;

        for (const auto& point : points) {
            QVector2D screenPoint = mapToScreen(point);
            float distance = (mousePos - screenPoint).length();

            if (distance < 5.0f) {  // Threshold for detecting hover
                pointHovered = true;
                hoveredPoint = point;
                update();
                break;
            }
        }

        // If no point is hovered, reset the hovered point and update
        if (!pointHovered) {
            hoveredPoint.reset();
            update();  // Trigger repaint to remove highlight
        }

        return true;
    }

    // Pass the event on to the base class
    return QOpenGLWidget::event(event);
}

void GraphWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->pos().x() - lastMousePosition.x();
    int dy = event->pos().y() - lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        lastMousePosition = event->pos();
        translation += QVector2D(dx, -dy);  // Adjust translation based on mouse drag
        update();
    }
}

void GraphWidget::drawDataPoints(QPainter &painter, int tickLength, int numTicks) {
    painter.setPen(Qt::white);
    drawXAxis(painter);
    drawYAxis(painter);

    for (const auto &point : points) {
        QVector2D dataPoint = mapToScreen(point);
        if (dataPoint.x() >= margin && dataPoint.x() <= width() - margin &&
            dataPoint.y() >= margin && dataPoint.y() <= height() - margin) {
            painter.setBrush(Qt::red);
            painter.drawEllipse(QPointF(dataPoint.x(), dataPoint.y()), 5, 5);
        }
    }
}

void GraphWidget::drawYAxis(QPainter &painter, int tickLength, int numTicks) {
    updateBounds();
    float yRange = maxBounds.y() - minBounds.y();
    float yTickSpacing = yRange / numTicks;

    float yAxisXPos = margin;

    painter.drawLine(yAxisXPos, height() - margin, yAxisXPos, margin); // Draw the Y-axis line

    for (int i = 0; i <= numTicks; ++i) {
        float yValue = minBounds.y() + i * yTickSpacing;
        float screenY = height() - (margin + (yValue * zoomLevel) + translation.y());

        if (screenY >= margin && screenY <= height() - margin) {
            painter.drawLine(yAxisXPos - tickLength, screenY, yAxisXPos, screenY);
            painter.drawText(yAxisXPos - 35, screenY + 5, QString::number(yValue, 'f', 1));
        }
    }
}

void GraphWidget::drawXAxis(QPainter &painter, int tickLength, int numTicks) {
    updateBounds();
    float xRange = maxBounds.x() - minBounds.x();
    float xTickSpacing = xRange / numTicks;

    float xAxisYPos = height() - margin;

    painter.drawLine(margin, xAxisYPos, width() - margin, xAxisYPos);

    for (int i = 0; i <= numTicks; ++i) {
        float xValue = minBounds.x() + i * xTickSpacing;
        float screenX = margin + (xValue * zoomLevel) + translation.x();

        if (screenX >= margin && screenX <= width() - margin) {
            painter.drawLine(screenX, xAxisYPos, screenX, xAxisYPos - tickLength);
            painter.drawText(screenX, xAxisYPos + 15, QString::number(xValue, 'f', 1));
        }
    }
}

QVector2D GraphWidget::mapToScreen(const QVector2D &point) const {
    float x = margin + (point.x() * zoomLevel) + translation.x();
    float y = margin + (point.y() * zoomLevel) + translation.y();
    return QVector2D(x, height() - y);
}

void GraphWidget::addDataPoint(const QVector2D &point) {
    points.append(point);
    updateBounds();
    adjustZoomAndTranslation();
    update(); // Trigger a redraw
}

void GraphWidget::addDataPoints(const std::vector<QVector2D> &p_points) {
    for (const auto &point : p_points) {
        points.append(point);
    }
    updateBounds();
    adjustZoomAndTranslation();
    update(); // Trigger a redraw
}

void GraphWidget::adjustZoomAndTranslation() {
    float xRange = maxBounds.x() - minBounds.x();
    float yRange = maxBounds.y() - minBounds.y();

    float xZoomFactor = (width() - 2 * margin) / xRange;
    float yZoomFactor = (height() - 2 * margin) / yRange;

    // Ensure the entire data fits within the view
    zoomLevel = qMin(xZoomFactor, yZoomFactor);

    // Adjust translation to center the graph with (0,0) at the bottom-left corner
    updateTranslationToBottomLeft();
}

void GraphWidget::updateTranslationToBottomLeft() {
    QVector2D screenBottomLeft(margin, margin);
    QVector2D dataBottomLeft(minBounds.x(), minBounds.y());
    translation = screenBottomLeft - (dataBottomLeft * zoomLevel);
}

void GraphWidget::rescaleAxes() {
    updateBounds();
    updateTranslationToBottomLeft();
    update();
}

void GraphWidget::clearPoints() {
    points.clear();
    minBounds = QVector2D(-1, -1);
    maxBounds = QVector2D(1, 1);
    translation = QVector2D(0, 0);
    zoomLevel = 1.0f;
    update();
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
