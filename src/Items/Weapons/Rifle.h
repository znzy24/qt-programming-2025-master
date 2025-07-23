#ifndef QT_PROGRAMMING_2025_RIFLE_H
#define QT_PROGRAMMING_2025_RIFLE_H

#include "Weapon.h"
#include <QTimer>
#include <QObject>

class Bullet : public QObject, public Item {
    Q_OBJECT
public:
    explicit Bullet(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter);
    ~Bullet() override;

private slots:
    void advance();

private:
    qreal velocity;
    int damage;
    QTimer* moveTimer;
    int lifetime = 0;
    const int maxLifetime = 100;
    bool checkCollision();
    Character* shooter; // 记录发射该子弹的角色
};

class Rifle : public Weapon {
public:
    explicit Rifle(QGraphicsItem* parent = nullptr);
    
    void attack(Character* attacker) override;
    int getCost() const override { return 0; } // 避免与内部弹药计数冲突
    int getInitialPoints() const override { return 10; } // 初始10发子弹
    int getMaxPoints() const override { return 10; }
    int getRemainingBullets() const { return remainingBullets; }
    
private:
    int remainingBullets = 10; // 默认有10发子弹
    qint64 lastFireTime = 0;   // 上次射击时间
    const qint64 fireInterval = 200; // 射击间隔(毫秒)
};

#endif // QT_PROGRAMMING_2025_RIFLE_H
