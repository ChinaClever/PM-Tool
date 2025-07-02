#pragma once
#include <QString>
#include <QVariantList>

struct DbWriteTask {
    enum TableType {
        ThreePhase,
        SinglePhase,
        BoxPhase,
        OutputBit
    } table;

    QString key;
    QVariantList values; // 具体参数根据表不同，比如三相是三个double，单相是一个double等
};
