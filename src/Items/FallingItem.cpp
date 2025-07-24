#include "FallingItem.h"
#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include "Maps/Platform.h"

FallingItem::FallingItem(Item* item, QGraphicsScene* scene, QObject* parent)
    : QObject(parent), item(item), scene(scene)
{
    // 创建定时器控制下落
    fallTimer = new QTimer(this);
    connect(fallTimer, &QTimer::timeout, this, &FallingItem::updatePosition);
    
    // 创建定时器控制自动销毁
    destroyTimer = new QTimer(this);
    destroyTimer->setSingleShot(true);
    connect(destroyTimer, &QTimer::timeout, this, &FallingItem::autoDestroy);
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
    
    // 开始自动销毁计时器（30秒后销毁）
    destroyTimer->start(AUTO_DESTROY_TIME);
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
            
            // 此时物品已落地，销毁控制器但不删除物品（等待30秒自动销毁或被玩家拾取）
            // 注：FallingItem 控制器删除后不影响物品本身
            QTimer::singleShot(0, this, &QObject::deleteLater);
            return;
        }
    }
    
    // 检查是否超出场景底部
    if (item->pos().y() > scene->sceneRect().height()) {
        // 删除超出边界的物品
        scene->removeItem(item);
        delete item; // 直接删除Item对象，因为Item不是QObject子类，没有deleteLater方法
        item = nullptr; // 防止悬挂指针
        destroyTimer->stop(); // 停止自动销毁计时器
        QTimer::singleShot(0, this, &QObject::deleteLater); // 安全地在下一个事件循环删除
    }
}

void FallingItem::autoDestroy()
{
    // 30秒后如果物品仍未被拾取，则自动销毁
    if (item && scene) {
        // 添加一个淡出动画效果
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
        effect->setOpacity(1.0);
        item->setGraphicsEffect(effect);
        
        // 使用计时器创建淡出效果（1秒淡出）
        QTimer* fadeTimer = new QTimer(this);
        qreal opacity = 1.0;
        connect(fadeTimer, &QTimer::timeout, [this, effect, fadeTimer, &opacity]() {
            opacity -= 0.05;
            if (opacity <= 0) {
                fadeTimer->stop();
                fadeTimer->deleteLater();
                
                // 完全淡出后删除物品
                if (item && scene) {
                    scene->removeItem(item);
                    delete item;
                    item = nullptr;
                }
                
                // 删除控制器
                QTimer::singleShot(0, this, &QObject::deleteLater);
            } else {
                effect->setOpacity(opacity);
            }
        });
        fadeTimer->start(50); // 每50ms更新一次，总共约1秒完成淡出
    } else {
        // 如果物品或场景不存在，直接删除控制器
        QTimer::singleShot(0, this, &QObject::deleteLater);
    }
}
