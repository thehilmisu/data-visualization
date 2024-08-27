#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "GraphWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void updateGraph(); 
    void start();
    void stop();

private:
    GraphWidget *graphWidget;  // The OpenGL 2D rendering widget
    QTimer *timer;
};

#endif // MAINWINDOW_H
