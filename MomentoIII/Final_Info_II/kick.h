#ifndef KICK_H
#define KICK_H

#include "habilidad.h"
#include "goku.h" // Necesario para acceder a Goku
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QList> // Necesario para QList

class Kick : public Habilidad
{
    Q_OBJECT
public:
    explicit Kick(QObject *parent = nullptr);
    ~Kick();

    // Implementación de métodos virtuales puros de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void actualizar() override; // Aquí se implementará la gravedad
    void detener() override;
    void destruir(); // Nuevo método para la limpieza final
    QRectF boundingRect() const override;

    // Implementación de métodos virtuales puros de QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Método para establecer el objetivo (Goku)
    // void establecerObjetivo(Goku* objetivo);

    static void alternarVisualizacionHitbox();

private:
    QTimer* timerAnimacion;
    int frameActual;

    //Goku* objetivoGoku; // Puntero al objeto Goku

    QRectF hitbox;
    int dano; // Daño de la habilidad
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados; // Para evitar múltiples hits del mismo objeto

    // Métodos para hitbox y colisiones
    void verificarColisiones();

    // Variable estática para mostrar hitbox (controlada por tecla H)
    static bool mostrarHitbox;

signals:
};

#endif // KICK_H
