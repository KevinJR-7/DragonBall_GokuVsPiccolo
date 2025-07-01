#include "game.h"
#include "ui_game.h"
#include "player.h"

game::game(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::game)
{
    ui->setupUi(this);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);

    view->setScene(scene);
    view->resize(1000, 500);
    view->scale(2.0, 2.0);
    scene->setSceneRect(200, 200, 1000, 500);

    p = new player();
    scene->addItem(p);
    p->setPos(300, 300);

    g = new ground();
    scene->addItem(g);
    g->setPos(300, 400);

    //p->moverPlayer();

}

game::~game()
{
    delete ui;
}

void game::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_D){ p->moverDerecha(); }
    if(e->key() == Qt::Key_A){ p->moverIzquierda(); }
    if(e->key() == Qt::Key_W){ p->saltar(); if(!(p->jumpTimer->isActive())){ p->jumpTimer->start(100); } }
    if(e->key() == Qt::Key_S){ p->setPos(p->pos().x(), 300); p->fallTimer->start(100);}
}
