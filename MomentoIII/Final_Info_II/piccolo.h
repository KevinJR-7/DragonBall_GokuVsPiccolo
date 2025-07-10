#ifndef PICCOLO_H
#define PICCOLO_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"
#include "rayo.h"
#include "gravityblast.h"

class Piccolo : public Personaje
{
    Q_OBJECT
public:
    Piccolo(QObject *parent = nullptr);

    bool getFase() const { return fase;}
    void setFase(bool newFase) { fase = newFase;}
    void alternarFase();

    // Override para funciones de movimiento
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    void atacar() override;
    void recibirDanio(int danio) override;

    // Override para limpiar dirección horizontal en idle
    void iniciarAnimacionIdle() override;
    void actualizarAnimacion() override;

    // Override para funciones de sprites
    void cambiarSprite(const QString& direccion) override;
    void cambiarSpriteCentrado(const QString& direccion) override;

    // Método para animación de entrada
    void iniciarAnimacionEntrada();
    bool estaEnAnimacionEntrada() const { return animacionEntradaActiva; }

    // Métodos para Rayo
    void iniciarCargaRayo();
    void detenerCargaRayo();
    void lanzarRayo(); // Nuevo método para lanzar el proyectil
    bool estaCargandoRayo() const { return animacionRayoActiva; }

    // Métodos para GBlast
    void lanzarGravityBlast(Goku* gokuTarget);
    void iniciarCargaGravityBlast();
    void detenerCargaGravityBlast();
    bool estaCargandoGravityBlast() const { return animacionGravityBlastActiva; }

    // Para que Piccolo sepa a quién atacar con Gravity Blast
    void establecerObjetivo(Goku* objetivo);

private slots:
    void actualizarAnimacionEntrada();
    void actualizarAnimacionRayo(); // Para la animación de Rayo
    void actualizarAnimacionGravityBlast(); // Nuevo slot para animación de Gravity Blast

private:
    bool fase;

    QString ultimaDireccionHorizontal; // "adelante", "atras", o "" si no hay dirección horizontal

    // Variable para girar en x
    QTransform transform;

    // Variables para animación de entrada
    bool animacionEntradaActiva;
    int frameEntradaActual;
    QTimer* timerEntrada;

    // Variables para animación de Rayo
    bool animacionRayoActiva;
    int frameRayoActual;
    QTimer* timerRayo;
    QPointF posicionInicialQuieto; // Guardar posición del sprite quieto // Posición exacta antes de empezar Rayo

    // Sistema de ki
    int kiActual;
    int kiMaximo;
    int velocidadRecargaKi; // ki por segundo

    // Variables para animación de Gravity Blast
    bool animacionGravityBlastActiva;
    int frameGravityBlastActual;
    QTimer* timerGravityBlast;

    Goku* objetivoActual; // Puntero al objetivo de Piccolo

signals:
    void kiCambiado(int kiActual, int kiMaximo);
};

#endif // PICCOLO_H
