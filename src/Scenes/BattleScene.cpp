//
// Created by gerw on 8/20/24.
//

#include <QDebug>
#include "BattleScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Maps/Platform.h"

BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    // This is useful if you want the scene to have the exact same dimensions as the view
    setSceneRect(0, 0, 1280, 720);
    map = new Battlefield();
    character = new Link(map);
    character2 = new Link(map); 
    addItem(map);
    addItem(character);
    addItem(character2);
    character->setPos(100, 650);
    character2->setPos(800, 650);
    addItem(new Platform(PlatformType::Soil, 0, 670, 26));
    addItem(new Platform(PlatformType::Soil, 50, 520, 8));
    addItem(new Platform(PlatformType::Grass, 820, 520, 8));
    addItem(new Platform(PlatformType::Ice, 350, 370, 11));
    addItem(new Platform(PlatformType::Soil, 820, 220, 8));
    addItem(new Platform(PlatformType::Grass, 50, 220, 8));
}

void BattleScene::processInput() {
    Scene::processInput();
    if (character != nullptr) {
        character->processInput();
    }
    if (character2 != nullptr) {
        character2->processInput();
    }
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_A:
            if (character != nullptr) character->setLeftDown(true); break;
        case Qt::Key_D:
            if (character != nullptr) character->setRightDown(true); break;
        case Qt::Key_J:
            if (character != nullptr) character->setPickDown(true); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(true); break;
        case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(true); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(true); break;
        case Qt::Key_1:
            if (character2 != nullptr) character2->setPickDown(true); break;
        case Qt::Key_Up:
            if (character2 != nullptr) character2->setJumpDown(true); break;
        default:
            Scene::keyPressEvent(event);
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_A:
            if (character != nullptr) character->setLeftDown(false); break;
        case Qt::Key_D:
            if (character != nullptr) character->setRightDown(false); break;
        case Qt::Key_J:
            if (character != nullptr) character->setPickDown(false); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(false); break;
        case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(false); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(false); break;
        case Qt::Key_1:
            if (character2 != nullptr) character2->setPickDown(false); break;
        case Qt::Key_Up:
            if (character2 != nullptr) character2->setJumpDown(false); break;
        default:
            Scene::keyReleaseEvent(event);
    }
}

void BattleScene::update() {
    Scene::update();
}

void BattleScene::processMovement() {
    Scene::processMovement();
    QRectF bounds = sceneRect();
    if (character != nullptr) {
        QPointF newPos = character->pos() + character->getVelocity() * (double)deltaTime;
        QSizeF pixSize = character->boundingRect().size();
        qreal halfW = pixSize.width() / 2;
        qreal fullH = pixSize.height();
        qreal leftLimit = bounds.left() + halfW;
        qreal rightLimit = bounds.left() + bounds.width() - halfW;
        newPos.setX(qBound(leftLimit, newPos.x(), rightLimit));
        newPos.setY(qBound(bounds.top() + fullH, newPos.y(), bounds.bottom()));
        character->setPos(newPos);
    }
    if (character2 != nullptr) {
        QPointF newPos2 = character2->pos() + character2->getVelocity() * (double)deltaTime;
        QSizeF pixSize2 = character2->boundingRect().size();
        qreal halfW2 = pixSize2.width() / 2;
        qreal fullH2 = pixSize2.height();
        qreal leftLimit2 = bounds.left() + halfW2;
        qreal rightLimit2 = bounds.left() + bounds.width() - halfW2;
        newPos2.setX(qBound(leftLimit2, newPos2.x(), rightLimit2));
        newPos2.setY(qBound(bounds.top() + fullH2, newPos2.y(), bounds.bottom()));
        character2->setPos(newPos2);
    }
    
}

void BattleScene::processPicking() {
    Scene::processPicking();
    if (character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100.);
        if (mountable != nullptr) {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(character, mountable));
        }
    }
}

Mountable *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;

    for (QGraphicsItem *item: items()) {
        if (auto mountable = dynamic_cast<Mountable *>(item)) {
            if (!mountable->isMounted()) {
                qreal distance = QLineF(pos, item->pos()).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }

    return nearest;
}

Mountable *BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    // Limitation: currently only supports armor
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    return nullptr;
}
