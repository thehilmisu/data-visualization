#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector>
#include <QPainter>
#include <optional>  
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>


class GraphWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    //void addGraph(const std::vector<QVector2D>& points);  // Declaration of addGraph
    void addDataPoint(const QVector2D& point);
    void addDataPoints(const std::vector<QVector2D>& points);
    void clearPoints();
    void rescaleAxes();  // Declaration of rescaleAxes

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // void mousePressEvent(QMouseEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;
     void wheelEvent(QWheelEvent *event) override;
     bool event(QEvent *event) override;

private:
    void drawDataPoints(QPainter &painter, int tickLength = 5, int numTicks = 10);
    void drawXAxis(QPainter &painter, int tickLength = 5, int numTicks = 10);
    void drawYAxis(QPainter &painter, int tickLength = 5, int numTicks = 10);
    void updateBounds(); // Add this declaration for the empty updateBounds function
    void adjustZoomAndTranslation();
    void updateTranslationToCenter();
    QVector2D mapToScreen(const QVector2D& point) const;

    QVector<QVector2D> points;
    QVector2D minBounds;
    QVector2D maxBounds;
    QVector2D translation;
    float zoomLevel;
    QPoint lastMousePosition;

    std::optional<QVector2D> hoveredPoint;

    const int margin = 75;

};

#endif // GRAPHWIDGET_H