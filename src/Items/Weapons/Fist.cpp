#include "Fist.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Fist::Fist(QGraphicsItem *parent) 
    : Weapon(parent, "", WeaponType::Fist, 5, 50.0)  // 空字符串因为拳头不需要图像
{
    // 拳头不需要显示图像
    if (pixmapItem) {
        pixmapItem->setVisible(false);
    }
}

void Fist::attack(Character* attacker) {
    if (!attacker || !attacker->scene()) return;
    
    // 计算攻击方向和区域
    QPointF attackDirection = attacker->isFacingLeft() ? QPointF(-1, 0) : QPointF(1, 0);
    QPointF attackPos = attacker->pos() + attackDirection * attackRange;
    QRectF attackArea(attackPos.x() - 25, attackPos.y() - 40, 50, 80);
    
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