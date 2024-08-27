#ifndef OPENGLWIDGET2D_H
#define OPENGLWIDGET2D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector>
#include <QPainter>
#include <optional>  // Include for std::optional
#include <QMouseEvent>
#include <QWheelEvent>

class OpenGLWidget2D : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit OpenGLWidget2D(QWidget *parent = nullptr);
    ~OpenGLWidget2D();

    void addGraph(const std::vector<QVector2D>& points);  // Declaration of addGraph
    void addDataPoint(const QVector2D& point);
    void addDataPoints(const std::vector<QVector2D>& points);
    void clearGraphs();
    void rescaleAxes();  // Declaration of rescaleAxes

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

private:
    void drawAxisLabels(QPainter &painter, int tickLength = 5, int numTicks = 10);
    void updateBounds(const QVector2D& point, bool& requiresZoomOut);
    void updateBounds(); // Add this declaration for the empty updateBounds function
    void adjustZoomAndTranslation();
    void updateTranslationToCenter();
    QVector2D mapToScreen(const QVector2D& point) const;

    QVector<QVector<QVector2D>> graphs;
    QVector2D minBounds;
    QVector2D maxBounds;
    QVector2D translation;
    float zoomLevel;
    QPoint lastMousePosition;

    std::optional<QVector2D> hoveredPoint;

    const int margin = 50;
};

#endif // OPENGLWIDGET2D_H
