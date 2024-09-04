#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QVector2D>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), graphWidget(new GraphWidget(this)), timer(new QTimer(this)) {


    setCentralWidget(graphWidget);
    resize(800, 600);
    

    // Adding some test data points
    graphWidget->addDataPoint(QVector2D(25, 5));
    // graphWidget->addDataPoint(QVector2D(5, 3));
    // graphWidget->addDataPoint(QVector2D(0.0f, 0.0f));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Start"), this, &MainWindow::start);
    fileMenu->addAction(tr("&Stop"), this, &MainWindow::stop);
    fileMenu->addAction(tr("&Exit"),this, &MainWindow::close);

    
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);
    timer->setInterval(1000); 

}

MainWindow::~MainWindow() {
    delete timer;
    delete graphWidget;
}

void MainWindow::updateGraph()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    double currentTimeSecs = currentTime.toSecsSinceEpoch();

    // Generate a random data point
    float x = QRandomGenerator::global()->generateDouble() * 100.0;  
    float y = QRandomGenerator::global()->generateDouble() * 100.0;  

    //qDebug() << x << y;
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
    
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.bmp);;All Files (*)");

    if (!imagePath.isEmpty()) {
        //QVector2D coordinate(400.0f, 300.0f);  // Centered coordinate (example)
        //openGLWidget->setImageAtCoordinate(coordinate, imagePath);
    }
}
