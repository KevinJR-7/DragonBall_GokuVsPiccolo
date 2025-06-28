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

    // yIn = 300;
    // posY = yIn;
    // velIn = 10;
    // velY = velIn;
    // tiempo = 0;

    // jumpTimer = new QTimer;
    // jumpTimer->start(100);
    // connect(jumpTimer, SIGNAL(timeout()), this, SLOT(saltar()));

    //MoveTimer = new QTimer;
    //MoveTimer->start(500);
    //connect(MoveTimer, SIGNAL(timeout()), this, SLOT(moverPlayer()));
}

void player::moverDerecha()
{
    this->setPos(this->pos().x() + 5, this->pos().y());
}

void player::moverIzquierda()
{
    this->setPos(this->pos().x() - 5, this->pos().y());
}

// void player::saltar()
// {
//     posY = yIn + (-velIn * tiempo) + (0.5 * 9.8 * tiempo * tiempo);
//     velY = -velIn + 9.8 * tiempo;
//     setPos(this->pos().x(), posY);
//     tiempo += 0.1;

//     // if (posY>viewRect.height()-30||posY<0){
//     //     velIn = sqrt(velX * velX + velY * velY)*0.8;



//     //     theta = atan2(velY,velX);
//     //     qDebug()<<"Choque en Y"<<atan2(velY,velX)<<cos(theta);
//     //     velIn = 0.8*velIn;
//     //     tiempo = 0;
//     //     if(posY<0){
//     //         dirY = -1;}
//     //     else{dirY = 1;}
//     //     yIn = posY-10*dirY;

//     //     xIn = posX;

//     //     //if (velIn<5){timerMovPar->stop();}
//     // }
// }
