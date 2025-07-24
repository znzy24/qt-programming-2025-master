#ifndef QT_PROGRAMMING_2024_ITEM_H
#define QT_PROGRAMMING_2024_ITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsEffect>

class Item : public QGraphicsItem {
public:
    explicit Item(QGraphicsItem *parent, const QString &pixmapPath);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // 为派生类提供设置图形效果的方法
    void setGraphicsEffect(QGraphicsEffect* effect);

protected:
    QGraphicsPixmapItem *pixmapItem;
};


#endif
