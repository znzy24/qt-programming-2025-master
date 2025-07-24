#include "Fist.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Fist::Fist(QGraphicsItem *parent) 
    : Weapon(parent, "", WeaponType::Fist, 5, 40) 
{
    if (pixmapItem) {
        pixmapItem->setVisible(false);
    }
}

void Fist::attack(Character* attacker) {
    if (!attacker || !attacker->scene()) return;
    
    // 拳头攻击使用更窄更精准的攻击区域
    QPointF attackDirection = attacker->isFacingLeft() ? QPointF(-1, 0) : QPointF(1, 0);
    QPointF attackPos = attacker->pos() + attackDirection * attackRange;
    QRectF attackArea(attackPos.x() - 20, attackPos.y() - 30, 40, 60);  // 更小的攻击区域
    
    
    // 拳头攻击可以连击多个敌人（不再使用break提前退出循环）
    for (QGraphicsItem* item : attacker->scene()->items(attackArea)) {
        if (auto enemy = dynamic_cast<Character*>(item)) {
            if (enemy != attacker) {
                enemy->takeDamage(damage);
            }
        }
    }
}