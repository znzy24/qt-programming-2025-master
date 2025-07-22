#ifndef QT_PROGRAMMING_2024_WEAPONBAR_H
#define QT_PROGRAMMING_2024_WEAPONBAR_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>

class WeaponBar : public QGraphicsRectItem {
public:
    WeaponBar(QGraphicsItem* parent = nullptr);
    
    void setWeaponPoints(int points, int maxPoints);

private:
    QGraphicsRectItem* backgroundRect;
    QGraphicsTextItem* textItem;
    int currentWeaponPoints;
    int maxWeaponPoints;
};

#endif