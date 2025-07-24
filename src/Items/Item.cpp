#include "Item.h"
#include <QGraphicsEffect>

Item::Item(QGraphicsItem *parent, const QString &pixmapPath) : QGraphicsItem(parent) {
    if (pixmapPath != "") {
        pixmapItem = new QGraphicsPixmapItem(QPixmap(pixmapPath), this);
    }
}

[[nodiscard]] QRectF Item::boundingRect() const{
        if (pixmapItem != nullptr) {
            return this->pixmapItem->boundingRect();
        }
        return {};
    }

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    return;
}

void Item::setGraphicsEffect(QGraphicsEffect* effect) {
    if (pixmapItem) {
        pixmapItem->setGraphicsEffect(effect);
    }
}