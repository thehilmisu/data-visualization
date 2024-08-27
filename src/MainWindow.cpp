#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), openGLWidget(new OpenGLWidget2D(this)) {

    std::vector<QVector2D> data = {QVector2D(5, 5), QVector2D(8, 8), QVector2D(20, 5)};
    
    openGLWidget->addGraph(data);
    openGLWidget->rescaleAxes();
    setCentralWidget(openGLWidget);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);
}

MainWindow::~MainWindow() {
    delete openGLWidget;
}

void MainWindow::openFile() {
    
    // QString imagePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.bmp);;All Files (*)");

    // if (!imagePath.isEmpty()) {
    //     QVector2D coordinate(400.0f, 300.0f);  // Centered coordinate (example)
    //     openGLWidget->setImageAtCoordinate(coordinate, imagePath);
    // }
}
