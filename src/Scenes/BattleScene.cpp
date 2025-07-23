#include <QDebug>
#include <QRandomGenerator>
#include "BattleScene.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Maps/Platform.h"
#include "../Items/Weapons/SolidBall.h"
#include "../Items/Weapons/Rifle.h"
#include "../Items/Weapons/Sniper.h"
#include "../Items/FallingItem.h"
#include "../Items/Weapons/Knife.h"

BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);
    map = new Battlefield();
    character = new Character(nullptr);
    character2 = new Character(nullptr);
    addItem(map);
    addItem(character);
    addItem(character2);
    character->setPos(100, 650);
    character2->setPos(800, 650);
    
    // 初始化游戏结束文本，但不显示
    gameOverText = new QGraphicsTextItem();
    gameOverText->setDefaultTextColor(Qt::red);
    QFont gameOverFont;
    gameOverFont.setPointSize(36);
    gameOverFont.setBold(true);
    gameOverText->setFont(gameOverFont);
    gameOverText->setZValue(1001); // 确保显示在最上层
    gameOverText->setPos(sceneRect().width() / 2 - 150, sceneRect().height() / 2 - 50); // 居中显示
    gameOverText->setVisible(false);
    addItem(gameOverText);
    
    // 初始化物品掉落计时器
    itemDropTimer = new QTimer(this);
    connect(itemDropTimer, &QTimer::timeout, this, &BattleScene::spawnRandomItem);
    // 每5-10秒随机掉落一个物品
    itemDropTimer->start(5000 + QRandomGenerator::global()->bounded(5000));

    addItem(new Platform(PlatformType::Soil, 0, 670, 26));
    addItem(new Platform(PlatformType::Soil, 50, 520, 8));
    addItem(new Platform(PlatformType::Grass, 820, 520, 8));
    addItem(new Platform(PlatformType::Ice, 350, 370, 11));
    addItem(new Platform(PlatformType::Soil, 820, 220, 8));
    addItem(new Platform(PlatformType::Grass, 50, 220, 8));

    leftBar = new HealthBar();
    leftBar->setPos(10, 10);
    rightBar = new HealthBar();
    rightBar->setPos(1070, 10);
    addItem(leftBar);
    addItem(rightBar);
    leftWeaponBar = new WeaponBar();
    leftWeaponBar->setPos(10, 40);
    rightWeaponBar = new WeaponBar();
    rightWeaponBar->setPos(1070, 40);
    addItem(leftWeaponBar);
    addItem(rightWeaponBar);
    
    // 游戏开始时不放置任何武器，武器会随机从空中掉落
}

void BattleScene::processInput() {
    Scene::processInput();
    
    // 如果游戏已结束，不再处理角色输入
    if (gameOver) {
        return;
    }
    
    if (character != nullptr) {
        character->processInput();
    }
    if (character2 != nullptr) {
        character2->processInput();
    }
    return;
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    // 如果游戏已结束，按R键可以重新开始游戏
    if (gameOver) {
        if (event->key() == Qt::Key_R) {
            // 重置游戏状态
            gameOver = false;
            winner = 0;
            gameOverText->setVisible(false);
            
            // 重置角色生命值和位置
            character->setLifeValue(character->getMaxLifeValue());
            character2->setLifeValue(character2->getMaxLifeValue());
            character->setPos(100, 650);
            character2->setPos(800, 650);
            
            // 移除武器
            character->pickupWeapon(nullptr);
            character2->pickupWeapon(nullptr);
            
            // 重新启动物品掉落计时器（如果已停止）
            if (!itemDropTimer->isActive()) {
                itemDropTimer->start(5000 + QRandomGenerator::global()->bounded(5000));
            }
        }
        return;
    }
    
    switch (event->key()) {
        case Qt::Key_A:
            if (character != nullptr) character->setLeftDown(true); break;
        case Qt::Key_D:
            if (character != nullptr) character->setRightDown(true); break;
        case Qt::Key_J:
            if (character != nullptr) character->setPickDown(true); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(true); break;
        case Qt::Key_K:
            if (character != nullptr) character->setAttackDown(true); break;
        case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(true); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(true); break;
        case Qt::Key_1:
            if (character2 != nullptr) character2->setPickDown(true); break;
        case Qt::Key_Up:
            if (character2 != nullptr) character2->setJumpDown(true); break;
        case Qt::Key_0:
            if (character2 != nullptr) character2->setAttackDown(true); break;
        default:
            Scene::keyPressEvent(event);
    }
    return;
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_A:
            if (character != nullptr) character->setLeftDown(false); break;
        case Qt::Key_D:
            if (character != nullptr) character->setRightDown(false); break;
        case Qt::Key_J:
            if (character != nullptr) character->setPickDown(false); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(false); break;
        case Qt::Key_K:
            if (character != nullptr) character->setAttackDown(false); break;
            case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(false); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(false); break;
        case Qt::Key_1:
            if (character2 != nullptr) character2->setPickDown(false); break;
        case Qt::Key_Up:
            if (character2 != nullptr) character2->setJumpDown(false); break;
        case Qt::Key_0:
            if (character2 != nullptr) character2->setAttackDown(false); break;
        default:
            Scene::keyReleaseEvent(event);
    }
    return;
}

