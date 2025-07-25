#include "Sniper.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QDateTime>
#include <QGraphicsEffect>

SniperBullet::SniperBullet(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter)
    : Item(nullptr, ":/Items/Weapons/Bullet.png"), damage(damage), shooter(shooter)
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);

        QGraphicsColorizeEffect* glowEffect = new QGraphicsColorizeEffect();
        glowEffect->setColor(Qt::red);
        glowEffect->setStrength(0.3);
        pixmapItem->setGraphicsEffect(glowEffect);
    }
    
    setPos(startPos);
    setZValue(25); 
    
    velocity = facingLeft ? -8.0 : 8.0;
    
    setRotation(facingLeft ? 180 : 0);
    
    scene->addItem(this);
    
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &SniperBullet::advance);
    moveTimer->start(16); 
}

SniperBullet::~SniperBullet() {
    if (moveTimer) {
        moveTimer->stop();
        moveTimer->deleteLater();
    }
    
    if (scene()) {
        scene()->removeItem(this);
    }
}

void SniperBullet::advance() {
    setPos(pos().x() + velocity, pos().y());
    
    if (checkCollision()) {
        deleteLater();
        return;
    }

    QRectF sceneRect = scene()->sceneRect();
    if (!sceneRect.contains(pos())) {
        deleteLater();
        return;
    }
    
    if (++lifetime > maxLifetime) {
        deleteLater();
    }
}

bool SniperBullet::checkCollision() {
    QList<QGraphicsItem*> collisions = scene()->items(pos());
    
    for (QGraphicsItem* item : collisions) {
        if (item == this) continue;
        
        Character* character = dynamic_cast<Character*>(item);
        if (character) {
            if (character == shooter) continue;
            
            character->takeDamage(damage);
            return true;
        }

        Platform* platform = dynamic_cast<Platform*>(item);
        if (platform) {
            return true;
        }
    }
    return false;
}

Sniper::Sniper(QGraphicsItem* parent)
    : Weapon(parent, ":/Items/Weapons/Sniper.png", WeaponType::Sniper, 30, 800) // 伤害值高达30
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(100, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Sniper::attack(Character* attacker) {
    if (!attacker || remainingBullets <= 0 || !attacker->scene()) {
        return;
    }

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (currentTime - lastFireTime < fireInterval) {
        return;
    }

    lastFireTime = currentTime;

    QPointF startPos = attacker->scenePos();
    bool facingLeft = attacker->isFacingLeft();
    

    startPos.ry() -= 70; 
    if (facingLeft) {
        startPos.rx() -= 60; 
    } else {
        startPos.rx() += 60; 
    }
    
    new SniperBullet(attacker->scene(), startPos, facingLeft, damage, attacker);
    
    remainingBullets--;
    
    attacker->setWeaponPoints(remainingBullets, 3);
    
    if (remainingBullets <= 0) {
        setVisible(false);
        
        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        
        QGraphicsScene* currentScene = attacker->scene();
        Character* player = attacker;
        
        QObject::connect(timer, &QTimer::timeout, [timer, player, currentScene]() {
            if (currentScene && player && player->scene() == currentScene) {
                player->pickupWeapon(nullptr);
            }
            timer->deleteLater();
        });
        
        timer->start(500);
    }
}
