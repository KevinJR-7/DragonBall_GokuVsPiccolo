#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"

<<<<<<< HEAD
#include "ground.h"

class player : public QObject, public QGraphicsRectItem
=======
class player : public Personaje
>>>>>>> main
{
    Q_OBJECT
public:
    player(QObject *parent = nullptr);
    
    // Implementación obligatoria de métodos virtuales puros
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    void atacar() override;
    void recibirDanio(int danio) override;

protected:

private:
<<<<<<< HEAD
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
=======
>>>>>>> main

signals:
};

#endif // PLAYER_H
