//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include <QDebug>
#include <QDateTime>
#include "Character.h"


Character::Character(QGraphicsItem *parent) : Item(parent, ":/Items/Characters/CharacterStand.png") {
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(72, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
    setZValue(10);
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

bool Character::isStandingOnPlatform(qreal* platformY, PlatformType* type) {
    QPointF foot(pos().x(), pos().y() + 2);
    QList<QGraphicsItem*> items = scene()->items(foot);
    for (QGraphicsItem* item : items) {
        Platform* platform = dynamic_cast<Platform*>(item);
        if (platform && typeid(*platform) == typeid(Platform)) {
            qreal topY = platform->y();
            qreal leftX = platform->x();
            qreal rightX = platform->x() + platform->boundingRect().width();
            if (std::abs(topY - foot.y()) < 40 && foot.x() >= leftX && foot.x() <= rightX) {
                if (platformY) *platformY = topY;
                if (type) *type = platform->getType();
                return true;
            }
        }
    }
    return false;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
    qreal gravity = 0.1;
    qreal platformY = 0;
    if (velocity.y() >= 0 && isStandingOnPlatform(&platformY)) {
        setY(platformY);
        Character::velocity.setY(0);
        canJump = true;
    } else {
        Character::velocity.setY(Character::velocity.y() + gravity);
        canJump = false;
    }
}

void Character::setCrouchPose() {
    if (poseState == Crouch) return;
    if (pixmapItem) {
        QPixmap crouchPixmap(":/Items/Characters/CharacterCrouch.png");
        QPixmap scaled = crouchPixmap.scaled(72, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
    setZValue(10);
    poseState = Crouch;
}

void Character::setStandPose() {
    if (poseState == Stand) return;
    if (pixmapItem) {
        QPixmap standPixmap(":/Items/Characters/CharacterStand.png");
        QPixmap scaled = standPixmap.scaled(72, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
    }
    setZValue(10);
    poseState = Stand;
}

void Character::processInput() {
    crouching = isPickDown();
    auto velocity = this->velocity;
    const auto moveSpeed = 1.0;
    const auto jumpSpeed = -1.5;

    PlatformType platformType = PlatformType::Soil;
    qreal dummyY;
    isStandingOnPlatform(&dummyY, &platformType);

    qreal actualMoveSpeed = moveSpeed;
    if (platformType == PlatformType::Ice) {
        actualMoveSpeed = moveSpeed * 2.0;
    }

    if (!crouching) {
        velocity.setX(0);
        if (isLeftDown()) {
            velocity.setX(velocity.x() - actualMoveSpeed);
            setTransform(QTransform().scale(1, 1));
            facingLeft = true;
        }
        if (isRightDown()) {
            velocity.setX(velocity.x() + actualMoveSpeed);
            setTransform(QTransform().scale(-1, 1));
            facingLeft = false;
        }
        if (isJumpDown() && canJump) {
            velocity.setY(jumpSpeed);
            canJump = false;
        }
    } else {
        velocity.setX(0);
    }
    if (attackDown) {
        attack();
    }
    setVelocity(velocity);

    if (platformType == PlatformType::Grass && crouching) {
        if (pixmapItem) pixmapItem->setVisible(false);
    } else {
        if (pixmapItem) pixmapItem->setVisible(true);
    }

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

Weapon* Character::pickupWeapon(Weapon* newWeapon) {
    if (newWeapon == nullptr) return nullptr;
    
    Weapon* oldWeapon = weapon;
    weapon = newWeapon;
    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    
    if (oldWeapon) {
        oldWeapon->unmount();
    }
    
    return oldWeapon;
}


bool Character::isAttackDown() const {
    return attackDown;
}

void Character::setAttackDown(bool attackDown) {
    this->attackDown = attackDown;
}

bool Character::isFacingLeft() const {
    return facingLeft;
}

void Character::attack() {
    static qint64 lastAttackTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // 攻击冷却时间 500ms
    if (currentTime - lastAttackTime < 500) {
        return;
    }
    lastAttackTime = currentTime;
    
    if (weapon) {
        weapon->attack(this);
        return;
    }
    
    // 无武器时使用默认拳头攻击
    static Fist defaultFist; // 静态默认拳头武器
    defaultFist.attack(this);
}

void Character::takeDamage(int damage) {
    lifevalue = qMax(0, lifevalue - damage);
    // 可以在这里添加受伤效果，如闪烁、击退等
}
