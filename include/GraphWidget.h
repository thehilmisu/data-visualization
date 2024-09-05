#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QPointF>
#include <QVector>
#include <QVector2D>
#include <QWheelEvent>

class GraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

public slots:
    void addPoint(float x, float y);
    void addPoint(QVector2D point);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QVector<QVector2D>  relativePoints;  
    QVector<QVector2D> points;
    QVector2D minBounds;
    QVector2D maxBounds;
    QVector2D translation;
    float zoomLevel;  
    void drawAxisLines(QPainter &painter);
    void drawTickMarks(QPainter &painter);
    void drawPoints(QPainter &painter);
    void updateBounds();
    void adjustZoomAndTranslation();
    void updateTranslationToBottomLeft();
    QVector2D mapToScreen(const QVector2D &point) const;
    float margin;
};

#endif // GRAPHWIDGET_H
