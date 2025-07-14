#ifndef RAYO_H
#define RAYO_H

#include "habilidad.h"
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class Rayo : public Habilidad {
    Q_OBJECT
public:
    explicit Rayo(QObject *parent = nullptr);
    ~Rayo();

    // ==================== INICIALIZACIÓN ====================
    void crear(float x, float y, float dirX, float dirY, float velocidad, float alcance);
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void detener() override;

    // ==================== ACTUALIZACIÓN ====================
    void mover();
    void actualizar() override;
    void actualizar_timeout();

    // ==================== GESTIÓN DE VIDA ====================
    bool estaActivo() const;
    void destruir();

    // ==================== INTERFAZ GRÁFICA ====================
    void dibujar(QPainter *painter);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // ==================== CONFIGURACIÓN PERSONALIZADA ====================
    void establecerPotencia(int potencia);
    void establecerSegmentos(int segmentos);

    // ==================== COLISIONES ====================
    QRectF getHitbox() const;
    void verificarColisiones();
    bool colisionaCon(QGraphicsItem* item);
    void procesarColision(QGraphicsItem* item);
    static void alternarVisualizacionHitbox();

private:
    // ==================== PROPIEDADES BÁSICAS ====================
    QTransform transform;
    float x, y;
    float dirX, dirY;
    float velocidad;
    float alcance;
    float distanciaRecorrida;
    bool activo;
    QTimer *timer;

    // ==================== SPRITES Y ANIMACIÓN ====================
    QPixmap hame1, hame2, hame3;
    QPixmap hameha1, hameha2, hameha3;
    bool spritesValidos;
    bool usarHameha;
    QTimer *animacionTimer;
    int contadorAnimacion;
    void cargarSprites();
    void construirRayo();
    void iniciarAnimacion();
    void detenerAnimacion();
    void alternarSprites();

    // ==================== TAMAÑO Y ESCALA ====================
    int anchoTotal;
    int altoTotal;

    // ==================== HITBOX ====================
    QRectF hitbox;
    int daño;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados;
    static bool mostrarHitbox;

    // ==================== TRAZADO DE TRAYECTORIA ====================
    QList<QPointF> trayectoria;
    int maxPuntosTrayectoria;
};

#endif // RAYO_H
