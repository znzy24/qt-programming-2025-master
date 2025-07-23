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
    
    // 检查游戏是否结束
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
    
    // 游戏是否已经结束
    bool gameOver = false;
    // 胜利的角色（1表示左侧角色胜利，2表示右侧角色胜利）
    int winner = 0;
    // 游戏结束的文本显示
    QGraphicsTextItem* gameOverText = nullptr;
    
    // 物品掉落计时器
    QTimer* itemDropTimer = nullptr;
    
    // 生成随机掉落物品
    void spawnRandomItem();
};


#endif
