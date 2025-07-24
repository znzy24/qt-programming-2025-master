#ifndef QT_PROGRAMMING_2024_BANDAGE_H
#define QT_PROGRAMMING_2024_BANDAGE_H

#include "Equipment.h"

// 绷带：拾取后立刻恢复少量血量
class Bandage : public Equipment {
public:
    explicit Bandage(QGraphicsItem* parent = nullptr);
    
    // 实现useBy方法：恢复角色少量血量
    void useBy(Character* character) override;
    
private:
    int healAmount = 20; // 绷带恢复的血量值
};

#endif // QT_PROGRAMMING_2024_BANDAGE_H
