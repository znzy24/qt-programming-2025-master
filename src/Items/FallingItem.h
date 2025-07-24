#ifndef QT_PROGRAMMING_2024_FALLINGITEM_H
#define QT_PROGRAMMING_2024_FALLINGITEM_H

#include "Item.h"
#include <QTimer>

// 下落物品类，处理物品从空中掉落的逻辑
class FallingItem : public QObject {
    Q_OBJECT

public:
    explicit FallingItem(Item* item, QGraphicsScene* scene, QObject* parent = nullptr);
    ~FallingItem();

    // 开始下落
    void startFalling();

private slots:
    // 更新位置
    void updatePosition();
    
    // 物品自动销毁（30秒后未被拾取时）
    void autoDestroy();

private:
    Item* item;
    QGraphicsScene* scene;
    QTimer* fallTimer;
    QTimer* destroyTimer; // 控制物品自动销毁的计时器
    qreal velocityY = 0;
    const qreal gravity = 0.05; // 重力加速度
    static const int AUTO_DESTROY_TIME = 30000; // 30秒后自动销毁
};

#endif // QT_PROGRAMMING_2024_FALLINGITEM_H
