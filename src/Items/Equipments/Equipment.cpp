#include "Equipment.h"

Equipment::Equipment(QGraphicsItem* parent, const QString& pixmapPath)
    : Item(parent, pixmapPath)
{
}

void Equipment::mountToParent()
{
    Mountable::mountToParent();
}

void Equipment::unmount()
{
    Mountable::unmount();
    setParentItem(nullptr);
}
