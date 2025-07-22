#include "Knife.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Knife::Knife(QGraphicsItem *parent) 
    : Weapon(parent, ":/Items/Weapons/Knife.png", WeaponType::Knife, 10, 70)  // 伤害10，范围70
{
    // 设置小刀的图像大小
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(24, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Knife::attack(Character* attacker) {
    if (!attacker || !attacker->scene()) return;
    
    // 小刀攻击范围较小但伤害适中
    QPointF attackDirection = attacker->isFacingLeft() ? QPointF(-1, 0) : QPointF(1, 0);
    QPointF attackPos = attacker->pos() + attackDirection * attackRange;
    QRectF attackArea(attackPos.x() - 22, attackPos.y() - 35, 45, 70);
    
    // 检测攻击范围内的敌人
    for (QGraphicsItem* item : attacker->scene()->items(attackArea)) {
        if (auto enemy = dynamic_cast<Character*>(item)) {
            if (enemy != attacker) { // 不攻击自己
                enemy->takeDamage(damage);
                break; // 一次攻击只能命中一个敌人
            }
        }
    }
}