#ifndef QT_PROGRAMMING_2024_FALLINGITEM_H
#define QT_PROGRAMMING_2024_FALLINGITEM_H

#include "Item.h"
#include <QTimer>
#include <QHash>

class FallingItem : public QObject {
    Q_OBJECT

public:
    explicit FallingItem(Item* item, QGraphicsScene* scene, QObject* parent = nullptr);
    ~FallingItem();

    void startFalling();
    
    void cancelAutoDestroy();
    
    Item* getItem() const { return item; }
    
    static FallingItem* findControllerForItem(Item* item);
    
    static QHash<Item*, FallingItem*> activeControllers;

private slots:
    void updatePosition();
    
    void autoDestroy();

private:
    Item* item;
    QGraphicsScene* scene;
    QTimer* fallTimer;
    QTimer* destroyTimer; 
    qreal velocityY = 0;
    const qreal gravity = 0.05; 
    static const int AUTO_DESTROY_TIME = 30000; 
};

#endif // QT_PROGRAMMING_2024_FALLINGITEM_H
