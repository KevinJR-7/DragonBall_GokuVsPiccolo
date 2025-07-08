#ifndef PICCOLO_H
#define PICCOLO_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"

class Piccolo : public Personaje
{
    Q_OBJECT
public:
    Piccolo(QObject *parent = nullptr);

    // Override para funciones de movimiento
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    // void atacar() override;
    // void recibirDanio(int danio) override;

    // Override para limpiar dirección horizontal en idle
    void iniciarAnimacionIdle() override;
    void actualizarAnimacion() override;

    // Override para funciones de sprites
    void cambiarSprite(const QString& direccion) override;

    // Método para animación de entrada
    void iniciarAnimacionEntrada();

private slots:
    void actualizarAnimacionEntrada();

private:
    QString ultimaDireccionHorizontal; // "adelante", "atras", o "" si no hay dirección horizontal

    // Variable para girar en x
    QTransform transform;

    // Variables para animación de entrada
    bool animacionEntradaActiva;
    int frameEntradaActual;
    QTimer* timerEntrada;


signals:
};

#endif // PICCOLO_H
