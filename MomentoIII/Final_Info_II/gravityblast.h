#ifndef GRAVITYBLAST_H
#define GRAVITYBLAST_H

#include "habilidad.h"
#include "goku.h"
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QList>
#include <QVector2D>

class GravityBlast : public Habilidad
{
    Q_OBJECT

public:
    // ==================== CONSTRUCTOR / DESTRUCTOR ====================
    explicit GravityBlast(QObject *parent = nullptr);
    ~GravityBlast();

    // ==================== MÉTODOS DE HABILIDAD ====================
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void actualizar() override;
    void actualizarFisica() override;
    void detener() override;
    void destruir();

    // ==================== QGRAPHICSITEM ====================
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // ==================== CONFIGURACIÓN ====================
    void establecerObjetivo(Goku* objetivo);
    static void alternarVisualizacionHitbox();

private:
    // ==================== SPRITES ====================
    QPixmap spriteActual;
    QPixmap spriteGravityBlast1;
    QPixmap spriteGravityBlast2;
    QPixmap spriteGravityBlast3;
    QPixmap spriteGravityBlast4;

    // ==================== ANIMACIÓN ====================
    QTimer* timerAnimacion;
    int frameActual;

    // ==================== FÍSICA ====================
    Goku* objetivoGoku;
    qreal fuerzaGravitacional;
    qreal velocidadMaxima;
    QVector2D velocidadActual;
    QVector2D direccionMovimientoQVector;

    // ==================== HITBOX ====================
    QRectF hitbox;
    int dano;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados;

    // ==================== TIEMPO DE VIDA ====================
    qreal tiempoVida;
    qreal tiempoMaximo;
    qreal dt;

    // ==================== FUNCIONES INTERNAS ====================
    void cargarSprites();
    void actualizarAnimacion();
    void actualizarHitbox();
    void verificarColisiones();

    // ==================== DEBUG ====================
    static bool mostrarHitbox;
};

#endif // GRAVITYBLAST_H
