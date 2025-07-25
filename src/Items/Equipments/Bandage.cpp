#include "Bandage.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Bandage::Bandage(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/Bandage.png")
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(35, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Bandage::useBy(Character* character)
{
    if (!character) return;
    int currentHealth = character->getLifeValue();
    int maxHealth = character->getMaxLifeValue();
    int newHealth = qMin(currentHealth + healAmount, maxHealth);
    character->setLifeValue(newHealth);
    
    if (scene()) {
        scene()->removeItem(this);
        delete this;
    }
}
