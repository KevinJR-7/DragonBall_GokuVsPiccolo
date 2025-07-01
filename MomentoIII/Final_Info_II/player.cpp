#include <QBrush>
#include <QPixmap>
#include <QPainter>

#include "player.h"

player::player(QObject *parent)
    : QObject{parent}, QGraphicsRectItem(0, 0, 35, 50)
{
    setPen(Qt::NoPen); // oculta el borde del rectángulo
    QPixmap imagen(":/Goku/Sprites/goku/quieto.png");
    if (imagen.isNull()) {
        qDebug() << "No se pudo cargar la imagen";
    }

    //imagen = imagen.scaled(200, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    setBrush(QBrush(imagen));
    setFlag(QGraphicsItem::ItemIsMovable);

    yIn = 300;// this->pos().y();
    posY = yIn;
    posY_s = yIn;
    velIn = -20;
    velY = velIn;
    tiempo = 0.0;

    fallTimer = new QTimer(this);
    connect(fallTimer, SIGNAL(timeout()), this, SLOT(caer()));
    fallTimer->start(100);
    falling = true;

    jumpTimer = new QTimer(this);
    connect(jumpTimer, SIGNAL(timeout()), this, SLOT(saltar()));
    //jumpTimer->start(100);
    jumping = false;

    collisionTimer = new QTimer(this);
    connect(collisionTimer, SIGNAL(timeout()), this, SLOT(detectarColisiones()));
    collisionTimer->start(100);

    //MoveTimer = new QTimer;
    //MoveTimer->start(500);
    //connect(MoveTimer, SIGNAL(timeout()), this, SLOT(moverPlayer()));
}

void player::detectarColisiones() {
    QList<QGraphicsItem*> colisiones = collidingItems();

    for(QGraphicsItem* item : colisiones)
    {
        ground* g = dynamic_cast<ground*>(item);
        if(g)
        {
            if(fallTimer->isActive()){ fallTimer->stop(); }
            if(jumpTimer->isActive()){ jumpTimer->stop(); }
            falling = false;
            jumping = false;
            posY = this->pos().y();
            posY_s = 0;
            velY = 0;
            tiempo = 0;
            yIn = posY;
        }
    }
}

void player::moverDerecha()
{
    this->setPos(this->pos().x() + 5, this->pos().y());
}

void player::moverIzquierda()
{
    this->setPos(this->pos().x() - 5, this->pos().y());
}

void player::caer()
{
    posY = yIn + (-velIn * tiempo) + (0.5 * 9.8 * tiempo * tiempo);
    velY = -velIn + 9.8 * tiempo;
    //qDebug() << "posY:" << posY;

    setPos(this->pos().x(), posY);
    tiempo += 0.1;

}

void player::saltar()
{
    posY_s = yIn + (velIn * tiempo) + (0.5 * 20 * tiempo * tiempo);
    velY = velIn + 20 * tiempo;
    setPos(this->pos().x(), posY_s);
    if(!(fallTimer->isActive())){ fallTimer->start(100); }
    tiempo += 0.1;

    // if (posY>viewRect.height()-30||posY<0){
    //     velIn = sqrt(velX * velX + velY * velY)*0.8;



    //     theta = atan2(velY,velX);
    //     qDebug()<<"Choque en Y"<<atan2(velY,velX)<<cos(theta);
    //     velIn = 0.8*velIn;
    //     tiempo = 0;
    //     if(posY<0){
    //         dirY = -1;}
    //     else{dirY = 1;}
    //     yIn = posY-10*dirY;

    //     xIn = posX;

    //     //if (velIn<5){timerMovPar->stop();}
    // }
}
