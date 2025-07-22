#include "WeaponBar.h"
#include <QFont>

WeaponBar::WeaponBar(QGraphicsItem* parent) : QGraphicsRectItem(parent) {
    setRect(0, 0, 200, 25);
    setBrush(QBrush(Qt::blue));
    setZValue(1000); 
    
    textItem = new QGraphicsTextItem(this);
    textItem->setPlainText("WP:1/1");
    textItem->setPos(50, 2); 
    
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::white);
}

void WeaponBar::setWeaponPoints(int points, int maxPoints) {
    currentWeaponPoints = qMax(0, points);
    maxWeaponPoints = qMax(1, maxPoints);
    qreal width = 200.0 * currentWeaponPoints / maxWeaponPoints;
    setRect(0, 0, width, 25);
    
    textItem->setPlainText(QString("WP:%1/%2").arg(currentWeaponPoints).arg(maxWeaponPoints));
}