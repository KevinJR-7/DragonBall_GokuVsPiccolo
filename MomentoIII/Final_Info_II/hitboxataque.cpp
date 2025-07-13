#include "hitboxataque.h"
#include "piccolo.h"
#include <QGraphicsScene>

HitboxAtaque::HitboxAtaque(qreal x, qreal y, qreal w, qreal h, int daño, int tiempoVidaMs, QObject* parent)
    : QObject(parent), QGraphicsRectItem(x, y, w, h), daño(daño)
{
    setBrush(QBrush(Qt::transparent));
    setPen(QPen(Qt::red));
    setZValue(10); // encima de sprites normales

    // Timer para autodestruirse
    timerDestruir = new QTimer(this);
    connect(timerDestruir, &QTimer::timeout, this, [this]() {
        scene()->removeItem(this);
        deleteLater();
    });
    timerDestruir->start(tiempoVidaMs);
}

void HitboxAtaque::advance(int step)
{
    if (!step || yaGolpeo || !scene()) return;

    QList<QGraphicsItem*> colisiones = scene()->collidingItems(this);
    for (QGraphicsItem* item : colisiones) {
        if (Piccolo* pic = dynamic_cast<Piccolo*>(item)) {
            yaGolpeo = true;

            pic->recibirDanio(daño);
            emit colisionConEnemigo();

            // Evita más colisiones
            setEnabled(false);
            setVisible(false);        // Para ocultarlo
            setRect(0, 0, 0, 0);       // Ya no tiene área

            // NO eliminar aquí. Lo eliminará el timer
            break;
        }
    }
}

