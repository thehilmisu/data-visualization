#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QVector2D>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), openGLWidget(new OpenGLWidget2D(this)), timer(new QTimer(this)) {

    std::vector<QVector2D> data = {QVector2D(5, 5), QVector2D(8, 8), QVector2D(20, 5)};
    
    openGLWidget->addGraph(data);
    openGLWidget->rescaleAxes();
    setCentralWidget(openGLWidget);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);

    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);
    timer->start(1000);  // Start the timer with a 1-second interval

}

MainWindow::~MainWindow() {
    delete timer;
    delete openGLWidget;
}

void MainWindow::updateGraph()
{
    // Generate a random data point
    float x = QRandomGenerator::global()->generateDouble() * 100.0;  // Scales the random double between 0.0 and 10.0
    float y = QRandomGenerator::global()->generateDouble() * 100.0;  // Scales the random double between 0.0 and 10.0

    // Add the data point to the graph
    openGLWidget->addDataPoint(QVector2D(x, y));
}

void MainWindow::openFile() {
    
    // QString imagePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.bmp);;All Files (*)");

    // if (!imagePath.isEmpty()) {
    //     QVector2D coordinate(400.0f, 300.0f);  // Centered coordinate (example)
    //     openGLWidget->setImageAtCoordinate(coordinate, imagePath);
    // }
}
