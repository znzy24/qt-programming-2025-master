#include "FallingItem.h"
#include <QGraphicsScene>
#include "Maps/Platform.h"

FallingItem::FallingItem(Item* item, QGraphicsScene* scene, QObject* parent)
    : QObject(parent), item(item), scene(scene)
{
    // 创建定时器控制下落
    fallTimer = new QTimer(this);
    connect(fallTimer, &QTimer::timeout, this, &FallingItem::updatePosition);
}

FallingItem::~FallingItem()
{
    if (fallTimer) {
        fallTimer->stop();
        fallTimer->deleteLater();
    }
    // 注意：不要在这里删除item，因为它可能已经被角色拾取
}

void FallingItem::startFalling()
{
    if (!item || !scene) return;
    
    // 确保物品在场景中
    if (item->scene() != scene) {
        scene->addItem(item);
    }
    
    // 开始下落计时器
    fallTimer->start(16); // 约60fps
}

void FallingItem::updatePosition()
{
    if (!item || !scene) {
        QTimer::singleShot(0, this, &QObject::deleteLater); // 安全地在下一个事件循环删除
        return;
    }
    
    // 应用重力
    velocityY += gravity;
    
    // 更新位置
    QPointF newPos = item->pos();
    newPos.setY(newPos.y() + velocityY);
    item->setPos(newPos);
    
    // 检查是否碰到平台
    QPointF checkPoint(item->pos().x(), item->pos().y() + item->boundingRect().height() / 2);
    QList<QGraphicsItem*> items = scene->items(checkPoint);
    
    for (QGraphicsItem* sceneItem : items) {
        if (Platform* platform = dynamic_cast<Platform*>(sceneItem)) {
            // 让物品停在平台上
            qreal platformTopY = platform->y();
            item->setPos(item->pos().x(), platformTopY - item->boundingRect().height() / 2);
            
            // 停止下落
            fallTimer->stop();
            QTimer::singleShot(0, this, &QObject::deleteLater); // 安全地在下一个事件循环删除
            return;
        }
    }
    
    // 检查是否超出场景底部
    if (item->pos().y() > scene->sceneRect().height()) {
        // 删除超出边界的物品
        scene->removeItem(item);
        delete item; // 直接删除Item对象，因为Item不是QObject子类，没有deleteLater方法
        item = nullptr; // 防止悬挂指针
        QTimer::singleShot(0, this, &QObject::deleteLater); // 安全地在下一个事件循环删除
    }
}
