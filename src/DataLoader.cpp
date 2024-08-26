#include "DataLoader.h"
#include <QFile>
#include <QTextStream>

std::vector<std::vector<float>> DataLoader::loadCSV(const QString &filePath) {
    std::vector<std::vector<float>> data;
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            data.push_back(parseLine(line));
        }
        file.close();
    }

    return data;
}

std::vector<float> DataLoader::parseLine(const QString &line) {
    std::vector<float> values;
    for (const QString &item : line.split(',')) {
        values.push_back(item.toFloat());
    }
    return values;
}
