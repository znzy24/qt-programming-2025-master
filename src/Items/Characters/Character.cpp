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

bool Character::isAttackDown() const {
    return attackDown;
}

void Character::setAttackDown(bool attackDown) {
    this->attackDown = attackDown;
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
    const auto baseSpeed = 1.0;
    const auto jumpSpeed = -1.5;

    PlatformType platformType = PlatformType::Soil;
    qreal dummyY;
    isStandingOnPlatform(&dummyY, &platformType);

    // 应用移动速度乘数（正常速度 * 平台速度修正 * 装备速度加成）
    qreal moveSpeed = baseSpeed * speedMultiplier;
    
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

    updateWeaponPosition();
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

void Character::pickupWeapon(Weapon* newWeapon) {
    Weapon* oldWeapon = weapon;

    if (oldWeapon) {
        oldWeapon->unmount();
        scene()->removeItem(oldWeapon);
        delete oldWeapon;
    }
    
    if (newWeapon == nullptr) {
        weapon = nullptr;
        // 如果没有武器，恢复默认点数
        setWeaponPoints(1, 1);
        return;
    }

    weapon = newWeapon;
    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    newWeapon->setVisible(true);
    newWeapon->setZValue(5);
    updateWeaponPosition();
    
    // 根据武器自身定义更新武器点数
    setWeaponPoints(newWeapon->getInitialPoints(), newWeapon->getMaxPoints());
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
        // 检查武器是否需要消耗点数
        int weaponCost = weapon->getCost();
        
        if (weaponCost > 0 && currentWeaponPoints < weaponCost) {
            // 武器点数不足，无法攻击
            return;
        }
        
        // 执行攻击
        weapon->attack(this);
        
        // 消耗武器点数（小刀和拳头的cost为0，不消耗点数）
        if (weaponCost > 0) {
            consumeWeaponPoint(weaponCost);
        }
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

void Character::updateWeaponPosition() {
    if (weapon == nullptr) return;
    
    // 设置武器相对于角色的固定位置（右手位置）
    QPointF weaponOffset(-25, -30); // 相对位置
    weapon->setPos(weaponOffset);
    weapon->setTransform(QTransform().scale(1, 1)); // 保持武器不额外翻转
}

int Character::getLifeValue() const {
    return lifevalue;
}

int Character::getMaxLifeValue() const {
    return 100;
}

void Character::setWeaponPoints(int points, int maxPoints) {
    currentWeaponPoints = points;
    maxWeaponPoints = maxPoints;
    return;
}

int Character::getMaxWeaponPoints() const {
    return maxWeaponPoints;
}

int Character::getCurrentWeaponPoints() const {
    return currentWeaponPoints;
}

void Character::setLifeValue(int lifeValue) {
    lifevalue = qBound(0, lifeValue, getMaxLifeValue());
    return;
}

void Character::consumeWeaponPoint(int point) {
    if (currentWeaponPoints > 0) {
        currentWeaponPoints -= point;
        if (currentWeaponPoints < 0) {
            currentWeaponPoints = 0;
        }
    }
    return;
}

qreal Character::getSpeedMultiplier() const {
    return speedMultiplier;
}

void Character::setSpeedMultiplier(qreal multiplier) {
    speedMultiplier = multiplier;
}
