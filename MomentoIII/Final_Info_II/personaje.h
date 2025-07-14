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
#include <QPainter> // Necesario para QPainter

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // Constructor y destructor
    explicit Personaje(QObject *parent = nullptr);
    virtual ~Personaje();

    // Métodos virtuales puros para acciones básicas que deben ser implementadas por las clases hijas
    virtual void moverDerecha();
    virtual void moverIzquierda();
    virtual void moverArriba();
    virtual void moverAbajo();
    virtual void atacar();
    virtual void recibirDanio(int danio);

    // Métodos virtuales con implementación por defecto que pueden ser sobrescritos
    virtual void saltar();
    virtual void cambiarSprite(const QString& direccion);
    virtual void cambiarSpriteCentrado(const QString& direccion);
    virtual void cambiarSpriteConOffset(const QString& direccion, qreal offsetX, qreal offsetY);
    virtual void iniciarAnimacionIdle();
    virtual void establecerEscala(qreal escala);
    virtual void morir();
    virtual void iniciarAnimacionEntrada();
    virtual void kiCambiado(int kiActual, int kiMaximo);

    // Verifica si el personaje está en la animación de entrada
    bool estaEnAnimacionEntrada() const { return animacionEntradaActiva; }

    // Personaliza el dibujo del personaje (ej. para visualizar trayectorias o hitboxes)
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Métodos no virtuales (comunes para todos los personajes)
    void establecerVida(int vidaMaxima);
    void establecerVelocidad(qreal velocidad);
    void establecerNombre(const QString& nombre);
    void establecerCarpetaSprites(const QString& carpeta);

    // Métodos para configurar la física del salto
    void establecerFisicaSalto(qreal masa, qreal resistencia = 0.1);
    void establecerVelocidadSalto(qreal velocidadInicial);
    void establecerVelocidadHorizontalSalto(qreal velocidadH);

    // Método para aplicar movimiento horizontal durante el salto
    void aplicarMovimientoHorizontal(qreal deltaX);

    // Sobreescritura de setPos para propósitos de depuración
    void setPos(qreal x, qreal y);

    // Métodos para pausar/reanudar el temporizador de salto
    void pausarTimerSalto();
    void reanudarTimerSalto();

    // Métodos para el manejo de hitboxes (áreas de colisión)
    void establecerHitbox(qreal ancho, qreal alto, qreal offsetX = 0, qreal offsetY = 0);
    void establecerHitboxSalto(qreal ancho, qreal alto, qreal offsetX = 0, qreal offsetY = 0);
    void aplicarHitboxSalto();
    void restaurarHitboxNormal();
    QRectF obtenerHitbox() const;
    QRectF obtenerHitboxGlobal() const;
    bool colisionaCon(Personaje* otroPersonaje) const;
    bool colisionaCon(const QRectF& rectangulo) const;

    // Métodos para verificar y establecer los límites de la pantalla/escena
    void verificarLimitesPantalla(const QRectF& limitesEscena);
    void establecerLimitesEscena(const QRectF& limites);

    // Métodos para la visualización del hitbox (para depuración)
    void mostrarHitbox(bool mostrar = true);
    void ocultarHitbox();
    bool estaHitboxVisible() const { return hitboxVisible; }

    // Getters para obtener propiedades del personaje
    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    qreal getVelocidad() const { return velocidadMovimiento; }
    QString getNombre() const { return nombre; }
    bool estaVivo() const { return vida > 0; }
    bool estaSaltando() const { return saltando; }

protected:
    // Variables protegidas, accesibles por las clases hijas
    QString ultimaDireccionHorizontal; // Dirección horizontal actual ("adelante", "atras", o "")
    QPointF posicionInicialQuieto; // Posición exacta antes de ciertas acciones (ej. cargar un ataque)
    int kiActual; // Nivel de ki actual
    int kiMaximo; // Nivel de ki máximo

    QString nombre; // Nombre del personaje
    QString carpetaSprites; // Ruta de la carpeta donde se encuentran los sprites del personaje
    int vida; // Vida actual del personaje
    int vidaMaxima; // Vida máxima del personaje
    qreal velocidadMovimiento; // Velocidad de movimiento del personaje
    qreal escalaSprite; // Escala a la que se dibuja el sprite

    // Variables para la animación de entrada
    bool animacionEntradaActiva;
    int frameEntradaActual;
    QTimer* timerEntrada;

    // Variables de animación general
    int frameActual; // Frame actual de la animación
    int frameMaximo; // Número total de frames en la animación
    QTimer *animacionTimer; // Temporizador para la actualización de la animación
    bool moviendose; // Indica si el personaje se está moviendo
    QString baseFase; // Fase base del personaje (ej. "normal", "super_saiyajin")

    // Variables de salto
    bool saltando; // Indica si el personaje está saltando
    qreal alturaOriginal; // Altura inicial antes del salto
    qreal velIn; // Velocidad inicial de salto
    qreal tiempo; // Tiempo transcurrido durante el salto
    QTimer *jumpTimer; // Temporizador para la lógica del salto

    // Variables para animación de salto (7 sprites)
    int frameSaltoActual; // Frame actual de la animación de salto
    qreal alturaMaximaAlcanzada; // Altura máxima alcanzada durante el salto

    // Variables para física avanzada del salto
    qreal velocidadVertical;     // Velocidad vertical actual (dy/dt)
    qreal aceleracionVertical;   // Aceleración vertical actual (d²y/dt²)
    qreal velocidadHorizontalSalto; // Velocidad horizontal durante el salto
    qreal coeficienteResistencia; // Resistencia del aire
    qreal masaPersonaje;         // Masa del personaje
    qreal deltaT;                // Paso de tiempo para la integración física

    // Variables para movimiento horizontal durante el salto
    qreal posicionXSalto;        // Posición X deseada durante el salto
    qreal velocidadHorizontal;   // Velocidad horizontal acumulada

    // Variables para separar el movimiento vertical del horizontal
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

    QRectF limitesEscena; // Límites de la escena para el movimiento del personaje

    // Variables para la visualización del hitbox
    bool hitboxVisible; // Indica si el hitbox es visible para depuración
    QGraphicsRectItem* hitboxVisual; // Objeto gráfico para mostrar el hitbox

    // Variables para el trazado de trayectoria de salto (para depuración o visualización)
    QList<QPointF> trayectoriaSalto; // Lista de puntos de la trayectoria del salto
    int maxPuntosTrayectoriaSalto; // Máximo número de puntos a guardar

protected slots:
    // Slots protegidos para la actualización de la lógica del personaje
    virtual void actualizarSalto();
    virtual void actualizarAnimacion();
    void actualizarVisualizacionHitbox(); // Actualiza la representación visual del hitbox
    virtual void actualizarAnimacionEntrada();

signals:
    void personajeMuerto(Personaje* personaje);
    void vidaCambiada(int vidaActual, int vidaMaxima);
    void personajeAtaco(Personaje* atacante);

    //void personajeAterrizo(); // Nueva señal para cuando el personaje aterriza
};

#endif // PERSONAJE_H
