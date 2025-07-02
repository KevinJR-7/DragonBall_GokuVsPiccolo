#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QString>
#include <QGraphicsRectItem>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Personaje(QObject *parent = nullptr);
    virtual ~Personaje();

    // Métodos virtuales puros (deben ser implementados por las clases hijas)
    virtual void moverDerecha();
    virtual void moverIzquierda();
    virtual void moverArriba();
    virtual void moverAbajo();
    virtual void atacar();
    virtual void recibirDanio(int danio);

    // Métodos virtuales con implementación por defecto (pueden ser sobrescritos)
    virtual void saltar();
    virtual void cambiarSprite(const QString& direccion);
    virtual void iniciarAnimacionIdle();
    virtual void establecerEscala(qreal escala);
    virtual void morir();

    // Métodos no virtuales (comunes para todos los personajes)
    void establecerVida(int vidaMaxima);
    void establecerVelocidad(qreal velocidad);
    void establecerNombre(const QString& nombre);
    void establecerCarpetaSprites(const QString& carpeta);
    
    // Métodos para configurar física del salto
    void establecerFisicaSalto(qreal masa, qreal resistencia = 0.1);
    void establecerVelocidadSalto(qreal velocidadInicial);
    
    // Métodos para manejo de hitboxes
    void establecerHitbox(qreal ancho, qreal alto, qreal offsetX = 0, qreal offsetY = 0);
    QRectF obtenerHitbox() const;
    QRectF obtenerHitboxGlobal() const;
    bool colisionaCon(Personaje* otroPersonaje) const;
    bool colisionaCon(const QRectF& rectangulo) const;
    
    // Métodos para límites de pantalla
    void verificarLimitesPantalla(const QRectF& limitesEscena);
    void establecerLimitesEscena(const QRectF& limites);
    
    // Métodos para visualización de hitbox
    void mostrarHitbox(bool mostrar = true);
    void ocultarHitbox();
    bool estaHitboxVisible() const { return hitboxVisible; }
    
    // Getters
    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    qreal getVelocidad() const { return velocidadMovimiento; }
    QString getNombre() const { return nombre; }
    bool estaVivo() const { return vida > 0; }
    bool estaSaltando() const { return saltando; }

protected:
    // Variables protegidas (accesibles por las clases hijas)
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
    
    // Variables para animación de salto (7 sprites)
    int frameSaltoActual;
    qreal alturaMaximaAlcanzada;
    
    // Variables para física avanzada del salto
    qreal velocidadVertical;     // dy/dt - velocidad actual
    qreal aceleracionVertical;   // d²y/dt² - aceleración actual
    qreal coeficienteResistencia; // Resistencia del aire
    qreal masaPersonaje;         // Masa del personaje
    qreal deltaT;                // Paso de tiempo para integración

    // Variables para hitbox y colisiones
    qreal hitboxAncho;
    qreal hitboxAlto;
    qreal hitboxOffsetX;
    qreal hitboxOffsetY;
    QRectF limitesEscena;
    
    // Variables para visualización de hitbox
    bool hitboxVisible;
    QGraphicsRectItem* hitboxVisual;
    
protected slots:
    virtual void actualizarSalto();
    virtual void actualizarAnimacion();
    void actualizarVisualizacionHitbox();

signals:
    void personajeMuerto(Personaje* personaje);
    void vidaCambiada(int vidaActual, int vidaMaxima);
    void personajeAtaco(Personaje* atacante);
};

#endif // PERSONAJE_H
