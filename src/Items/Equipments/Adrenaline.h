#ifndef QT_PROGRAMMING_2024_ADRENALINE_H
#define QT_PROGRAMMING_2024_ADRENALINE_H

#include "Equipment.h"
#include <QTimer>

// 肾上腺素：拾取后在之后一段时间内增加移动速度并持续缓慢回血
class Adrenaline : public Equipment {
public:
    explicit Adrenaline(QGraphicsItem* parent = nullptr);
    ~Adrenaline() override;
    
    // 实现useBy方法：使角色在一段时间内增加移动速度并持续回血
    void useBy(Character* character) override;
    
private:
    // 检查效果是否应该结束
    static void checkEffectEnd(void* userData);
    
    // 为角色提供治疗
    static void healCharacter(void* userData);
    
private:
    static const int EFFECT_DURATION = 10000; // 效果持续时间，单位毫秒
    static const int HEAL_INTERVAL = 1000;    // 回血间隔，单位毫秒
    static const int HEAL_AMOUNT = 2;         // 每次回血量
    static const qreal SPEED_BOOST_FACTOR;    // 速度提升系数
    
    Character* affectedCharacter = nullptr;
    QTimer* effectTimer = nullptr;
    QTimer* healTimer = nullptr;
};

#endif // QT_PROGRAMMING_2024_ADRENALINE_H