void BattleScene::update() {
    Scene::update();
    leftBar->setLife(character->getLifeValue(), character->getMaxLifeValue());
    rightBar->setLife(character2->getLifeValue(), character2->getMaxLifeValue());
    leftWeaponBar->setWeaponPoints(character->getCurrentWeaponPoints(), character->getMaxWeaponPoints());
    rightWeaponBar->setWeaponPoints(character2->getCurrentWeaponPoints(), character2->getMaxWeaponPoints());
    
    // 检查游戏是否结束
    if (!gameOver) {
        checkGameOver();
    }
    return;
}

void BattleScene::processMovement() {
    Scene::processMovement();
    
    // 如果游戏已结束，不再处理角色移动
    if (gameOver) {
        return;
    }
    
    QRectF bounds = sceneRect();
    if (character != nullptr) {
        QPointF newPos = character->pos() + character->getVelocity() * (double)deltaTime;
        QSizeF pixSize = character->boundingRect().size();
        qreal halfW = pixSize.width() / 2;
        qreal fullH = pixSize.height();
        qreal leftLimit = bounds.left() + halfW;
        qreal rightLimit = bounds.left() + bounds.width() - halfW;
        newPos.setX(qBound(leftLimit, newPos.x(), rightLimit));
        newPos.setY(qBound(bounds.top() + fullH, newPos.y(), bounds.bottom()));
        character->setPos(newPos);
    }
    if (character2 != nullptr) {
        QPointF newPos2 = character2->pos() + character2->getVelocity() * (double)deltaTime;
        QSizeF pixSize2 = character2->boundingRect().size();
        qreal halfW2 = pixSize2.width() / 2;
        qreal fullH2 = pixSize2.height();
        qreal leftLimit2 = bounds.left() + halfW2;
        qreal rightLimit2 = bounds.left() + bounds.width() - halfW2;
        newPos2.setX(qBound(leftLimit2, newPos2.x(), rightLimit2));
        newPos2.setY(qBound(bounds.top() + fullH2, newPos2.y(), bounds.bottom()));
        character2->setPos(newPos2);
    }
    return;
}

void BattleScene::processPicking() {
    Scene::processPicking();
    
    // 如果游戏已结束，不再处理拾取操作
    if (gameOver) {
        return;
    }
    
    if (character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100);
        if (mountable != nullptr) {
           pickupMountable(character, mountable);
        }
    }
    return;
}

Mountable *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;

    for (QGraphicsItem *item: items()) {
        if (auto mountable = dynamic_cast<Mountable *>(item)) {
            if (!mountable->isMounted()) {
                qreal distance = QLineF(pos, item->pos()).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }

    return nearest;
}

void BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        character->pickupWeapon(weapon);
    }
    return;
}

// 实现随机物品掉落功能
void BattleScene::spawnRandomItem() {
    // 如果游戏已经结束，停止生成物品
    if (gameOver) {
        itemDropTimer->stop();
        return;
    }
    
    // 随机选择一种武器类型
    int weaponType = QRandomGenerator::global()->bounded(4); // 0-3之间的随机数
    Item* newItem = nullptr;
    
    switch (weaponType) {
        case 0:
            newItem = new Knife();
            break;
        case 1:
            newItem = new SolidBall();
            break;
        case 2:
            newItem = new Rifle();
            break;
        case 3:
            newItem = new Sniper();
            break;
    }
    
    if (!newItem) return;
    
    // 随机选择掉落位置（x坐标）
    qreal dropX = QRandomGenerator::global()->bounded(100, static_cast<int>(sceneRect().width() - 100));
    qreal dropY = 50; // 从接近顶部的位置掉落
    
    newItem->setPos(dropX, dropY);
    
    // 创建掉落控制器并开始掉落
    FallingItem* fallingItem = new FallingItem(newItem, this, this);
    fallingItem->startFalling();
    
    // 调整下一次生成物品的时间间隔（5-10秒）
    itemDropTimer->setInterval(5000 + QRandomGenerator::global()->bounded(5000));
}

// 实现检查游戏是否结束的函数
bool BattleScene::checkGameOver() {
    // 如果游戏已经结束，不再重复检查
    if (gameOver) {
        return true;
    }
    
    // 检查左侧角色(character)生命值
    if (character->getLifeValue() <= 0) {
        // 右侧角色(character2)胜利
        winner = 2;
        gameOver = true;
        gameOverText->setPlainText("右方玩家胜利！");
        gameOverText->setVisible(true);
        return true;
    }
    
    // 检查右侧角色(character2)生命值
    if (character2->getLifeValue() <= 0) {
        // 左侧角色(character)胜利
        winner = 1;
        gameOver = true;
        gameOverText->setPlainText("左方玩家胜利！");
        gameOverText->setVisible(true);
        return true;
    }
    
    // 游戏继续
    return false;
}
