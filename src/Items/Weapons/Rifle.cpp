 #include "Rifle.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QDateTime>

// Bullet 实现 -----------------------------------------

Bullet::Bullet(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter)
    : Item(nullptr, ":/Items/Weapons/Bullet.png"), damage(damage), shooter(shooter)
{
    // 设置图像大小
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
    
    // 设置初始位置
    setPos(startPos);
    setZValue(20);  // 确保显示在大多数物体上方
    
    // 水平速度，根据朝向决定方向
    velocity = facingLeft ? -5.0 : 5.0;  // 子弹速度较快
    
    // 设置旋转，使子弹朝向移动方向
    setRotation(facingLeft ? 180 : 0);
    
    // 添加到场景
    scene->addItem(this);
    
    // 创建定时器控制移动
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Bullet::advance);
    moveTimer->start(16);  // 约60fps
}

Bullet::~Bullet() {
    if (moveTimer) {
        moveTimer->stop();
        moveTimer->deleteLater();
    }
    
    if (scene()) {
        scene()->removeItem(this);
    }
}

void Bullet::advance() {
    // 应用匀速直线运动
    setPos(pos().x() + velocity, pos().y());
    
    // 检测碰撞
    if (checkCollision()) {
        deleteLater();
        return;
    }
    
    // 检查是否超出场景边界
    QRectF sceneRect = scene()->sceneRect();
    if (!sceneRect.contains(pos())) {
        deleteLater();
        return;
    }
    
    // 超过最大生命周期则销毁
    if (++lifetime > maxLifetime) {
        deleteLater();
    }
}

bool Bullet::checkCollision() {
    QList<QGraphicsItem*> collisions = scene()->items(pos());
    
    for (QGraphicsItem* item : collisions) {
        // 避免与自身碰撞
        if (item == this) continue;
        
        // 检查与角色的碰撞
        Character* character = dynamic_cast<Character*>(item);
        if (character) {
            // 避免与发射者碰撞
            if (character == shooter) continue;
            
            // 对其他角色造成伤害
            character->takeDamage(damage);
            return true;
        }
        
        // 检查与平台的碰撞
        Platform* platform = dynamic_cast<Platform*>(item);
        if (platform) {
            return true;
        }
    }
    return false;
}

// Rifle 实现 -----------------------------------------

Rifle::Rifle(QGraphicsItem* parent)
    : Weapon(parent, ":/Items/Weapons/Rifle.png", WeaponType::Rifle, 10, 500)
{
    // 设置武器外观
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(100, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Rifle::attack(Character* attacker) {
    // 检查是否有效及是否有子弹
    if (!attacker || remainingBullets <= 0 || !attacker->scene()) {
        return;
    }
    
    // 检查射击间隔
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (currentTime - lastFireTime < fireInterval) {
        return; // 射击间隔未到，不能射击
    }
    
    // 更新上次射击时间
    lastFireTime = currentTime;
    
    // 创建子弹实例
    QPointF startPos = attacker->scenePos();
    bool facingLeft = attacker->isFacingLeft();
    
    // 根据角色朝向调整发射位置
    startPos.ry() -= 70;  // 从角色上半身发射
    if (facingLeft) {
        startPos.rx() -= 60;  // 左手发射偏移
    } else {
        startPos.rx() += 60;  // 右手发射偏移
    }
    
    // 创建并发射子弹
    new Bullet(attacker->scene(), startPos, facingLeft, damage, attacker);
    
    // 减少剩余子弹数
    remainingBullets--;
    
    // 更新显示的武器点数
    attacker->setWeaponPoints(remainingBullets, 10);
    
    // 如果用完了，延迟移除武器并恢复拳头
    if (remainingBullets <= 0) {
        // 将武器设置为不可见，在下一帧删除
        setVisible(false);
        
        // 创建一个定时器，延迟一小段时间后切换回拳头
        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        
        // 保存一份当前的scene指针，因为weapon可能已经被删除
        QGraphicsScene* currentScene = attacker->scene();
        Character* player = attacker; // 保存玩家引用
        
        // 连接定时器到lambda，执行武器切换
        QObject::connect(timer, &QTimer::timeout, [timer, player, currentScene]() {
            // 检查场景和玩家是否还存在
            if (currentScene && player && player->scene() == currentScene) {
                // 移除武器，恢复拳头
                player->pickupWeapon(nullptr);
            }
            // 销毁定时器
            timer->deleteLater();
        });
        
        // 启动定时器，500ms后执行
        timer->start(500);
    }
}
