#ifndef KAMEHAMEHA_H
#define KAMEHAMEHA_H

#include "habilidad.h"
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class Kamehameha : public Habilidad
{
    Q_OBJECT

public:
    explicit Kamehameha(QObject *parent = nullptr);
    ~Kamehameha();

    // Métodos sobrescritos de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void detener() override;
    void actualizar() override;

    // Crear
    void crear(float x, float y, float dirX, float dirY, float velocidad, float alcance);

    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Control de estado
    bool estaActivo() const;

    // Hitbox y colisiones
    QRectF getHitbox() const;
    void verificarColisiones();
    bool colisionaCon(QGraphicsItem* item);
    void procesarColision(QGraphicsItem* item);

    // Visualización debug
    static void alternarVisualizacionHitbox();

private:
    // ==================== FÍSICA ====================

    void mover();
    void destruir();

    float x, y;
    float dirX, dirY;
    float velocidad;
    float alcance;
    float distanciaRecorrida;
    bool activo;

    // ==================== ANIMACIÓN Y SPRITES ====================
    void cargarSprites();
    void iniciarAnimacion();
    void detenerAnimacion();
    void alternarSprites();

    QPixmap hame1, hame2, hame3;
    QPixmap hameha1, hameha2, hameha3;
    bool usarHameha;
    bool spritesValidos;
    QTimer *animacionTimer;

    int anchoTotal;
    int altoTotal;

    // ==================== HITBOX Y COLISIONES ====================
    QRectF hitbox;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados;
    int daño;

    static bool mostrarHitbox;

    // ==================== TRAZADO DE TRAYECTORIA ====================
    QList<QPointF> trayectoria;
    int maxPuntosTrayectoria;

    // ==================== ACTUALIZACIÓN ====================
    QTimer *timer;

private slots:
    void actualizar_timeout();
};

#endif // KAMEHAMEHA_H
