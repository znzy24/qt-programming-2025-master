#ifndef QT_PROGRAMMING_2024_ADRENALINE_H
#define QT_PROGRAMMING_2024_ADRENALINE_H

#include "Equipment.h"
#include <QTimer>

class Adrenaline : public Equipment {
public:
    explicit Adrenaline(QGraphicsItem* parent = nullptr);
    ~Adrenaline() override;
    
    void useBy(Character* character) override;
    
private:
    static void checkEffectEnd(void* userData);
    
    static void healCharacter(void* userData);
    
private:
    static const int EFFECT_DURATION = 10000;
    static const int HEAL_INTERVAL = 1000;
    static const int HEAL_AMOUNT = 2;
    static const qreal SPEED_BOOST_FACTOR = 1.5;

    Character* affectedCharacter = nullptr;
    QTimer* effectTimer = nullptr;
    QTimer* healTimer = nullptr;
};

#endif
