#ifndef QT_PROGRAMMING_2024_HEALTHBAR_H
#define QT_PROGRAMMING_2024_HEALTHBAR_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>

class HealthBar : public QGraphicsRectItem {
public:
    HealthBar(QGraphicsItem* parent = nullptr);
    
    void setLife(int life, int maxLife = 100);

private:
    QGraphicsRectItem* backgroundRect;
    QGraphicsTextItem* textItem;
    int currentLife;
    int maxLife;
};

#endif
