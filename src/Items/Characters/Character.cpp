#include <QTransform>
#include <QDebug>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include "Character.h"


Character::Character(QGraphicsItem *parent, bool useAlternateSprite) : 
    Item(parent, useAlternateSprite ? ":/Items/Characters/CharacterStand2.png" : ":/Items/Characters/CharacterStand.png"),
    useAlternateSprite(useAlternateSprite) {
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(80, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());

        originalPixmap = scaled;
    }
    setZValue(10);
    
    facingLeft = false; 
    setTransform(QTransform().scale(-1, 1)); 
    
    attackAnimationTimer = new QTimer();
    attackAnimationTimer->setSingleShot(true);
    QObject::connect(attackAnimationTimer, &QTimer::timeout, [this]() {
        hideAttackEffect();
    });
    
    damageAnimationTimer = new QTimer();
    damageAnimationTimer->setSingleShot(true);
    QObject::connect(damageAnimationTimer, &QTimer::timeout, [this]() {
        hideDamageEffect();
    });
}

Character::~Character() {
    if (attackAnimationTimer) {
        attackAnimationTimer->stop();
        delete attackAnimationTimer;
        attackAnimationTimer = nullptr;
    }
    
    if (damageAnimationTimer) {
        damageAnimationTimer->stop();
        delete damageAnimationTimer;
        damageAnimationTimer = nullptr;
    }
    
    if (attackEffectItem) {
        if (attackEffectItem->scene()) {
            attackEffectItem->scene()->removeItem(attackEffectItem);
        }
        delete attackEffectItem;
        attackEffectItem = nullptr;
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
        QString crouchPath = useAlternateSprite ? ":/Items/Characters/CharacterCrouch2.png" : ":/Items/Characters/CharacterCrouch.png";
        QPixmap crouchPixmap(crouchPath);
        QPixmap scaled = crouchPixmap.scaled(80
            , 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
        originalPixmap = scaled;
    }
    setZValue(10);
    poseState = Crouch;
}

void Character::setStandPose() {
    if (poseState == Stand) return;
    if (pixmapItem) {
        QString standPath = useAlternateSprite ? ":/Items/Characters/CharacterStand2.png" : ":/Items/Characters/CharacterStand.png";
        QPixmap standPixmap(standPath);
        QPixmap scaled = standPixmap.scaled(80, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());
        originalPixmap = scaled;
    }
    setZValue(10);
    poseState = Stand;
}

void Character::processInput() {
    crouching = isPickDown();
    auto velocity = this->velocity;
    const auto baseSpeed = 0.6;
    const auto jumpSpeed = -1.5;

    PlatformType platformType = PlatformType::Soil;
    qreal dummyY;
    isStandingOnPlatform(&dummyY, &platformType);

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
        if (weapon) weapon->setVisible(false);
    } else {
        if (pixmapItem) pixmapItem->setVisible(true);
        if (weapon) weapon->setVisible(true);
    }

    if (!lastPickDown && pickDown) { 
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;

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
        setWeaponPoints(1, 1);
        return;
    }

    weapon = newWeapon;
    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    newWeapon->setVisible(true);
    newWeapon->setZValue(5);
    updateWeaponPosition();
    
    setWeaponPoints(newWeapon->getInitialPoints(), newWeapon->getMaxPoints());
}

bool Character::isFacingLeft() const {
    return facingLeft;
}

void Character::attack() {
    static qint64 lastAttackTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    int cooldownTime = 500;
    
    if (weapon) {
        switch (weapon->getWeaponType()) {
            case WeaponType::Knife:
                cooldownTime = 400; 
                break;
            case WeaponType::SolidBall:
                cooldownTime = 800; 
                break;
            case WeaponType::Rifle:
                cooldownTime = 350; 
                break;
            case WeaponType::Sniper:
                cooldownTime = 1200; 
                break;
            default:
                cooldownTime = 500;
                break;
        }
    } else {
        cooldownTime = 450; 
    }
    
    if (currentTime - lastAttackTime < cooldownTime) {
        return;
    }
    lastAttackTime = currentTime;
    
    if (weapon) {
        int weaponCost = weapon->getCost();
        
        if (weaponCost > 0 && currentWeaponPoints < weaponCost) {
            return;
        }
        
        showAttackEffect();
        
        weapon->attack(this);
        
        if (weaponCost > 0) {
            consumeWeaponPoint(weaponCost);
        }
        return;
    } else {
        showAttackEffect();
    }
    
    static Fist defaultFist;
    defaultFist.attack(this);
}

void Character::takeDamage(int damage) {
    lifevalue = qMax(0, lifevalue - damage);
    
    if (!isDamageAnimationActive && damage > 0) {
        showDamageEffect();
    }
}

