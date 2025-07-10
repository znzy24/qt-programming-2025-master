#ifndef PLATFORM_H
#define PLATFORM_H

#include <QGraphicsPixmapItem>

enum class PlatformType {
    Soil,
    Grass,
    Ice
};

class Platform : public QGraphicsPixmapItem {
public:
    Platform(PlatformType type, qreal x, qreal y, int width, QGraphicsItem* parent = nullptr);
};
#endif // PLATFORM_H