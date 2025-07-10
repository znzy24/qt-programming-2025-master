//
// Created by gerw on 8/20/24.
//
#include "MyGame.h"
#include "Scenes/BattleScene.h"

MyGame::MyGame(QWidget *parent) : QMainWindow(parent) {
    battleScene = new BattleScene(this);
    view = new QGraphicsView(this);
    view->setScene(battleScene);
    view->setSceneRect(0, 0, 1280, 720);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    view->setFixedSize(view->sceneRect().width(), view->sceneRect().height());
    setCentralWidget(view);
    setFixedSize(view->size());
    battleScene->startLoop();
}
