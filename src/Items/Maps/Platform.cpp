#include "Platform.h"

#include <QPainter>

Platform::Platform(PlatformType type, qreal x, qreal y, int width, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent)
{
    setPos(x, y);
    QString pixmapPath;
    switch (type) {
        case PlatformType::Soil:
            pixmapPath = ":/Items/Maps/Soil.png";
            break;
        case PlatformType::Grass:
            pixmapPath = ":/Items/Maps/Grass.png";
            break;
        case PlatformType::Ice:
            pixmapPath = ":/Items/Maps/Ice.png";
            break;
    }
    QPixmap unit(pixmapPath);
    int unitW = unit.width();
    int unitH = unit.height();

    QPixmap resultPixmap(width * unitW, unitH);
    resultPixmap.fill(Qt::transparent);
    QPainter painter(&resultPixmap);

    for (int i = 0; i < width; ++i) {
        painter.drawPixmap(i * unitW, 0, unit);
    }
    painter.end();

    setPixmap(resultPixmap);
    setZValue(0.1);
}