#ifndef QT_PROGRAMMING_2024_EQUIPMENT_H
#define QT_PROGRAMMING_2024_EQUIPMENT_H

#include "../Item.h"
#include "../Mountable.h"

class Character;

// 装备基类
class Equipment : public Item, public Mountable {
public:
    explicit Equipment(QGraphicsItem* parent = nullptr, const QString& pixmapPath = "");
    virtual ~Equipment() = default;
    
    // 被角色使用时的效果
    virtual void useBy(Character* character) = 0;
    
    // 实现Mountable接口
    void mountToParent() override;
    void unmount() override;
};

#endif // QT_PROGRAMMING_2024_EQUIPMENT_H
