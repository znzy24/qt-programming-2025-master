#ifndef QT_PROGRAMMING_2024_BANDAGE_H
#define QT_PROGRAMMING_2024_BANDAGE_H

#include "Equipment.h"

class Bandage : public Equipment {
public:
    explicit Bandage(QGraphicsItem* parent = nullptr);
    
    void useBy(Character* character) override;
    
private:
    int healAmount = 20;
};

#endif