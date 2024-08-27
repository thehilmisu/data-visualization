#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLTexture>
#include <QImage>
#include <vector>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void setPoints(const std::vector<QVector3D>& points);
    void setImage(const QString &imagePath); // Function to load an image

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPoint lastMousePosition;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    QVector3D rotation;
    float zoomLevel;

    std::vector<QVector3D> points; // Store the points to be rendered

    QOpenGLTexture *texture;       // Texture for rendering the image
    QVector3D imagePosition;       // Position of the image in 3D space
    QString imagePath;             // Path to the image
    bool textureNeedsUpdate;       // Flag to check if texture needs to be loaded
};

#endif // OPENGLWIDGET_H
