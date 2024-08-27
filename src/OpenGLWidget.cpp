#include "OpenGLWidget.h"
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), zoomLevel(1.0f), texture(nullptr), textureNeedsUpdate(false) {}

OpenGLWidget::~OpenGLWidget() {
    delete texture; // Clean up the texture
}

void OpenGLWidget::setPoints(const std::vector<QVector3D>& points) {
    this->points = points;
    update(); // Trigger a repaint to show the new points
}

void OpenGLWidget::setImage(const QString &imagePath) {
    this->imagePath = imagePath;
    textureNeedsUpdate = true;
    update(); // Trigger a repaint to process the image loading
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

    // Load the texture if needed
    if (textureNeedsUpdate) {
        if (texture) {
            delete texture;
            texture = nullptr;
        }
        QImage img(imagePath);
        if (!img.isNull()) {
            texture = new QOpenGLTexture(img.mirrored());
            textureNeedsUpdate = false;
        } else {
            qDebug() << "Failed to load image:" << imagePath;
        }
    }

    // Render the image as a textured quad at each point
    if (texture) {
        texture->bind();

        for (const auto& point : points) {
            glPushMatrix(); // Save the current matrix

            // Translate to the point's position
            glTranslatef(point.x(), point.y(), point.z());

            // Render the quad
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom-left
            glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.0f);  // Bottom-right
            glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.0f);   // Top-right
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.0f);  // Top-left
            glEnd();

            glPopMatrix(); // Restore the previous matrix
        }
    }
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
