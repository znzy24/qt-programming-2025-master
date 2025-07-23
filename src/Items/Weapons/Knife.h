#ifndef QT_PROGRAMMING_2024_KNIFE_H
#define QT_PROGRAMMING_2024_KNIFE_H

#include "Weapon.h"

class Knife : public Weapon {
public:
    explicit Knife(QGraphicsItem *parent = nullptr);
    
    void attack(Character* attacker) override;
};

#endif