void Character::updateWeaponPosition() {
    if (weapon == nullptr) return;
    
    QPointF weaponOffset(-25, -30); 
    
    switch (weapon->getWeaponType()) {
        case WeaponType::Fist:
            weaponOffset = QPointF(-20, -20);
            break;
        case WeaponType::Knife:
            weaponOffset = QPointF(-45, -67);
            break;
        case WeaponType::SolidBall:
            weaponOffset = QPointF(-30, -65);
            break;
        case WeaponType::Rifle:
            weaponOffset = QPointF(-20, -65);
            break;
        case WeaponType::Sniper:
            weaponOffset = QPointF(-20, -65);
            break;
        default:
            break;
    }
    
    if (poseState == Crouch) {
        weaponOffset.setY(weaponOffset.y() + 5);
    }
    
    weapon->setPos(weaponOffset);
    weapon->setTransform(QTransform().scale(1, 1)); 
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

void Character::showAttackEffect() {
    if (isAttackAnimationActive) return;
    
    isAttackAnimationActive = true;
    
    if (!attackEffectItem) {
        QPixmap effectPixmap;
        
        if (weapon) {
            switch (weapon->getWeaponType()) {
                case WeaponType::Knife: {
                    effectPixmap = QPixmap(":/Items/Characters/KnifeAttack.png"); 
                    effectPixmap = effectPixmap.scaled(50, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    break;
                }
                case WeaponType::SolidBall: {
                    effectPixmap = QPixmap(1, 1); 
                    effectPixmap.fill(Qt::transparent);
                    break;
                }
                case WeaponType::Rifle: {
                    effectPixmap = QPixmap(1, 1);
                    effectPixmap.fill(Qt::transparent);
                    break;
                }
                case WeaponType::Sniper: {
                    effectPixmap = QPixmap(1, 1);
                    effectPixmap.fill(Qt::transparent);
                    break;
                }
                default: {
                    effectPixmap = QPixmap(32, 32);
                    effectPixmap.fill(Qt::transparent);
                    QPainter painter(&effectPixmap);
                    painter.setPen(QPen(Qt::red, 2));
                    painter.setBrush(QBrush(QColor(255, 0, 0, 128)));
                    painter.drawEllipse(0, 0, 30, 30);
                    break;
                }
            }
        } else {
            effectPixmap = QPixmap(":/Items/Characters/FistAttack.png"); 
            effectPixmap = effectPixmap.scaled(50, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        
        attackEffectItem = new QGraphicsPixmapItem(effectPixmap);
        attackEffectItem->setZValue(15);
        scene()->addItem(attackEffectItem);
    }
    
    QPointF effectPos = pos();
    int effectDuration = 300; 
    
    if (weapon) {
        switch (weapon->getWeaponType()) {
            case WeaponType::Rifle:
            case WeaponType::Sniper:
            case WeaponType::SolidBall: {
                attackEffectItem->setVisible(false);
                effectDuration = 10;
                break;
            }
            case WeaponType::Knife: {
                if (facingLeft) {
                    effectPos.setX(effectPos.x() - 40);
                } else {
                    effectPos.setX(effectPos.x() + 40);
                }
                effectPos.setY(effectPos.y() - 80); 
                effectDuration = 250; 
                break;
            }
            default:
                break;
        }
    } else {
        if (facingLeft) {
            effectPos.setX(effectPos.x() - 20);
        } else {
            effectPos.setX(effectPos.x() + 20);
        }
        effectPos.setY(effectPos.y() - 80); 

        effectDuration = 300; 
    }
    
    attackEffectItem->setPos(effectPos);
    attackEffectItem->setVisible(true);
    
    if (facingLeft) { 
        if (!weapon || weapon->getWeaponType() == WeaponType::Knife) {
            attackEffectItem->setTransform(QTransform().scale(-1, 1));
        }
    } else {
        if (!weapon || weapon->getWeaponType() == WeaponType::Knife) {
            attackEffectItem->setTransform(QTransform().scale(1, 1));
        }
    }
    
    attackAnimationTimer->start(effectDuration);
}

void Character::hideAttackEffect() {
    if (attackEffectItem) {
        attackEffectItem->setVisible(false);
        if (attackEffectItem->scene()) {
            attackEffectItem->scene()->removeItem(attackEffectItem);
        }
        delete attackEffectItem;
        attackEffectItem = nullptr;
    }
    isAttackAnimationActive = false;
}

void Character::showDamageEffect() {
    if (isDamageAnimationActive) return;
    
    isDamageAnimationActive = true;
    damageFlashCount = 0;
    
    if (pixmapItem) {
        QPixmap redPixmap = originalPixmap;
        QPainter painter(&redPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.fillRect(redPixmap.rect(), QColor(255, 0, 0, 128));
        painter.end();
        
        pixmapItem->setPixmap(redPixmap);
    }
    
    damageAnimationTimer->start(100);
}

void Character::hideDamageEffect() {
    damageFlashCount++;
    
    if (damageFlashCount >= 4) {
        if (pixmapItem) {
            pixmapItem->setPixmap(originalPixmap);
        }
        isDamageAnimationActive = false;
        return;
    }
    
    if (damageFlashCount % 2 == 0) {
        if (pixmapItem) {
            QPixmap redPixmap = originalPixmap;
            QPainter painter(&redPixmap);
            painter.setCompositionMode(QPainter::CompositionMode_Overlay);
            painter.fillRect(redPixmap.rect(), QColor(255, 0, 0, 128));
            painter.end();
            pixmapItem->setPixmap(redPixmap);
        }
    } else {
        if (pixmapItem) {
            pixmapItem->setPixmap(originalPixmap);
        }
    }
    damageAnimationTimer->start(100);
}
