#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QString>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Personaje(QObject *parent = nullptr);
    virtual ~Personaje();

    // Funciones virtuales
    virtual void moverDerecha() = 0;
    virtual void moverIzquierda() = 0;
    virtual void moverArriba() = 0;
    virtual void moverAbajo() = 0;
    virtual void atacar() = 0;
    virtual void recibirDanio(int danio) = 0;

    // Funciones virtuales
    virtual void saltar();
    virtual void cambiarSprite(const QString& direccion);
    virtual void iniciarAnimacionIdle();
    virtual void establecerEscala(qreal escala);
    virtual void morir();

    // Funciones virtuales

    void establecerVida(int vidaMaxima);
    void establecerVelocidad(qreal velocidad);
    void establecerNombre(const QString& nombre);
    void establecerCarpetaSprites(const QString& carpeta);
    
    // Getters
    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    qreal getVelocidad() const { return velocidadMovimiento; }
    QString getNombre() const { return nombre; }
    bool estaVivo() const { return vida > 0; }
    bool estaSaltando() const { return saltando; }

protected:
    // Variables protegidas
    QString nombre;
    QString carpetaSprites;
    int vida;
    int vidaMaxima;
    qreal velocidadMovimiento;
    qreal escalaSprite;
    
    // Variables de animación
    int frameActual;
    int frameMaximo;
    QTimer *animacionTimer;
    bool moviendose;
    
    // Variables de salto
    bool saltando;
    qreal alturaOriginal;
    qreal velIn;
    qreal tiempo;
    QTimer *jumpTimer;


    //Slots y signals una especie de escucha y digo entre clases, aun testeando
protected slots:
    virtual void actualizarSalto();
    virtual void actualizarAnimacion();

signals:
    void personajeMuerto(Personaje* personaje);
    void vidaCambiada(int vidaActual, int vidaMaxima);
    void personajeAtaco(Personaje* atacante);
};

#endif // PERSONAJE_H
