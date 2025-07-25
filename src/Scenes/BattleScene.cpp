#include <QDebug>
#include <QRandomGenerator>
#include <functional>
#include <vector>
#include "BattleScene.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Maps/Platform.h"
#include "../Items/Weapons/SolidBall.h"
#include "../Items/Weapons/Rifle.h"
#include "../Items/Weapons/Sniper.h"
#include "../Items/FallingItem.h"
#include "../Items/Weapons/Knife.h"
#include "../Items/Equipments/Bandage.h"
#include "../Items/Equipments/MedKit.h"
#include "../Items/Equipments/Adrenaline.h"

BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);
    map = new Battlefield();
    character = new Character(nullptr);
    character2 = new Character(nullptr, true);  // 使用替代精灵图
    addItem(map);
    addItem(character);
    addItem(character2);
    character->setPos(100, 650);
    character2->setPos(1100, 650);
    
    gameOverText = new QGraphicsTextItem();
    gameOverText->setDefaultTextColor(Qt::red);
    QFont gameOverFont;
    gameOverFont.setPointSize(36);
    gameOverFont.setBold(true);
    gameOverText->setFont(gameOverFont);
    gameOverText->setZValue(1001); 
    gameOverText->setPos(sceneRect().width() / 2 - 200, sceneRect().height() / 2 - 50); // 居中显示
    gameOverText->setVisible(false);
    addItem(gameOverText);
    
    itemDropTimer = new QTimer(this);
    connect(itemDropTimer, &QTimer::timeout, this, &BattleScene::spawnRandomItem);
    itemDropTimer->start(5000 + QRandomGenerator::global()->bounded(5000));
    
    itemCleanupTimer = new QTimer(this);
    connect(itemCleanupTimer, &QTimer::timeout, this, &BattleScene::cleanupOldItems);
    itemCleanupTimer->start(5000);

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
    
}

