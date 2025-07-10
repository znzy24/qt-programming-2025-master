//
// Created by gerw on 8/20/24.
//

#include <memory>
#include "Link.h"

Link::Link(Map* map) : Character(nullptr) {
    this->map = map;
    armor = nullptr;
}
