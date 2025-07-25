 #include "Rifle.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QDateTime>

Bullet::Bullet(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter)
    : Item(nullptr, ":/Items/Weapons/Bullet.png"), damage(damage), shooter(shooter)
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }

    setPos(startPos);
    setZValue(20);
    
    velocity = facingLeft ? -5.0 : 5.0;
    
    setRotation(facingLeft ? 180 : 0);
    
    scene->addItem(this);
    
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

bool Bullet::checkCollision() {
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
Rifle::Rifle(QGraphicsItem* parent)
    : Weapon(parent, ":/Items/Weapons/Rifle.png", WeaponType::Rifle, 10, 500)
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(100, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Rifle::attack(Character* attacker) {
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
    
    new Bullet(attacker->scene(), startPos, facingLeft, damage, attacker);
    
    remainingBullets--;
    
    attacker->setWeaponPoints(remainingBullets, 10);
    
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
