#ifndef DATALOADER_H
#define DATALOADER_H

#include <vector>
#include <QString>

class DataLoader {
public:
    DataLoader() = default;
    std::vector<std::vector<float>> loadCSV(const QString &filePath);

private:
    std::vector<float> parseLine(const QString &line);
};

#endif // DATALOADER_H
