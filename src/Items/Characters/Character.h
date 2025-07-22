#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../Weapons/Weapon.h"
#include "../Maps/Map.h"
#include "../Maps/Platform.h"
#include "../Weapons/Fist.h"
#include "../Weapons/Knife.h"

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


    
    [[nodiscard]] const QPointF &getVelocity() const;
    [[nodiscard]] bool isPicking() const;
    [[nodiscard]] bool isCrouching() const;
    void setVelocity(const QPointF &velocity);
    void processInput();
    void pickupWeapon(Weapon* newWeapon);
    
    void setCrouchPose();
    void setStandPose();
    int getLifeValue() const { return lifevalue; }
    int getMaxLifeValue() const { return 100; }
    
    void attack();
    void takeDamage(int damage);
    [[nodiscard]] bool isFacingLeft() const;
    void updateWeaponPosition();
    int getCurrentWeaponPoints() const { return currentWeaponPoints; }
    int getMaxWeaponPoints() const { return maxWeaponPoints; }
    void setWeaponPoints(int points, int maxPoints = 1) { 
        currentWeaponPoints = points; 
        maxWeaponPoints = maxPoints; 
    }
    void consumeWeaponPoint(int point) { 
        if (currentWeaponPoints > 0) currentWeaponPoints -= point; 
    }
protected:
    Weapon *weapon{};
    QPointF velocity{};
    PoseState poseState = Stand;
//    QGraphicsEllipseItem *ellipseItem; // for debugging
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
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
