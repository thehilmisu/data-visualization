#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QVector2D>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), graphWidget(new GraphWidget(this)), timer(new QTimer(this)) {

    std::vector<QVector2D> data = {QVector2D(5, 5), QVector2D(8, 8), QVector2D(20, 5)};
    
    graphWidget->addGraph(data);
    graphWidget->rescaleAxes();
    setCentralWidget(graphWidget);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Start"), this, &MainWindow::start);
    fileMenu->addAction(tr("&Stop"), this, &MainWindow::stop);

    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);
    timer->setInterval(1000); 

}

MainWindow::~MainWindow() {
    delete timer;
    delete graphWidget;
}

void MainWindow::updateGraph()
{
    // Generate a random data point
    float x = QRandomGenerator::global()->generateDouble() * 100.0;  // Scales the random double between 0.0 and 10.0
    float y = QRandomGenerator::global()->generateDouble() * 100.0;  // Scales the random double between 0.0 and 10.0

    // Add the data point to the graph
    graphWidget->addDataPoint(QVector2D(x, y));
}

void MainWindow::start()
{
    timer->start();
}

void MainWindow::stop()
{
    timer->stop();
}

void MainWindow::openFile() {
    
    // QString imagePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.bmp);;All Files (*)");

    // if (!imagePath.isEmpty()) {
    //     QVector2D coordinate(400.0f, 300.0f);  // Centered coordinate (example)
    //     openGLWidget->setImageAtCoordinate(coordinate, imagePath);
    // }
}
