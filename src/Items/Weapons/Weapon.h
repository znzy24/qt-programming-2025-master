#ifndef QT_PROGRAMMING_2024_WEAPON_H
#define QT_PROGRAMMING_2024_WEAPON_H

#include "../Item.h"
#include "../Mountable.h"

class Character;

enum class WeaponType {
    Fist,
    Sword, 
    Bow,    
    Staff    
};

class Weapon : public Item, public Mountable {
public:
    explicit Weapon(QGraphicsItem *parent, const QString &pixmapPath, WeaponType type = WeaponType::Sword, int damage = 25, qreal range = 60.0);

    void mountToParent() override;
    void unmount() override;
    
    virtual void attack(Character* attacker);
    
    WeaponType getType() const { return weaponType; }
    int getDamage() const { return damage; }
    qreal getRange() const { return attackRange; }

protected:
    WeaponType weaponType;
    int damage;
    qreal attackRange;
};

#endif //QT_PROGRAMMING_2024_WEAPON_H