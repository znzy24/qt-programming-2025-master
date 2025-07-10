//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include <QDebug>
#include "Character.h"


Character::Character(QGraphicsItem *parent) : Item(parent, ":/Items/Characters/CharacterStand.png") {
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(120, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
}

bool Character::isLeftDown() const {
    return leftDown;
}

void Character::setLeftDown(bool leftDown) {
    Character::leftDown = leftDown;
}

bool Character::isRightDown() const {
    return rightDown;
}

void Character::setRightDown(bool rightDown) {
    Character::rightDown = rightDown;
}

bool Character::isPickDown() const {
    return pickDown;
}

void Character::setPickDown(bool pickDown) {
    Character::pickDown = pickDown;
}

bool Character::isJumpDown() const {
    return jumpDown;
}

void Character::setJumpDown(bool jumpDown) {
    Character::jumpDown = jumpDown;
}

const QPointF &Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
    qreal groundY = 0;
    if (map) {
        groundY = map->getFloorHeight();
    }
    if (velocity.y() > 0 && pos().y() >= groundY) {
        setY(groundY); 
        Character::velocity.setY(0);
        canJump = true;
    }
}

void Character::setCrouchPose() {
    if (poseState == Crouch) return;
    if (pixmapItem) {
        QPixmap crouchPixmap(":/Items/Characters/CharacterCrouch.png");
        QPixmap scaled = crouchPixmap.scaled(120, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
    poseState = Crouch;
}

void Character::setStandPose() {
    if (poseState == Stand) return;
    if (pixmapItem) {
        QPixmap standPixmap(":/Items/Characters/CharacterStand.png");
        QPixmap scaled = standPixmap.scaled(120, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
    poseState = Stand;
}

void Character::processInput() {
    crouching = isPickDown();
    auto velocity = this->velocity;
    const auto moveSpeed = 1.0;
    const auto jumpSpeed = -3.0;
    const auto gravity = 0.2;
    if (!crouching) {
        velocity.setX(0);
        if (isLeftDown()) {
            velocity.setX(velocity.x() - moveSpeed);
            setTransform(QTransform().scale(1, 1));
        }
        if (isRightDown()) {
            velocity.setX(velocity.x() + moveSpeed);
            setTransform(QTransform().scale(-1, 1));
        }
        if (isJumpDown() && canJump) {
            velocity.setY(jumpSpeed);
            canJump = false; 
        }
    } else {
        velocity.setX(0); 
    }
    velocity.setY(velocity.y() + gravity);
    setVelocity(velocity);

    if (!lastPickDown && pickDown) { // first time pickDown
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;

    // 姿态切换
    if (crouching) {
        setCrouchPose(); 
    } 
    else {
        setStandPose();
    }
}

bool Character::isPicking() const {
    return picking;
}

bool Character::isCrouching() const {
    return crouching;
}

Armor *Character::pickupArmor(Armor *newArmor) {
    auto oldArmor = armor;
    if (oldArmor != nullptr) {
        oldArmor->unmount();
        oldArmor->setPos(newArmor->pos());
        oldArmor->setParentItem(parentItem());
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;
    return oldArmor;
}

