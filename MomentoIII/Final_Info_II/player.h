#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>

#include "ground.h"

class player : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    player(QObject *parent = nullptr);
    void moverDerecha();
    void moverIzquierda();
    QTimer *jumpTimer;
    QTimer *fallTimer;
    QTimer *collisionTimer;
    //void saltar();

protected:

private:
    bool falling;
    bool jumping;
    qreal posX,posY,velIn,theta,dirX,dirY,xIn,yIn,tiempo,velY,velX;

    QPixmap pixmap;
    //QTimer *MoveTimer;

public slots:
    void saltar();

private slots:
    //void moverPlayer();
    void caer();
    void detectarColisiones();

signals:
};

#endif // PLAYER_H
