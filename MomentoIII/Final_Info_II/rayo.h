#ifndef RAYO_H
#define RAYO_H

#include "habilidad.h"
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class Rayo : public Habilidad
{
    Q_OBJECT
public:
    explicit Rayo(QObject *parent = nullptr);
    ~Rayo();

    // Sistema simple de proyectil
    void crear(float x, float y, float dirX, float dirY, float velocidad, float alcance);
    void mover();
    void actualizar() override;
    void dibujar(QPainter *painter);
    bool estaActivo() const;
    void destruir();

    // Implementación de métodos virtuales puros de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void detener() override;

    // QGraphicsItem implementation
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Métodos específicos del Rayo
    void establecerPotencia(int potencia);
    void establecerSegmentos(int segmentos);

    // Sistema de hitbox y colisiones
    QRectF getHitbox() const;
    void verificarColisiones();
    bool colisionaCon(QGraphicsItem* item);
    void procesarColision(QGraphicsItem* item);

    // Método estático para alternar visualización del hitbox
    static void alternarVisualizacionHitbox();

private:
    // Variables básicas del proyectil
    float x, y;
    float dirX, dirY;
    float velocidad;
    float alcance;
    float distanciaRecorrida;
    bool activo;
    QTimer *timer;

    // Sprites del rayo
    QPixmap hame1, hame2, hame3;
    QPixmap hameha1, hameha2, hameha3;
    bool spritesValidos;

    // Configuración del rayo compuesto
    int anchoTotal;
    int altoTotal;

    // Sistema de animación intercalada
    bool usarHameha; // true = usar hameha, false = usar hame
    QTimer *animacionTimer;
    int contadorAnimacion;

    // Variables del hitbox
    QRectF hitbox;
    int daño;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados; // Para evitar múltiples hits del mismo objeto

    // Variable estática para mostrar hitbox (controlada por tecla H)
    static bool mostrarHitbox;

    void cargarSprites();
    void construirRayo();
    void iniciarAnimacion();
    void detenerAnimacion();

private slots:
    void actualizar_timeout();
    void alternarSprites(); // Nuevo slot para alternar sprites
};

#endif // RAYO_H
