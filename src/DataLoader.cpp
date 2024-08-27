#include "DataLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

std::vector<std::vector<float>> DataLoader::loadCSV(const QString &filePath) {
    std::vector<std::vector<float>> data;
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << filePath;
        return data;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filePath;
        return data;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines
        std::vector<float> row = parseLine(line);
        if (!row.empty()) {
            data.push_back(row);
        }
    }
    file.close();

    return data;
}

std::vector<float> DataLoader::parseLine(const QString &line) {
    std::vector<float> row;
    QStringList tokens = line.split(',');
    for (const QString &token : tokens) {
        bool ok;
        float value = token.toFloat(&ok);
        if (ok) {
            row.push_back(value);
        }
    }
    return row;
}
