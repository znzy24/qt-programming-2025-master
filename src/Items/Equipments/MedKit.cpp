#include "MedKit.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

MedKit::MedKit(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/MedKit.png")
{
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void MedKit::useBy(Character* character)
{
    if (!character) return;
    
    int maxHealth = character->getMaxLifeValue();
    character->setLifeValue(maxHealth);
    
    if (scene()) {
        scene()->removeItem(this);
        delete this;
    }
}
