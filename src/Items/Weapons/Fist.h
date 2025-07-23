#ifndef QT_PROGRAMMING_2024_FIST_H
#define QT_PROGRAMMING_2024_FIST_H

#include "Weapon.h"

class Fist : public Weapon {
public:
    explicit Fist(QGraphicsItem *parent = nullptr);
    
    void attack(Character* attacker) override;
};

#endif