void BattleScene::processInput() {
    Scene::processInput();
    
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
    if (gameOver) {
        if (event->key() == Qt::Key_R) {
            gameOver = false;
            winner = 0;
            gameOverText->setVisible(false);
            
            character->setLifeValue(character->getMaxLifeValue());
            character2->setLifeValue(character2->getMaxLifeValue());
            character->setPos(100, 650);
            character2->setPos(1100, 650);
            
            character->pickupWeapon(nullptr);
            character2->pickupWeapon(nullptr);
            
            for (const DroppedItemInfo& info : droppedItems) {
                if (info.item && !info.isPicked) {
                    FallingItem* controller = FallingItem::findControllerForItem(info.item);
                    if (controller) {
                        delete controller;
                    } else if (info.item->scene() == this) {
                        removeItem(info.item);
                        delete info.item;
                    }
                }
            }
            
            droppedItems.clear();
            
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
        case Qt::Key_S:
            if (character != nullptr) character->setPickDown(true); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(true); break;
        case Qt::Key_F:
            if (character != nullptr) character->setAttackDown(true); break;
        case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(true); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(true); break;
        case Qt::Key_Down:
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
        case Qt::Key_S:
            if (character != nullptr) character->setPickDown(false); break;
        case Qt::Key_W:
            if (character != nullptr) character->setJumpDown(false); break;
        case Qt::Key_F:
            if (character != nullptr) character->setAttackDown(false); break;
            case Qt::Key_Left:
            if (character2 != nullptr) character2->setLeftDown(false); break;
        case Qt::Key_Right:
            if (character2 != nullptr) character2->setRightDown(false); break;
        case Qt::Key_Down:
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
    
    if (!gameOver) {
        checkGameOver();
    }
    return;
}

void BattleScene::processMovement() {
    Scene::processMovement();
    
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
    
    if (gameOver) {
        return;
    }
    
    if (character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100);
        if (mountable != nullptr) {
           pickupMountable(character, mountable);
        }
    }
    
    if (character2->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character2->pos(), 100);
        if (mountable != nullptr) {
           pickupMountable(character2, mountable);
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
    Item* item = dynamic_cast<Item*>(mountable);
    if (item) {
        item->setGraphicsEffect(nullptr);
        
        for (auto& info : droppedItems) {
            if (info.item == item) {
                info.isPicked = true;
                break;
            }
        }
    }
    
    if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        character->pickupWeapon(weapon);
        return;
    }
    
    if (auto equipment = dynamic_cast<Equipment *>(mountable)) {
        equipment->useBy(character);
        return;
    }
}

void BattleScene::spawnRandomItem() {
    if (gameOver) {
        itemDropTimer->stop();
        return;
    }
    
 
    struct ItemInfo {
        int weight;           
        std::function<Item*()> creator;  
    };
    
    std::vector<ItemInfo> itemWeights = {
        {20, []() { return new Knife(); }},       
        {15, []() { return new SolidBall(); }},   
        {10, []() { return new Rifle(); }},        
        {5,  []() { return new Sniper(); }},       
        {30, []() { return new Bandage(); }},      
        {15, []() { return new Adrenaline(); }},   
        {5,  []() { return new MedKit(); }}         
    };
    
    int totalWeight = 0;
    for (const auto& item : itemWeights) {
        totalWeight += item.weight;
    }
    
    int randomValue = QRandomGenerator::global()->bounded(totalWeight);
    
    Item* newItem = nullptr;
    int accumulatedWeight = 0;
    
    for (const auto& item : itemWeights) {
        accumulatedWeight += item.weight;
        if (randomValue < accumulatedWeight) {
            newItem = item.creator();
            break;
        }
    }
    
    if (!newItem) {
        newItem = new Bandage();
    }
    
    if (!newItem) return;
    
    qreal dropX = QRandomGenerator::global()->bounded(100, static_cast<int>(sceneRect().width() - 100));
    qreal dropY = 50; 
    
    newItem->setPos(dropX, dropY);
    
    FallingItem* fallingItem = new FallingItem(newItem, this, this);
    fallingItem->startFalling();
    
    DroppedItemInfo itemInfo;
    itemInfo.item = newItem;
    itemInfo.dropTime = QDateTime::currentMSecsSinceEpoch();
    itemInfo.isPicked = false;
    droppedItems.append(itemInfo);
    
    itemDropTimer->setInterval(5000 + QRandomGenerator::global()->bounded(5000));
}

bool BattleScene::checkGameOver() {
    if (gameOver) {
        return true;
    }
    
    if (character->getLifeValue() <= 0) {
        winner = 2;
        gameOver = true;
        gameOverText->setPlainText("恭喜玩家2胜利！\n按R键重新开始");
        gameOverText->setVisible(true);
        return true;
    }
    
    if (character2->getLifeValue() <= 0) {
        winner = 1;
        gameOver = true;
        gameOverText->setPlainText("恭喜玩家1胜利！\n按R键重新开始");
        gameOverText->setVisible(true);
        return true;
    }
    
    return false;
}

void BattleScene::cleanupOldItems() {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    const qint64 timeoutMs = 30000; 
    
    for (int i = droppedItems.size() - 1; i >= 0; i--) {
        auto& itemInfo = droppedItems[i];
        
        if (itemInfo.isPicked || !itemInfo.item || !itemInfo.item->scene()) {
            droppedItems.removeAt(i);
            continue;
        }
        
        if (currentTime - itemInfo.dropTime > timeoutMs) {
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
            effect->setOpacity(1.0);
            itemInfo.item->setGraphicsEffect(effect);
            
            QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(1000); 
            animation->setStartValue(1.0);
            animation->setEndValue(0.0);
            
            Item* itemToRemove = itemInfo.item;
            QObject::connect(animation, &QPropertyAnimation::finished, [this, itemToRemove, animation]() {
                if (itemToRemove && itemToRemove->scene()) {
                    removeItem(itemToRemove);
                    delete itemToRemove;
                }
                animation->deleteLater();
            });

            animation->start(QAbstractAnimation::DeleteWhenStopped);
            
            droppedItems.removeAt(i);
        }
    }
}
