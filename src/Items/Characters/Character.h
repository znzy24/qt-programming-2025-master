#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../Weapons/Weapon.h"
#include "../Maps/Map.h"
#include "../Maps/Platform.h"
#include "../Weapons/Fist.h"
#include "../Weapons/Knife.h"
#include "../Weapons/SolidBall.h"

enum PoseState { Stand, Crouch };

class Character : public Item {
public:
    explicit Character(QGraphicsItem *parent);

    [[nodiscard]] bool isLeftDown() const;
    void setLeftDown(bool leftDown);
    [[nodiscard]] bool isRightDown() const;
    void setRightDown(bool rightDown);
    [[nodiscard]] bool isPickDown() const;
    void setPickDown(bool pickDown);
    [[nodiscard]] bool isJumpDown() const;
    void setJumpDown(bool jumpDown);
    [[nodiscard]] bool isAttackDown() const;
    void setAttackDown(bool attackDown);
    [[nodiscard]] bool isFacingLeft() const;

    void setCrouchPose();
    void setStandPose();
    [[nodiscard]] bool isCrouching() const;
    
    void pickupWeapon(Weapon* newWeapon);
    [[nodiscard]] bool isPicking() const;

    [[nodiscard]] const QPointF &getVelocity() const;
    void setVelocity(const QPointF &velocity);

    void processInput();
    
    int getLifeValue() const;
    int getMaxLifeValue() const;
    void setLifeValue(int lifeValue);
    int getCurrentWeaponPoints() const;
    int getMaxWeaponPoints() const;
    void setWeaponPoints(int points, int maxPoints = 1);

    void attack();
    void takeDamage(int damage);
    void consumeWeaponPoint(int point);
    void updateWeaponPosition();
    
    // 获取和设置速度乘数（用于肾上腺素等装备的速度加成效果）
    qreal getSpeedMultiplier() const;
    void setSpeedMultiplier(qreal multiplier);
protected:
    Weapon *weapon{};
    QPointF velocity{};
    PoseState poseState = Stand;
    PlatformType currentPlatformType = PlatformType::Soil;
    bool leftDown{};
    bool rightDown{};
    bool pickDown{};
    bool lastPickDown{};
    bool picking{};
    bool crouching{};
    bool jumpDown{};
    bool canJump{true}; 
    int lifevalue = 100;
    qreal nearestPlatformY = 1e9;
    bool isStandingOnPlatform(qreal* platformY = nullptr, PlatformType* type = nullptr);
    bool attackDown{};
    bool facingLeft{false};
    int currentWeaponPoints = 1;
    int maxWeaponPoints = 1;
    qreal speedMultiplier = 1.0; // 速度乘数，默认为1.0，可被装备提升
};


#endif
