#ifndef QT_PROGRAMMING_2024_WEAPON_H
#define QT_PROGRAMMING_2024_WEAPON_H

#include "../Item.h"
#include "../Mountable.h"

class Character;

enum class WeaponType {
    Fist,
    Knife,
    SolidBall  
};

class Weapon : public Item, public Mountable {
public:
    explicit Weapon(QGraphicsItem *parent, const QString &pixmapPath, WeaponType type = WeaponType::Fist, int damage = 25, qreal range = 60.0);

    void mountToParent() override;
    void unmount() override;
    
    virtual void attack(Character* attacker);

    virtual int getCost() const { return 0; }
    virtual int getInitialPoints() const { return 1; }
    virtual int getMaxPoints() const { return 1; }

protected:
    WeaponType weaponType;
    int damage;
    qreal attackRange;
};

#endif