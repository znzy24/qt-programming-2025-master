#ifndef QT_PROGRAMMING_2024_MAP_H
#define QT_PROGRAMMING_2024_MAP_H

#include <QGraphicsScene>
#include <QPointF>
#include "../Item.h"

class Map : public Item {
public:
    explicit Map(QGraphicsItem *parent = nullptr, const QString &pixmapPath = "");
};


#endif
