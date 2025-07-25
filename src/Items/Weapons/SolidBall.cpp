#include "SolidBall.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QRandomGenerator>


ProjectileBall::ProjectileBall(QGraphicsScene* scene, const QPointF& startPos, bool facingLeft, int damage, Character* shooter)
    : Item(nullptr, ":/Items/Weapons/SolidBall.png"), damage(damage), shooter(shooter)
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
    
    setPos(startPos);
    setZValue(20); 

    xVelocity = facingLeft ? -4.5 : 4.5;  
    yVelocity = -3.8;                    

    xVelocity += QRandomGenerator::global()->bounded(801) / 1000.0 - 0.4;  
    yVelocity += QRandomGenerator::global()->bounded(501) / 1000.0 - 0.25;  
    
    scene->addItem(this);
    
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &ProjectileBall::advance);
    moveTimer->start(16);
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
    yVelocity += gravity; 
    setPos(pos().x() + xVelocity, pos().y() + yVelocity);

    qreal rotationSpeed = xVelocity > 0 ? 10.0 : -10.0;
    setRotation(rotation() + rotationSpeed);
    
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

bool ProjectileBall::checkCollision() {
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


SolidBall::SolidBall(QGraphicsItem* parent)
    : Weapon(parent, ":/Items/Weapons/SolidBall.png", WeaponType::SolidBall, 15, 300)
{
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
    
    QPointF startPos = attacker->scenePos();
    bool facingLeft = attacker->isFacingLeft();
    
    startPos.ry() -= 65; 
    if (facingLeft) {
        startPos.rx() -= 30; 
    } else {
        startPos.rx() += 30; 
    }
    
    new ProjectileBall(attacker->scene(), startPos, facingLeft, damage, attacker);
    
    remainingShots--;
    
    attacker->setWeaponPoints(remainingShots, 3);
    
    if (remainingShots <= 0) {
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