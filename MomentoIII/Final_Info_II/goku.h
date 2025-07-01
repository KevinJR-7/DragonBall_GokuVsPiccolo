#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"

class Goku : public Personaje
{
    Q_OBJECT
public:
    Goku(QObject *parent = nullptr);
    
    // Implementación obligatoria de métodos virtuales puros
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    void atacar() override;
    void recibirDanio(int danio) override;
    
    // Override para limpiar dirección horizontal en idle
    void iniciarAnimacionIdle() override;

protected:

private:
    QString ultimaDireccionHorizontal; // "adelante", "atras", o "" si no hay dirección horizontal

signals:
};

#endif // GOKU_H
