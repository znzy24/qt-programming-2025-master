#ifndef QT_PROGRAMMING_2025_SOLIDBALL_H
#define QT_PROGRAMMING_2025_SOLIDBALL_H

#include "Weapon.h"
#include <QTimer>
#include <QObject>

class ProjectileBall : public QObject, public Item {
    Q_OBJECT
public:
    explicit ProjectileBall(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter);
    ~ProjectileBall() override;

private slots:
    void advance();

private:
    qreal xVelocity;
    qreal yVelocity;
    const qreal gravity = 0.1;
    int damage;
    QTimer* moveTimer;
    int lifetime = 0;
    const int maxLifetime = 150; // 增加生命周期，使其能飞得更远
    bool checkCollision();
    Character* shooter; // 记录发射该球的角色
};


class SolidBall : public Weapon {
public:
    explicit SolidBall(QGraphicsItem* parent = nullptr);
    
    void attack(Character* attacker) override;
    int getCost() const override { return 0; } // 改为0，避免与内部计数冲突
    int getInitialPoints() const override { return 3; }
    int getMaxPoints() const override { return 3; }
    int getRemainingShots() const { return remainingShots; }
    
private:
    int remainingShots = 3; 
};

#endif