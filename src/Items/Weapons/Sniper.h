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
    const int maxLifetime = 150; 
    bool checkCollision();
    Character* shooter; 
};

class Sniper : public Weapon {
public:
    explicit Sniper(QGraphicsItem* parent = nullptr);
    
    void attack(Character* attacker) override;
    int getCost() const override { return 0; }
    int getInitialPoints() const override { return 3; } 
    int getMaxPoints() const override { return 3; }
    int getRemainingBullets() const { return remainingBullets; }
    
private:
    int remainingBullets = 3; 
    qint64 lastFireTime = 0;   
    const qint64 fireInterval = 1500; 
};

#endif
