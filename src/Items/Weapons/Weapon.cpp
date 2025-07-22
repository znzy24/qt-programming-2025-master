#include "Weapon.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Weapon::Weapon(QGraphicsItem *parent, const QString &pixmapPath, WeaponType type, int damage, qreal range) 
    : Item(parent, pixmapPath), weaponType(type), damage(damage), attackRange(range) {
}

void Weapon::mountToParent() {
    if (auto character = dynamic_cast<Character*>(parentItem())) {
        setVisible(false); // 武器装备后不显示在地面
    }
}

void Weapon::unmount() {
    setParentItem(nullptr);
    if (weaponType != WeaponType::Fist) { // 拳头永远不显示
        setVisible(true); // 其他武器掉落后重新显示
    }
}

void Weapon::attack(Character* attacker){}