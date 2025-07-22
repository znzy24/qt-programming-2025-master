#include "HealthBar.h"
#include <QFont>

HealthBar::HealthBar(QGraphicsItem* parent): QGraphicsRectItem(parent){
    setRect(0, 0, 200, 25);
    setBrush(QBrush(Qt::red));
    setZValue(1000); 
    
    textItem = new QGraphicsTextItem(this);
    textItem->setPlainText("HP:100/100");
    textItem->setPos(50, 2); 
    
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::white);
}

void HealthBar::setLife(int life, int maxLife) {
    this->currentLife = qMax(0, life);
    this->maxLife = maxLife;
    
    qreal width = 200.0 * this->currentLife / this->maxLife;
    setRect(0, 0, width, 25);

    textItem->setPlainText(QString("HP:%1/%2").arg(this->currentLife).arg(this->maxLife));
}