#ifndef GRAVITYBLAST_H
#define GRAVITYBLAST_H

#include "habilidad.h"
#include "goku.h" // Necesario para acceder a Goku
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QList> // Necesario para QList
#include <cmath>
#include <QVector2D>

class GravityBlast : public Habilidad
{
    Q_OBJECT

public:
    explicit GravityBlast(QObject *parent = nullptr);
    virtual ~GravityBlast();

    // Implementación de métodos virtuales puros de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void actualizar() override; // Aquí se implementará la gravedad
    void actualizarFisica() override; // SOBRESCRIBIR para manejar el movimiento y colisiones
    void detener() override;
    void destruir(); // Nuevo método para la limpieza final

    // Implementación de métodos virtuales puros de QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Método para establecer el objetivo (Goku)
    void establecerObjetivo(Goku* objetivo);

    static void alternarVisualizacionHitbox();

private:
    QPixmap spriteActual;
    QPixmap spriteGravityBlast1; // Sprites para animación
    QPixmap spriteGravityBlast2;
    QPixmap spriteGravityBlast3;
    QPixmap spriteGravityBlast4;

    QTimer* timerAnimacion;
    int frameActual;

    Goku* objetivoGoku; // Puntero al objeto Goku
    qreal fuerzaGravitacional; // Fuerza de atracción hacia Goku
    qreal velocidadMaxima; // Velocidad máxima del proyectil
    QVector2D velocidadActual; // Velocidad actual del proyectil
    QVector2D direccionMovimientoQVector; // Dirección actual como QVector2D

    // Variables del hitbox (inspirado en BlastB)
    QRectF hitbox;
    int dano; // Daño de la habilidad
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados; // Para evitar múltiples hits del mismo objeto

    // Tiempo de vida
    qreal tiempoVida;
    qreal tiempoMaximo;
    qreal dt;

    void cargarSprites();
    void actualizarAnimacion();
    // Métodos para hitbox y colisiones
    void actualizarHitbox();
    void verificarColisiones();

    // Variable estática para mostrar hitbox (controlada por tecla H)
    static bool mostrarHitbox;
};

#endif // GRAVITYBLAST_H
