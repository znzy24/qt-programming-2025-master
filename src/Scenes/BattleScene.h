#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
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
    
    bool checkGameOver();

protected slots:

    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:

    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());

    void pickupMountable(Character *character, Mountable *mountable);

    Map *map = nullptr;

    Character *character = nullptr;
    Character *character2 = nullptr;

    HealthBar* leftBar = nullptr;
    HealthBar* rightBar = nullptr;

    Weapon *spareWeapon = nullptr;

    WeaponBar* leftWeaponBar = nullptr;
    WeaponBar* rightWeaponBar = nullptr;
    
    bool gameOver = false;
    int winner = 0;
    QGraphicsTextItem* gameOverText = nullptr;
    
    QTimer* itemDropTimer = nullptr;
    
    QTimer* itemCleanupTimer = nullptr;
    
    struct DroppedItemInfo {
        Item* item;
        qint64 dropTime;
        bool isPicked = false;
    };
    QList<DroppedItemInfo> droppedItems;
    
    void spawnRandomItem();
    
    void cleanupOldItems();
};


#endif
