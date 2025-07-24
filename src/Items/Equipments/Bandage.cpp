#include "Bandage.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

Bandage::Bandage(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/Bandage.png")
{
    // 设置适当的大小
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
}

void Bandage::useBy(Character* character)
{
    if (!character) return;
    
    // 恢复少量血量
    int currentHealth = character->getLifeValue();
    int maxHealth = character->getMaxLifeValue();
    int newHealth = qMin(currentHealth + healAmount, maxHealth);
    character->setLifeValue(newHealth);
    
    // 使用后从场景中移除
    if (scene()) {
        scene()->removeItem(this);
        delete this; // 直接删除，不使用deleteLater()
    }
}
