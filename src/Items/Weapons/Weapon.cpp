#include "Weapon.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Weapon::Weapon(QGraphicsItem *parent, const QString &pixmapPath, WeaponType type, int damage, qreal range) 
    : Item(parent, pixmapPath), weaponType(type), damage(damage), attackRange(range) {
}

void Weapon::mountToParent() {
    Mountable::mountToParent();
}

void Weapon::unmount() {
    Mountable::unmount();
    setParentItem(nullptr);
    setVisible(true);
}

void Weapon::attack(Character* attacker){}