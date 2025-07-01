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
    
    // Método para animación de entrada
    void iniciarAnimacionEntrada();
    bool estaEnAnimacionEntrada() const { return animacionEntradaActiva; }

protected:

private slots:
    void actualizarAnimacionEntrada();

private:
    QString ultimaDireccionHorizontal; // "adelante", "atras", o "" si no hay dirección horizontal
    
    // Variables para animación de entrada
    bool animacionEntradaActiva;
    int frameEntradaActual;
    QTimer* timerEntrada;

signals:
};

#endif // GOKU_H
