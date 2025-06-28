#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>

class player : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    player(QObject *parent = nullptr);
    void moverDerecha();
    void moverIzquierda();
    void saltar();

protected:

private:
    qreal posX,posY,velIn,theta,dirX,dirY,xIn,yIn,tiempo,velY,velX;

    QPixmap pixmap;
    QTimer *jumpTimer;
    //QTimer *MoveTimer;

private slots:
    //void moverPlayer();

signals:
};

#endif // PLAYER_H
