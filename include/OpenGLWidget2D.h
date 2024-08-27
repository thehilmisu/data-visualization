// OpenGLWidget2D.h
#ifndef OPENGLWIDGET2D_H
#define OPENGLWIDGET2D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QMouseEvent>
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

private:
    std::vector<std::vector<QVector2D>> graphs;
    QVector2D minBounds;
    QVector2D maxBounds;
    QVector2D translation;
    float zoomLevel;

    QPoint lastMousePosition;

    void updateBounds();
    void updateTranslationToCenter();
    QVector2D mapToScreen(const QVector2D& point) const;
    void drawAxisLabels(QPainter &painter, int tickLength = 5, int numTicks = 10);
};

#endif // OPENGLWIDGET2D_H
