#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"

class player : public Personaje
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

signals:
};

#endif // PLAYER_H
