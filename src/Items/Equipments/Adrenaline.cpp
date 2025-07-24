#include "Adrenaline.h"
#include "../Characters/Character.h"
#include <QGraphicsScene>
#include <QGraphicsEffect>
#include <QTimer>

const qreal Adrenaline::SPEED_BOOST_FACTOR = 1.5;

Adrenaline::Adrenaline(QGraphicsItem* parent)
    : Equipment(parent, ":/Items/Equipments/Adrenaline.png")
{
    // 设置适当的大小
    if (pixmapItem) {
        QPixmap original = pixmapItem->pixmap();
        QPixmap scaled = original.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapItem->setPixmap(scaled);
        pixmapItem->setOffset(-scaled.width() / 2, -scaled.height() / 2);
    }
    
    // 计时器将在useBy方法中创建
    effectTimer = nullptr;
    healTimer = nullptr;
}

Adrenaline::~Adrenaline()
{
    // 如果效果还在持续，恢复角色状态
    if (affectedCharacter) {
        // 取消移动速度加成
        affectedCharacter->setSpeedMultiplier(1.0);
    }
    
    // 停止计时器
    if (effectTimer) {
        effectTimer->stop();
        delete effectTimer;
    }
    
    if (healTimer) {
        healTimer->stop();
        delete healTimer;
    }
}

void Adrenaline::useBy(Character* character)
{
    if (!character) return;
    
    // 保存受影响的角色指针
    affectedCharacter = character;
    
    // 增加移动速度
    character->setSpeedMultiplier(SPEED_BOOST_FACTOR);
    
    // 创建并启动效果计时器
    effectTimer = new QTimer();
    effectTimer->setSingleShot(true);
    QObject::connect(effectTimer, &QTimer::timeout, [this]() {
        checkEffectEnd(this);
    });
    effectTimer->start(EFFECT_DURATION);
    
    // 创建并启动回血计时器
    healTimer = new QTimer();
    QObject::connect(healTimer, &QTimer::timeout, [this]() {
        healCharacter(this);
    });
    healTimer->start(HEAL_INTERVAL);
    
    // 从场景中移除物品但保留对象
    if (scene()) {
        scene()->removeItem(this);
        // 不要在这里删除对象，隐藏即可，等效果结束后再删除
        hide();
    }
}

void Adrenaline::checkEffectEnd(void* userData)
{
    Adrenaline* self = static_cast<Adrenaline*>(userData);
    if (!self) return;
    
    if (self->affectedCharacter) {
        // 恢复正常速度
        self->affectedCharacter->setSpeedMultiplier(1.0);
    }
    
    // 停止回血计时器
    if (self->healTimer) {
        self->healTimer->stop();
        delete self->healTimer;
        self->healTimer = nullptr;
    }
    
    // 效果结束，清理
    self->affectedCharacter = nullptr;
    delete self; // 直接删除自身对象
}

void Adrenaline::healCharacter(void* userData)
{
    Adrenaline* self = static_cast<Adrenaline*>(userData);
    if (!self || !self->affectedCharacter) {
        if (self && self->healTimer) {
            self->healTimer->stop();
        }
        return;
    }
    
    // 每个间隔恢复少量血量
    int currentHealth = self->affectedCharacter->getLifeValue();
    int maxHealth = self->affectedCharacter->getMaxLifeValue();
    
    // 如果血量已满，不再增加
    if (currentHealth >= maxHealth) {
        return;
    }
    
    // 增加血量
    int newHealth = qMin(currentHealth + HEAL_AMOUNT, maxHealth);
    self->affectedCharacter->setLifeValue(newHealth);
}
