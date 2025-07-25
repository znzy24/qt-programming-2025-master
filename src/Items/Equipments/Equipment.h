#ifndef QT_PROGRAMMING_2024_EQUIPMENT_H
#define QT_PROGRAMMING_2024_EQUIPMENT_H

#include "../Item.h"
#include "../Mountable.h"

class Character;

class Equipment : public Item, public Mountable {
public:
    explicit Equipment(QGraphicsItem* parent = nullptr, const QString& pixmapPath = "");
    virtual ~Equipment() = default;
    
    virtual void useBy(Character* character) = 0;
    
    void mountToParent() override;
    void unmount() override;
};

#endif
