#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"
#include "kamehameha.h"
#include "blastb.h"

class Goku : public Personaje
{
    Q_OBJECT
public:
    // Constructor
    Goku(QObject *parent = nullptr);

    // Override de funciones de movimiento
    void moverDerecha() override;
    void moverIzquierda() override;
    void moverArriba() override;
    void moverAbajo() override;
    void recibirDanio(int danio) override;

    // Override para iniciar la animación de idle
    void iniciarAnimacionIdle() override;

    // Método para iniciar la animación de entrada
    void iniciarAnimacionEntrada() override;

    // Métodos para la recarga de ki
    void iniciarRecargaKi();
    void detenerRecargaKi();
    bool estaRecargandoKi() const { return animacionKiActiva; }

    // Métodos para el ataque "Kamehameha"
    void iniciarCargaKamehameha();
    void detenerCargaKamehameha();
    void lanzarKamehameha();
    bool estaCargandoKamehameha() const { return animacionKamehamehaActiva; }

    // Método para cambiar el sprite del Kamehameha manteniendo la posición fija
    void cambiarSpriteKamehamehaFijo(const QString& direccion);

    // Métodos para la animación de ráfaga
    void iniciarAnimacionRafaga();
    void detenerAnimacionRafaga();
    bool estaEnAnimacionRafaga() const { return animacionRafagaActiva; }

    // Método para lanzar el proyectil "BlastB"
    void lanzarBlastB();

    // Métodos para el sistema de ki
    void establecerKi(int ki, int kiMax = 100);
    int obtenerKi() const { return kiActual; }
    int obtenerKiMaximo() const { return kiMaximo; }
    float obtenerPorcentajeKi() const { return (float)kiActual / kiMaximo * 100.0f; }

    // Override para el método de morir
    void morir() override;

    // Método para iniciar la animación de teletransportación
    void iniciarAnimacionTeleport();

    // Ataques cuerpo a cuerpo
    void golpear();
    void patear();
    void animarGolpe();
    void animarPatada();

    // Método para teletransportación
    void tp();

protected:

private slots:
    // Slots para actualizar diversas animaciones y lógicas
    void actualizarAnimacionEntrada() override;
    void actualizarAnimacionKi();
    void recargarKi(); // Incrementa el ki real
    void actualizarAnimacionKamehameha(); // Actualiza la animación del Kamehameha
    void actualizarAnimacionRafaga(); // Actualiza la animación de ráfaga

private:
    // Variables para la animación de recarga de ki
    bool animacionKiActiva;
    int frameKiActual;
    QTimer* timerKi; // Temporizador para la animación visual del ki
    QTimer* timerRecargaKi; // Temporizador para la recarga gradual del ki
    QPointF posicionOriginalKi; // Posición guardada al iniciar la recarga de ki

    // Variables para la animación de Kamehameha
    bool animacionKamehamehaActiva;
    int frameKamehamehaActual;
    QTimer* timerKamehameha; // Temporizador para la animación del Kamehameha
    QPointF posicionFijaKamehameha; // Posición para mantener fijo el personaje durante el Kamehameha

    // Variables para la animación de ráfaga
    bool animacionRafagaActiva;
    int frameRafagaActual;
    QTimer* timerRafaga; // Temporizador para la animación de ráfaga

    // Estados de la animación de recarga de ki
    enum EstadoKi { INICIO, BUCLE, FINAL };
    EstadoKi estadoKiActual; // Estado actual de la animación de ki
    int frameDentroEstado; // Frame actual dentro del estado de ki

    // Sistema de ki
    int velocidadRecargaKi; // Cantidad de ki recargada por segundo

    // Offsets para alinear los sprites de ki con el personaje
    qreal offsetKiX;
    qreal offsetKiY;

    // Timer y frame para la animación de muerte
    int frameMuerteActual = 1;
    QTimer* timerMuerte;

    // Variables y temporizadores para las animaciones de golpe y patada
    int frameGolpe = 0;
    int framePatada = 0;
    QTimer *timerGolpe = nullptr;
    QTimer *timerPatada = nullptr;

    // Variables y temporizador para la animación de teletransportación
    void animarTp(); // Método privado para la lógica de la animación de TP
    int frameTeleport = 1;
    QTimer *timerTeleport = nullptr;
    bool animacionTeleportActiva = false;

signals:
    // Señal emitida cuando el ki de Goku cambia
    void kiCambiado(int kiActual, int kiMaximo) override;
};

#endif // GOKU_H
