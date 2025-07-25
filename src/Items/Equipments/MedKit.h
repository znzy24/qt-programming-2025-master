#ifndef QT_PROGRAMMING_2024_MEDKIT_H
#define QT_PROGRAMMING_2024_MEDKIT_H

#include "Equipment.h"

class MedKit : public Equipment {
public:
    explicit MedKit(QGraphicsItem* parent = nullptr);
    
    void useBy(Character* character) override;
};

#endif
