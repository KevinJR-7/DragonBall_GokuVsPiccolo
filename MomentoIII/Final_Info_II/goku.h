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
    Goku(QObject *parent = nullptr);
    
    // Override para funciones de movimiento
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
    
    // Métodos para recarga de ki
    void iniciarRecargaKi();
    void detenerRecargaKi();
    bool estaRecargandoKi() const { return animacionKiActiva; }
    
    // Métodos para Kamehameha
    void iniciarCargaKamehameha();
    void detenerCargaKamehameha();
    void lanzarKamehameha(); // Nuevo método para lanzar el proyectil
    bool estaCargandoKamehameha() const { return animacionKamehamehaActiva; }

    // Método para cambiar sprite de Kamehameha manteniendo posición fija
    void cambiarSpriteKamehamehaFijo(const QString& direccion);

    // Métodos para ráfaga
    void iniciarAnimacionRafaga();
    void detenerAnimacionRafaga();
    bool estaEnAnimacionRafaga() const { return animacionRafagaActiva; }

    // Método para lanzar BlastB
    void lanzarBlastB();
    
    // Métodos para sistema de ki
    void establecerKi(int ki, int kiMax = 100);
    int obtenerKi() const { return kiActual; }
    int obtenerKiMaximo() const { return kiMaximo; }
    float obtenerPorcentajeKi() const { return (float)kiActual / kiMaximo * 100.0f; }

    //Metodos para morir
    void morir() override;

protected:

private slots:
    void actualizarAnimacionEntrada();
    void actualizarAnimacionKi();
    void recargarKi(); // Para incrementar el ki real
    void actualizarAnimacionKamehameha(); // Para la animación de Kamehameha
    void actualizarAnimacionRafaga(); // Para la animación de ráfaga

private:
    QString ultimaDireccionHorizontal; // "adelante", "atras", o "" si no hay dirección horizontal
    
    // Variables para animación de entrada
    bool animacionEntradaActiva;
    int frameEntradaActual;
    QTimer* timerEntrada;
    
    // Variables para animación de recarga de ki
    bool animacionKiActiva;
    int frameKiActual;
    QTimer* timerKi;
    QTimer* timerRecargaKi; // Timer para la recarga real de ki
    QPointF posicionOriginalKi; // Posición exacta antes de empezar ki
    
    // Variables para animación de Kamehameha
    bool animacionKamehamehaActiva;
    int frameKamehamehaActual;
    QTimer* timerKamehameha;
    QPointF posicionInicialQuieto; // Guardar posición del sprite quieto // Posición exacta antes de empezar Kamehameha
    QPointF posicionFijaKamehameha; // Posición fija para mantener durante la animación
    
    // Variables para animación de ráfaga
    bool animacionRafagaActiva;
    int frameRafagaActual;
    QTimer* timerRafaga;
    
    // Estados de animación de ki
    enum EstadoKi { INICIO, BUCLE, FINAL };
    EstadoKi estadoKiActual;
    int frameDentroEstado;
    
    // Sistema de ki
    int kiActual;
    int kiMaximo;
    int velocidadRecargaKi; // ki por segundo
    
    // Offsets para sprites de ki (para alinearlos con el sprite quieto)
    qreal offsetKiX;
    qreal offsetKiY;

    // Timer para la muerte
    int frameMuerteActual = 1;
    QTimer* timerMuerte = nullptr;


signals:
    void kiCambiado(int kiActual, int kiMaximo);
};

#endif // GOKU_H
