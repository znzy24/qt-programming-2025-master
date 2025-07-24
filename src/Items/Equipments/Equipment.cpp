#include "Equipment.h"

Equipment::Equipment(QGraphicsItem* parent, const QString& pixmapPath)
    : Item(parent, pixmapPath)
{
    // 装备的通用初始化代码
}

void Equipment::mountToParent()
{
    // 装备被拾起时调用
    Mountable::mountToParent();
}

void Equipment::unmount()
{
    // 装备被丢弃时调用
    Mountable::unmount();
    setParentItem(nullptr);
}
