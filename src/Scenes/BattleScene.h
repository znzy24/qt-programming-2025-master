#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Bar/HealthBar.h"
#include "../Items/Bar/WeaponBar.h"

class BattleScene : public Scene {
Q_OBJECT

public:
    explicit BattleScene(QObject *parent);

    void processInput() override;

    void processMovement() override;

    void processPicking() override;

protected slots:

    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:

    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());

    void * pickupMountable(Character *character, Mountable *mountable);

    Map *map;

    Character *character;
    Character *character2;

    HealthBar* leftBar = nullptr;
    HealthBar* rightBar = nullptr;

    Weapon *spareWeapon = nullptr;

    WeaponBar* leftWeaponBar = nullptr;
    WeaponBar* rightWeaponBar = nullptr;
};


#endif
