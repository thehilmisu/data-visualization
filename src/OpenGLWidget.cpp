#include "OpenGLWidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), zoomLevel(1.0f) {}

OpenGLWidget::~OpenGLWidget() {}

void OpenGLWidget::setPoints(const std::vector<QVector3D>& points) {
    this->points = points;
    update(); // Trigger a repaint to show the new points
}

void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);
}

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelViewMatrix.setToIdentity();
    modelViewMatrix.translate(0, 0, -10); // Move back a bit
    modelViewMatrix.rotate(rotation.x(), 1, 0, 0);
    modelViewMatrix.rotate(rotation.y(), 0, 1, 0);
    modelViewMatrix.rotate(rotation.z(), 0, 0, 1);
    modelViewMatrix.scale(zoomLevel);

    // Set up the transformation matrices
    QMatrix4x4 mvpMatrix = projectionMatrix * modelViewMatrix;
    glLoadMatrixf(mvpMatrix.constData());

    // Render the points
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f); // Set point color to white
    for (const auto& point : points) {
        glVertex3f(point.x(), point.y(), point.z());
    }
    glEnd();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->pos().x() - lastMousePosition.x();
    int dy = event->pos().y() - lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        rotation.setX(rotation.x() + dy);
        rotation.setY(rotation.y() + dx);
    } else if (event->buttons() & Qt::RightButton) {
        rotation.setX(rotation.x() + dy);
        rotation.setZ(rotation.z() + dx);
    }

    lastMousePosition = event->pos();
    update();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event) {
    zoomLevel += event->angleDelta().y() / 1200.0f;
    update();
}
