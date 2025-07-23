#ifndef QT_PROGRAMMING_2025_SNIPER_H
#define QT_PROGRAMMING_2025_SNIPER_H

#include "Weapon.h"
#include <QTimer>
#include <QObject>

class SniperBullet : public QObject, public Item {
    Q_OBJECT
public:
    explicit SniperBullet(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter);
    ~SniperBullet() override;

private slots:
    void advance();

private:
    qreal velocity;
    int damage;
    QTimer* moveTimer;
    int lifetime = 0;
    const int maxLifetime = 150; // 较长的生命周期，射程更远
    bool checkCollision();
    Character* shooter; // 记录发射该子弹的角色
};

class Sniper : public Weapon {
public:
    explicit Sniper(QGraphicsItem* parent = nullptr);
    
    void attack(Character* attacker) override;
    int getCost() const override { return 0; } // 避免与内部弹药计数冲突
    int getInitialPoints() const override { return 3; } // 初始只有3发子弹
    int getMaxPoints() const override { return 3; }
    int getRemainingBullets() const { return remainingBullets; }
    
private:
    int remainingBullets = 3; // 默认有3发子弹
    qint64 lastFireTime = 0;   // 上次射击时间
    const qint64 fireInterval = 1500; // 射击间隔长(1.5秒)
};

#endif // QT_PROGRAMMING_2025_SNIPER_H
