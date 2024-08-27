#ifndef DATALOADER_H
#define DATALOADER_H

#include <QString>
#include <vector>

class DataLoader {
public:
    std::vector<std::vector<float>> loadCSV(const QString &filePath);
private:
    std::vector<float> parseLine(const QString &line);
};

#endif // DATALOADER_H
