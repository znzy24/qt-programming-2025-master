#include "FallingItem.h"
#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include "Maps/Platform.h"

QHash<Item*, FallingItem*> FallingItem::activeControllers;

FallingItem::FallingItem(Item* item, QGraphicsScene* scene, QObject* parent)
    : QObject(parent), item(item), scene(scene)
{

    fallTimer = new QTimer(this);
    connect(fallTimer, &QTimer::timeout, this, &FallingItem::updatePosition);

    destroyTimer = new QTimer(this);
    destroyTimer->setSingleShot(true);
    connect(destroyTimer, &QTimer::timeout, this, &FallingItem::autoDestroy);

    if (item) {
        activeControllers[item] = this;
    }
}

FallingItem::~FallingItem()
{
    if (fallTimer) {
        fallTimer->stop();
        fallTimer->deleteLater();
    }
    
    if (destroyTimer) {
        destroyTimer->stop();
        destroyTimer->deleteLater();
    }

    if (item && activeControllers.contains(item)) {
        activeControllers.remove(item);
    }

}

void FallingItem::startFalling()
{
    if (!item || !scene) return;
    
    if (item->scene() != scene) {
        scene->addItem(item);
    }
    
    fallTimer->start(16);

    destroyTimer->start(AUTO_DESTROY_TIME);
}

void FallingItem::updatePosition()
{
    if (!item || !scene) {
        QTimer::singleShot(0, this, &QObject::deleteLater); 
        return;
    }
    
    velocityY += gravity;
    
    QPointF newPos = item->pos();
    newPos.setY(newPos.y() + velocityY);
    item->setPos(newPos);
    
    QPointF checkPoint(item->pos().x(), item->pos().y() + item->boundingRect().height() / 2);
    QList<QGraphicsItem*> items = scene->items(checkPoint);
    
    for (QGraphicsItem* sceneItem : items) {
        if (Platform* platform = dynamic_cast<Platform*>(sceneItem)) {
            qreal platformTopY = platform->y();
            item->setPos(item->pos().x(), platformTopY - item->boundingRect().height() / 2);
            
            fallTimer->stop();
            
            QTimer::singleShot(0, this, &QObject::deleteLater);
            return;
        }
    }
    
    if (item->pos().y() > scene->sceneRect().height()) {
        scene->removeItem(item);
        delete item;
        item = nullptr;
        destroyTimer->stop();
        QTimer::singleShot(0, this, &QObject::deleteLater);
    }
}

void FallingItem::cancelAutoDestroy() {
    if (destroyTimer) {
        destroyTimer->stop();
    }
    
    if (item) {
        activeControllers.remove(item);
    }
    
    QTimer::singleShot(0, this, &QObject::deleteLater);
}

FallingItem* FallingItem::findControllerForItem(Item* item) {
    if (!item || !activeControllers.contains(item)) {
        return nullptr;
    }
    return activeControllers[item];
}

void FallingItem::autoDestroy()
{
    if (item && scene) {
        
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
        effect->setOpacity(1.0);
        item->setGraphicsEffect(effect);
        
        QTimer* fadeTimer = new QTimer(this);
        qreal opacity = 1.0;
        connect(fadeTimer, &QTimer::timeout, [this, effect, fadeTimer, opacity = 1.0]() mutable {
            opacity -= 0.05;
            if (opacity <= 0) {
                fadeTimer->stop();
                fadeTimer->deleteLater();
                
                if (item && scene) {
                    scene->removeItem(item);
                    delete item;
                    item = nullptr;
                }
                
                QTimer::singleShot(0, this, &QObject::deleteLater);
            } else {
                effect->setOpacity(opacity);
            }
        });
        fadeTimer->start(50); 
    } else {
        QTimer::singleShot(0, this, &QObject::deleteLater);
    }
}
