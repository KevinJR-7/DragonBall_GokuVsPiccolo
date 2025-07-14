#ifndef PICCOLO_H
#define PICCOLO_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"
#include "rayo.h"
#include "gravityblast.h"
#include "kick.h"

// Declaración anticipada para evitar dependencia cíclica, ya que Goku necesita saber de Piccolo y viceversa
class Goku;

class Piccolo : public Personaje
{
    Q_OBJECT
public:
    // Constructor
    Piccolo(QObject *parent = nullptr);

    // Métodos para manejar la fase (transformación o estado de Piccolo)
    bool getFase() const { return fase;}
    void setFase(bool newFase) { fase = newFase;}
    void alternarFase();

    // Métodos para controlar el tipo de patada (alta o baja)
    bool getkickAlta() const { return kickAlta;}
    void setkickAlta(bool newkickAlta) { kickAlta = newkickAlta;}

    // Override de funciones de movimiento y acciones básicas del personaje
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    void atacar() override;
    void recibirDanio(int danio) override;
    void morir() override;

    // Override para el manejo de animaciones de idle y actualización general
    void iniciarAnimacionIdle() override;
    void actualizarAnimacion() override;

    // Override para funciones de manipulación de sprites
    void cambiarSprite(const QString& direccion) override;
    void cambiarSpriteCentrado(const QString& direccion) override;

    // Método para la animación de entrada del personaje
    void iniciarAnimacionEntrada() override;

    // Métodos para el ataque especial "Rayo"
    void iniciarCargaRayo();
    void detenerCargaRayo();
    void lanzarRayo();
    bool estaCargandoRayo() const { return animacionRayoActiva; }

    // Métodos para el ataque "Kick"
    void iniciarCargaKick();
    void detenerCargaKick();
    void lanzarKick();
    bool estaCargandoKick() const { return animacionKickActiva; }

    // Métodos para el ataque especial "Gravity Blast"
    void lanzarGravityBlast(Goku* gokuTarget);
    void iniciarCargaGravityBlast();
    void detenerCargaGravityBlast();
    bool estaCargandoGravityBlast() const { return animacionGravityBlastActiva; }

    // Método para establecer el objetivo de ciertos ataques (como Gravity Blast)
    void establecerObjetivo(Goku* objetivo);

private slots:
    // Slots para actualizar las animaciones de entrada y ataques especiales
    void actualizarAnimacionEntrada() override;
    void actualizarAnimacionRayo();
    void actualizarAnimacionKick();
    void actualizarAnimacionGravityBlast();

private:
    bool fase; // Indica la fase o transformación de Piccolo
    QTransform transform; // Para manipular la transformación (ej. girar) del sprite

    // Variables y temporizador para la animación del ataque "Rayo"
    bool animacionRayoActiva;
    int frameRayoActual;
    QTimer* timerRayo;

    // Variables y temporizador para la animación del ataque "Kick"
    bool animacionKickActiva;
    int frameKickActual;
    QTimer* timerKick;
    bool kickAlta; // Determina si la patada es alta

    // Variables y temporizador para la animación del ataque "Gravity Blast"
    bool animacionGravityBlastActiva;
    int frameGravityBlastActual;
    QTimer* timerGravityBlast;

    Goku* objetivoActual; // Puntero al objetivo de Piccolo para ataques dirigidos

signals:
    // Señal emitida cuando el ki de Piccolo cambia
    void kiCambiado(int kiActual, int kiMaximo) override;
};

#endif // PICCOLO_H
