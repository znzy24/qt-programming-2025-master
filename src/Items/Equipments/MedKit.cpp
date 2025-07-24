#include "MedKit.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>

MedKit::MedKit(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/MedKit.png")
{
    // 设置适当的大小
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
    
    // 恢复全部血量
    int maxHealth = character->getMaxLifeValue();
    character->setLifeValue(maxHealth);
    
    // 使用后从场景中移除
    if (scene()) {
        scene()->removeItem(this);
        delete this; // 直接删除，不使用deleteLater()
    }
}
