#include "Adrenaline.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>
#include <QGraphicsEffect>
#include <QTimer>

Adrenaline::Adrenaline(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/Adrenaline.png")
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(40, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
    
    effectTimer = nullptr;
    healTimer = nullptr;
}

Adrenaline::~Adrenaline()
{
    if (affectedCharacter) {
        affectedCharacter->setSpeedMultiplier(1.0);
    }
    
    if (effectTimer) {
        effectTimer->stop();
        delete effectTimer;
    }
    
    if (healTimer) {
        healTimer->stop();
        delete healTimer;
    }
}

void Adrenaline::useBy(Character* character)
{
    if (!character) return;
    
    affectedCharacter = character;

    character->setSpeedMultiplier(1.5);

    effectTimer = new QTimer();
    effectTimer->setSingleShot(true);
    QObject::connect(effectTimer, &QTimer::timeout, [this]() {
        checkEffectEnd(this);
    });
    effectTimer->start(EFFECT_DURATION);
    
    healTimer = new QTimer();
    QObject::connect(healTimer, &QTimer::timeout, [this]() {
        healCharacter(this);
    });
    healTimer->start(HEAL_INTERVAL);
    
    if (scene()) {
        scene()->removeItem(this);
        hide();
    }
}

void Adrenaline::checkEffectEnd(void* userData)
{
    Adrenaline* self = static_cast<Adrenaline*>(userData);
    if (!self) return;
    
    if (self->affectedCharacter) {
        self->affectedCharacter->setSpeedMultiplier(1.0);
    }
    
    if (self->healTimer) {
        self->healTimer->stop();
        delete self->healTimer;
        self->healTimer = nullptr;
    }
    
    self->affectedCharacter = nullptr;
    delete self;
}

void Adrenaline::healCharacter(void* userData)
{
    Adrenaline* self = static_cast<Adrenaline*>(userData);
    if (!self || !self->affectedCharacter) {
        if (self && self->healTimer) {
            self->healTimer->stop();
        }
        return;
    }
    
    int currentHealth = self->affectedCharacter->getLifeValue();
    int maxHealth = self->affectedCharacter->getMaxLifeValue();
    
    if (currentHealth >= maxHealth) {
        return;
    }
    
    int newHealth = qMin(currentHealth + HEAL_AMOUNT, maxHealth);
    self->affectedCharacter->setLifeValue(newHealth);
}
