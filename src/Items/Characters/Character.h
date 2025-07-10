//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../Armors/Armor.h"
#include "../Maps/Map.h"

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

    [[nodiscard]] const QPointF &getVelocity() const;

    [[nodiscard]] bool isPicking() const;

    [[nodiscard]] bool isCrouching() const;

    void setVelocity(const QPointF &velocity);

    void processInput();

    Armor* pickupArmor(Armor* newArmor);

    [[nodiscard]] bool isJumpDown() const;

    void setJumpDown(bool jumpDown);

    void setCrouchPose();

    void setStandPose();

protected:
    Armor *armor{};
    QPointF velocity{};
    PoseState poseState = Stand;
//    QGraphicsEllipseItem *ellipseItem; // for debugging
    bool leftDown{};
    bool rightDown{};
    bool pickDown{};
    bool lastPickDown{};
    bool picking{};
    bool crouching{};
    bool jumpDown{};
    bool canJump{true}; 
    Map* map = nullptr;
    int lifevalue = 100;
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
