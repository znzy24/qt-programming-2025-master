#include <QTransform>
#include <QDebug>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include "Character.h"


Character::Character(QGraphicsItem *parent) : Item(parent, ":/Items/Characters/CharacterStand.png") {
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(80, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height());

        originalPixmap = scaled;
    }
    setZValue(10);
    
    // 初始化朝向设置（默认朝右）
    facingLeft = false;  // 默认朝右
    setTransform(QTransform().scale(-1, 1));  // 设置图像朝右的变换
    
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
        QPixmap crouchPixmap(":/Items/Characters/CharacterCrouch.png");
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
        QPixmap standPixmap(":/Items/Characters/CharacterStand.png");
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
    
    // 根据武器类型设置不同的冷却时间
    int cooldownTime = 500; // 默认冷却时间（毫秒）
    
    if (weapon) {
        // 根据武器类型设置不同的冷却时间
        switch (weapon->getWeaponType()) {
            case WeaponType::Knife:
                cooldownTime = 400; // 小刀攻击冷却时间较短
                break;
            case WeaponType::SolidBall:
                cooldownTime = 800; // 投掷物冷却时间较长
                break;
            case WeaponType::Rifle:
                cooldownTime = 350; // 步枪冷却时间
                break;
            case WeaponType::Sniper:
                cooldownTime = 1200; // 狙击枪冷却时间最长
                break;
            default:
                cooldownTime = 500; // 默认冷却时间
                break;
        }
    } else {
        cooldownTime = 450; // 拳头攻击的冷却时间
    }
    
    // 检查是否已经冷却完毕
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
    
    QPointF weaponOffset(-25, -30);  // 默认偏移量
    
    // 根据武器类型调整位置
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
    
    // 如果角色是下蹲状态，调整武器位置更靠下一些
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
                    // 投掷类武器不需要额外的攻击动画，因为有飞行物动画
                    effectPixmap = QPixmap(1, 1);  // 创建一个几乎不可见的空图像
                    effectPixmap.fill(Qt::transparent);
                    break;
                }
                case WeaponType::Rifle: {
                    // 射击类武器不需要额外的攻击动画，因为有子弹飞行动画
                    effectPixmap = QPixmap(1, 1);
                    effectPixmap.fill(Qt::transparent);
                    break;
                }
                case WeaponType::Sniper: {
                    // 射击类武器不需要额外的攻击动画，因为有子弹飞行动画
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
                // 投掷类和射击类武器不需要显示攻击效果
                attackEffectItem->setVisible(false);
                effectDuration = 10;  // 极短的持续时间，因为不需要显示效果
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
        // 拳头攻击效果位置 - 无论朝向如何都使用一致的X偏移，因为我们会通过翻转来处理朝向
        if (facingLeft) {
            effectPos.setX(effectPos.x() - 20);
        } else {
            effectPos.setX(effectPos.x() + 20);
        }
        effectPos.setY(effectPos.y() - 80);  // 调整高度到合适位置
        
        // 为拳头攻击设置适当的持续时间
        effectDuration = 300;  // 拳头攻击图片显示时间
    }
    
    attackEffectItem->setPos(effectPos);
    attackEffectItem->setVisible(true);
    
    // 根据角色朝向设置攻击图片的翻转
    if (facingLeft) {  // 朝左时翻转攻击效果
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
        // 移除并删除攻击效果图像，以便下次攻击时创建新的
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
