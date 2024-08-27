#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "OpenGLWidget2D.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void updateGraph(); 

private:
    OpenGLWidget2D *openGLWidget;  // The OpenGL 2D rendering widget
    QTimer *timer;
};

#endif // MAINWINDOW_H
