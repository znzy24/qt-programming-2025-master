#include "SolidBall.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QRandomGenerator>

// ProjectileBall 实现 -----------------------------------------

ProjectileBall::ProjectileBall(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter)
    : Item(nullptr, ":/Items/Weapons/SolidBall.png"), damage(damage), shooter(shooter)
{
    // 设置图像大小
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
    
    // 设置初始位置和物理属性
    setPos(startPos);
    setZValue(20);  // 确保显示在大多数物体上方
    
    // 初速度设置 (斜抛运动) - 增加了抛掷速度和高度
    xVelocity = facingLeft ? -4.5 : 4.5;  // 增加水平速度
    yVelocity = -3.8;                     // 增加初始向上的速度（更高的弧线）
    
    // 添加随机性，使每次投掷略有不同
    xVelocity += QRandomGenerator::global()->bounded(801) / 1000.0 - 0.4;  // 减少水平随机性
    yVelocity += QRandomGenerator::global()->bounded(501) / 1000.0 - 0.25;  // 减少垂直随机性
    
    // 添加到场景
    scene->addItem(this);
    
    // 创建定时器控制移动
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &ProjectileBall::advance);
    moveTimer->start(16);  // 约60fps
}

ProjectileBall::~ProjectileBall() {
    if (moveTimer) {
        moveTimer->stop();
        moveTimer->deleteLater();
    }
    
    if (scene()) {
        scene()->removeItem(this);
    }
}

void ProjectileBall::advance() {
    // 应用物理运动 (斜抛运动)
    yVelocity += gravity;  // 重力加速度
    setPos(pos().x() + xVelocity, pos().y() + yVelocity);
    
    // 添加旋转效果，让实心球看起来在飞行过程中旋转
    // 根据水平速度方向决定旋转方向
    qreal rotationSpeed = xVelocity > 0 ? 10.0 : -10.0;
    setRotation(rotation() + rotationSpeed);
    
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

bool ProjectileBall::checkCollision() {
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

// SolidBall 实现 -----------------------------------------

SolidBall::SolidBall(QGraphicsItem* parent)
    : Weapon(parent, ":/Items/Weapons/SolidBall.png", WeaponType::SolidBall, 15, 300)
{
    // 设置武器外观
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void SolidBall::attack(Character* attacker) {
    if (!attacker || remainingShots <= 0 || !attacker->scene()) {
        return;
    }
    
    // 创建投掷物实例
    QPointF startPos = attacker->scenePos();
    bool facingLeft = attacker->isFacingLeft();
    
    // 根据角色朝向调整发射位置
    startPos.ry() -= 65;  // 从角色更高位置发射（头部位置）
    if (facingLeft) {
        startPos.rx() -= 30;  // 左手发射偏移增加
    } else {
        startPos.rx() += 30;  // 右手发射偏移增加
    }
    
    // 创建并发射实心球
    new ProjectileBall(attacker->scene(), startPos, facingLeft, damage, attacker);
    
    // 减少剩余次数
    remainingShots--;
    
    // 更新显示的武器点数
    attacker->setWeaponPoints(remainingShots, 3);
    
    // 如果用完了，自行处理移除武器
    if (remainingShots <= 0) {
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
                // 创建一个新的拳头
                player->pickupWeapon(nullptr);
            }
            // 销毁定时器
            timer->deleteLater();
        });
        
        // 启动定时器，500ms后执行
        timer->start(500);
    }
}