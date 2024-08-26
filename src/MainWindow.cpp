#include "MainWindow.h"
#include "OpenGLWidget.h"
#include "DataLoader.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), openGLWidget(new OpenGLWidget(this)) {

    
    openFile();
    setCentralWidget(openGLWidget);
}

MainWindow::~MainWindow() {
    delete openGLWidget;
}

void MainWindow::openFile() {
    //QString fileName = QFileDialog::getOpenFileName(this, "Open Data File", "", "CSV Files (*.csv);;All Files (*)");
    QString fileName = "../resources/sample_data.csv";
    if (!fileName.isEmpty()) {
        DataLoader dataLoader;
        std::vector<std::vector<float>> data = dataLoader.loadCSV(fileName);

        if (!data.empty()) {
            // Assume the data has three columns: X, Y, Z
            std::vector<QVector3D> points;
            for (const auto& row : data) {
                if (row.size() == 3) {
                    points.emplace_back(row[0], row[1], row[2]);
                }
            }

            // Pass the loaded points to the OpenGLWidget for rendering
            openGLWidget->setPoints(points);
        } else {
            QMessageBox::warning(this, "Error", "Failed to load data from the file.");
        }
    }
}

void MainWindow::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;All Files (*)");
    if (!fileName.isEmpty()) {
        // Save the current visualization as an image
    }
}
