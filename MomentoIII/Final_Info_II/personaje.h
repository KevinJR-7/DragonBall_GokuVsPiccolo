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
    virtual void cambiarSpriteCentrado(const QString& direccion);
    virtual void cambiarSpriteConOffset(const QString& direccion, qreal offsetX, qreal offsetY);
    virtual void iniciarAnimacionIdle();
    virtual void establecerEscala(qreal escala);
    virtual void morir();
    
    // Método para personalizar el dibujo (visualización de trayectorias)
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Métodos no virtuales (comunes para todos los personajes)
    void establecerVida(int vidaMaxima);
    void establecerVelocidad(qreal velocidad);
    void establecerNombre(const QString& nombre);
    void establecerCarpetaSprites(const QString& carpeta);
    
    // Métodos para configurar física del salto
    void establecerFisicaSalto(qreal masa, qreal resistencia = 0.1);
    void establecerVelocidadSalto(qreal velocidadInicial);
    void establecerVelocidadHorizontalSalto(qreal velocidadH);
    
    // Métodos para movimiento horizontal durante salto
    void aplicarMovimientoHorizontal(qreal deltaX);
    
    // Override setPos para debug
    void setPos(qreal x, qreal y);
    
    // Método temporal para pausar/reanudar timer de salto
    void pausarTimerSalto();
    void reanudarTimerSalto();
    
    // Métodos para manejo de hitboxes
    void establecerHitbox(qreal ancho, qreal alto, qreal offsetX = 0, qreal offsetY = 0);
    void establecerHitboxSalto(qreal ancho, qreal alto, qreal offsetX = 0, qreal offsetY = 0);
    void aplicarHitboxSalto();
    void restaurarHitboxNormal();
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
    QString baseFase;
    
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
    qreal velocidadHorizontalSalto; // dx/dt - velocidad horizontal durante salto
    qreal coeficienteResistencia; // Resistencia del aire
    qreal masaPersonaje;         // Masa del personaje
    qreal deltaT;                // Paso de tiempo para integración
    
    // Variables para movimiento horizontal durante el salto
    qreal posicionXSalto;        // Posición X deseada durante el salto
    qreal velocidadHorizontal;   // Velocidad horizontal acumulada
    
    // Variables para separar movimiento vertical del horizontal
    qreal offsetYSalto;          // Offset Y desde la posición base
    qreal posicionBaseSalto;     // Posición Y base al inicio del salto
    
    // Variables para hitbox y colisiones
    qreal hitboxAncho;
    qreal hitboxAlto;
    qreal hitboxOffsetX;
    qreal hitboxOffsetY;
    
    // Variables para hitbox de salto
    qreal hitboxSaltoAncho;
    qreal hitboxSaltoAlto;
    qreal hitboxSaltoOffsetX;
    qreal hitboxSaltoOffsetY;
    
    // Variables para hitbox normal (respaldo)
    qreal hitboxNormalAncho;
    qreal hitboxNormalAlto;
    qreal hitboxNormalOffsetX;
    qreal hitboxNormalOffsetY;
    
    QRectF limitesEscena;
    
    // Variables para visualización de hitbox
    bool hitboxVisible;
    QGraphicsRectItem* hitboxVisual;
    
    // Variables para trazado de trayectoria de salto
    QList<QPointF> trayectoriaSalto; // Lista de puntos de la trayectoria del salto
    int maxPuntosTrayectoriaSalto; // Máximo número de puntos a guardar
    
protected slots:
    virtual void actualizarSalto();
    virtual void actualizarAnimacion();
    void actualizarVisualizacionHitbox();

signals:
    void personajeMuerto(Personaje* personaje);
    void vidaCambiada(int vidaActual, int vidaMaxima);
    void personajeAtaco(Personaje* atacante);
    //void personajeAterrizo(); // Nueva señal para cuando el personaje aterriza
};

#endif // PERSONAJE_H
