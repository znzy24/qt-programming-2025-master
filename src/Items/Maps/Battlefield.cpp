//
// Created by gerw on 8/21/24.
//

#include "Battlefield.h"

Battlefield::Battlefield(QGraphicsItem *parent) : Map(parent, ":/Items/Maps/Battlefield/BackGround.png") {}

qreal Battlefield::getFloorHeight() {
    auto sceneRect = sceneBoundingRect();
    return (sceneRect.top() + sceneRect.bottom()) * 0.63;
}
