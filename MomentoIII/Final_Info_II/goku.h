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
    
    // Métodos para recarga de ki
    void iniciarRecargaKi();
    void detenerRecargaKi();
    bool estaRecargandoKi() const { return animacionKiActiva; }
    
    // Métodos para sistema de ki
    void establecerKi(int ki, int kiMax = 100);
    int obtenerKi() const { return kiActual; }
    int obtenerKiMaximo() const { return kiMaximo; }
    float obtenerPorcentajeKi() const { return (float)kiActual / kiMaximo * 100.0f; }

protected:

private slots:
    void actualizarAnimacionEntrada();
    void actualizarAnimacionKi();
    void recargarKi(); // Para incrementar el ki real

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

signals:
};

#endif // GOKU_H
