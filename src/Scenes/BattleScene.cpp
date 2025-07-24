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
    character2 = new Character(nullptr);
    addItem(map);
    addItem(character);
    addItem(character2);
    character->setPos(100, 650);
    character2->setPos(1100, 650);
    
    // 初始化游戏结束文本，但不显示
    gameOverText = new QGraphicsTextItem();
    gameOverText->setDefaultTextColor(Qt::red);
    QFont gameOverFont;
    gameOverFont.setPointSize(36);
    gameOverFont.setBold(true);
    gameOverText->setFont(gameOverFont);
    gameOverText->setZValue(1001); // 确保显示在最上层
    gameOverText->setPos(sceneRect().width() / 2 - 200, sceneRect().height() / 2 - 50); // 居中显示
    gameOverText->setVisible(false);
    addItem(gameOverText);
    
    // 初始化物品掉落计时器
    itemDropTimer = new QTimer(this);
    connect(itemDropTimer, &QTimer::timeout, this, &BattleScene::spawnRandomItem);
    // 每5-10秒随机掉落一个物品
    itemDropTimer->start(5000 + QRandomGenerator::global()->bounded(5000));
    
    // 初始化物品自动清除计时器
    itemCleanupTimer = new QTimer(this);
    connect(itemCleanupTimer, &QTimer::timeout, this, &BattleScene::cleanupOldItems);
    // 每5秒检查一次是否有需要清除的物品
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
            character2->setPos(1100, 650);
            
            // 移除武器
            character->pickupWeapon(nullptr);
            character2->pickupWeapon(nullptr);
            
            // 清理场景中的所有掉落物品
            for (const DroppedItemInfo& info : droppedItems) {
                if (info.item && !info.isPicked) {
                    // 获取物品对应的FallingItem控制器
                    FallingItem* controller = FallingItem::findControllerForItem(info.item);
                    if (controller) {
                        delete controller; // 这会移除控制器并从场景中移除物品
                    } else if (info.item->scene() == this) {
                        // 如果没有控制器但物品仍在场景中
                        removeItem(info.item);
                        delete info.item;
                    }
                }
            }
            
            // 清空掉落物品列表
            droppedItems.clear();
            
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
    
    // 处理玩家1的拾取操作
    if (character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100);
        if (mountable != nullptr) {
           pickupMountable(character, mountable);
        }
    }
    
    // 处理玩家2的拾取操作
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
    // 获取物品的图形效果，如果有淡出效果，则取消它
    Item* item = dynamic_cast<Item*>(mountable);
    if (item) {
        // 移除任何可能的淡出效果
        item->setGraphicsEffect(nullptr);
        
        // 标记物品已被拾取（用于自动清理）
        for (auto& info : droppedItems) {
            if (info.item == item) {
                info.isPicked = true;
                break;
            }
        }
    }
    
    // 处理武器拾取
    if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        character->pickupWeapon(weapon);
        return;
    }
    
    // 处理装备使用
    if (auto equipment = dynamic_cast<Equipment *>(mountable)) {
        equipment->useBy(character);
        return;
    }
}

// 实现随机物品掉落功能
void BattleScene::spawnRandomItem() {
    // 如果游戏已经结束，停止生成物品
    if (gameOver) {
        itemDropTimer->stop();
        return;
    }
    
    // 定义物品权重表
    // 小刀20，实心球15，步枪10，狙击枪5，绷带30，肾上腺素15，医疗箱5
    struct ItemInfo {
        int weight;            // 权重值
        std::function<Item*()> creator;  // 创建物品的函数
    };
    
    std::vector<ItemInfo> itemWeights = {
        {20, []() { return new Knife(); }},         // 小刀
        {15, []() { return new SolidBall(); }},     // 实心球
        {10, []() { return new Rifle(); }},         // 步枪
        {5,  []() { return new Sniper(); }},        // 狙击枪
        {30, []() { return new Bandage(); }},       // 绷带
        {15, []() { return new Adrenaline(); }},    // 肾上腺素
        {5,  []() { return new MedKit(); }}         // 医疗箱
    };
    
    // 计算总权重
    int totalWeight = 0;
    for (const auto& item : itemWeights) {
        totalWeight += item.weight;
    }
    
    // 生成一个随机值，范围为[0, totalWeight)
    int randomValue = QRandomGenerator::global()->bounded(totalWeight);
    
    // 根据权重选择物品
    Item* newItem = nullptr;
    int accumulatedWeight = 0;
    
    for (const auto& item : itemWeights) {
        accumulatedWeight += item.weight;
        if (randomValue < accumulatedWeight) {
            newItem = item.creator();
            break;
        }
    }
    
    // 如果出现错误，默认创建一个绷带
    if (!newItem) {
        newItem = new Bandage();
    }
    
    if (!newItem) return;
    
    // 随机选择掉落位置（x坐标）
    qreal dropX = QRandomGenerator::global()->bounded(100, static_cast<int>(sceneRect().width() - 100));
    qreal dropY = 50; // 从接近顶部的位置掉落
    
    newItem->setPos(dropX, dropY);
    
    // 创建掉落控制器并开始掉落
    FallingItem* fallingItem = new FallingItem(newItem, this, this);
    fallingItem->startFalling();
    
    // 记录新掉落的物品及其掉落时间
    DroppedItemInfo itemInfo;
    itemInfo.item = newItem;
    itemInfo.dropTime = QDateTime::currentMSecsSinceEpoch();
    itemInfo.isPicked = false;
    droppedItems.append(itemInfo);
    
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
        gameOverText->setPlainText("恭喜玩家2胜利！\n按R键重新开始");
        gameOverText->setVisible(true);
        return true;
    }
    
    // 检查右侧角色(character2)生命值
    if (character2->getLifeValue() <= 0) {
        // 左侧角色(character)胜利
        winner = 1;
        gameOver = true;
        gameOverText->setPlainText("恭喜玩家1胜利！\n按R键重新开始");
        gameOverText->setVisible(true);
        return true;
    }
    
    // 游戏继续
    return false;
}

// 清理超时未被拾取的物品
void BattleScene::cleanupOldItems() {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    const qint64 timeoutMs = 30000; // 30秒超时
    
    // 遍历所有记录的掉落物品
    for (int i = droppedItems.size() - 1; i >= 0; i--) {
        auto& itemInfo = droppedItems[i];
        
        // 检查是否已被拾取或者不存在
        if (itemInfo.isPicked || !itemInfo.item || !itemInfo.item->scene()) {
            droppedItems.removeAt(i);
            continue;
        }
        
        // 检查是否超时
        if (currentTime - itemInfo.dropTime > timeoutMs) {
            // 添加淡出效果
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
            effect->setOpacity(1.0);
            itemInfo.item->setGraphicsEffect(effect);
            
            // 创建淡出动画
            QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(1000); // 1秒钟淡出
            animation->setStartValue(1.0);
            animation->setEndValue(0.0);
            
            // 在动画完成后删除物品
            Item* itemToRemove = itemInfo.item;
            QObject::connect(animation, &QPropertyAnimation::finished, [this, itemToRemove, animation]() {
                if (itemToRemove && itemToRemove->scene()) {
                    removeItem(itemToRemove);
                    delete itemToRemove;
                }
                animation->deleteLater();
            });
            
            // 开始动画
            animation->start(QAbstractAnimation::DeleteWhenStopped);
            
            // 从列表中移除
            droppedItems.removeAt(i);
        }
    }
}
