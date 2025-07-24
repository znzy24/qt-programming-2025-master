#include "Knife.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Knife::Knife(QGraphicsItem *parent) 
    : Weapon(parent, ":/Items/Weapons/Knife.png", WeaponType::Knife, 10, 70)
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(50, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Knife::attack(Character* attacker) {
    if (!attacker || !attacker->scene()) return;
    
    QPointF attackDirection = attacker->isFacingLeft() ? QPointF(-1, 0) : QPointF(1, 0);
    QPointF attackPos = attacker->pos() + attackDirection * attackRange;
    QRectF attackArea(attackPos.x() - 35, attackPos.y() - 40, 35, 80);
    
    for (QGraphicsItem* item : attacker->scene()->items(attackArea)) {
        if (auto enemy = dynamic_cast<Character*>(item)) {
            if (enemy != attacker) {
                enemy->takeDamage(damage);
                break;
            }
        }
    }
}