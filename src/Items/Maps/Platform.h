#ifndef QT_PROGRAMMING_2024_PLATFORM_H
#define QT_PROGRAMMING_2024_PLATFORM_H

#include <QGraphicsPixmapItem>

enum class PlatformType {
    Soil,
    Grass,
    Ice
};

class Platform : public QGraphicsPixmapItem {
public:
    Platform(PlatformType type, qreal x, qreal y, int width, QGraphicsItem* parent = nullptr);

    PlatformType getType() const;
private:
    PlatformType type;
};
#endif