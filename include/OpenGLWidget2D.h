// OpenGLWidget2D.h
#ifndef OPENGLWIDGET2D_H
#define OPENGLWIDGET2D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QMouseEvent>
#include <optional>
#include <QWheelEvent>
#include <vector>

class OpenGLWidget2D : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit OpenGLWidget2D(QWidget *parent = nullptr);
    ~OpenGLWidget2D();

    void addGraph(const std::vector<QVector2D>& data);
    void clearGraphs();
    void rescaleAxes();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    bool event(QEvent *event) override;  // Add this line

private:
    QVector2D mapToScreen(const QVector2D& point) const;
    void drawAxisLabels(QPainter &painter, int tickLength = 5, int numTicks = 10);
    void updateBounds();
    void updateTranslationToCenter();

    QVector<QVector<QVector2D>> graphs;
    QVector2D minBounds, maxBounds;
    QVector2D translation;
    float zoomLevel;
    QPoint lastMousePosition;
    int margin;
    std::optional<QVector2D> hoveredPoint;  // Stores the hovered point if any
};



#endif // OPENGLWIDGET2D_H
