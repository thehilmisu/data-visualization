#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QVector2D>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>
#include <vector>
#include "pbPlots.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), graphWidget(new GraphWidget(this)), timer(new QTimer(this))
{

    // setCentralWidget(graphWidget);
    resize(800, 600);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Start"), this, &MainWindow::start);
    fileMenu->addAction(tr("&Stop"), this, &MainWindow::stop);
    fileMenu->addAction(tr("&Exit"), this, &MainWindow::close);

    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);
    timer->setInterval(1000);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Create a QLabel for displaying the image
    QLabel *label = new QLabel(this);

    bool success;
    StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    std::vector<double> xs{-2, -1, 0, 1, 2};
    std::vector<double> ys{2, -1, -2, -1, 2};

    success = DrawScatterPlot(imageReference, 600, 400, &xs, &ys, errorMessage);

    if (success)
    {
        std::vector<double> *originalPngData = ConvertToPNG(imageReference->image);

        // Convert std::vector<double> to std::vector<unsigned char>
        std::vector<unsigned char> pngData;
        pngData.reserve(originalPngData->size()); // Reserve enough space

        for (double value : *originalPngData)
        {
            pngData.push_back(static_cast<unsigned char>(value));
        }

        // Now you can use pngData to load the image
        QByteArray byteArray(reinterpret_cast<const char *>(pngData.data()), pngData.size());
        QPixmap pixmap;
        if (!pixmap.loadFromData(byteArray, "PNG"))
        {
            qWarning("Failed to load PNG data!");
        }
        else
        {

            label->setPixmap(pixmap);
            label->resize(pixmap.size());
            // label.show();
        }

        // Add the QLabel to the layout
        layout->addWidget(label);

        // Set the central widget of the MainWindow
        setCentralWidget(centralWidget);
    }
    else
    {
        qWarning("Failed to draw scatter plot!");
    }
}

MainWindow::~MainWindow()
{
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

    // qDebug() << x << y;
    //  Add the data point to the graph
    graphWidget->addPoint(QVector2D(x, y));
}

void MainWindow::start()
{
    timer->start();
}

void MainWindow::stop()
{
    timer->stop();
}

void MainWindow::openFile()
{

    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.bmp);;All Files (*)");

    if (!imagePath.isEmpty())
    {
        // QVector2D coordinate(400.0f, 300.0f);  // Centered coordinate (example)
        // openGLWidget->setImageAtCoordinate(coordinate, imagePath);
    }
}
