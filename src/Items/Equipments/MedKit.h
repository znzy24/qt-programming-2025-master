#ifndef QT_PROGRAMMING_2024_MEDKIT_H
#define QT_PROGRAMMING_2024_MEDKIT_H

#include "Equipment.h"

// 医疗箱：拾取后立刻恢复全部血量
class MedKit : public Equipment {
public:
    explicit MedKit(QGraphicsItem* parent = nullptr);
    
    // 实现useBy方法：恢复角色全部血量
    void useBy(Character* character) override;
};

#endif // QT_PROGRAMMING_2024_MEDKIT_H
