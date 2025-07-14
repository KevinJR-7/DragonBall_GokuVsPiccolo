#include "gravityblast.h"
#include "piccolo.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QLineF>
#include <QVector2D>

// ==============================================================
//                       VARIABLE ESTÁTICA
// ==============================================================

bool GravityBlast::mostrarHitbox = false;

// ==============================================================
//                         CONSTRUCTOR
// ==============================================================

GravityBlast::GravityBlast(QObject *parent)
    : Habilidad(parent),
    frameActual(1),
    objetivoGoku(nullptr),
    fuerzaGravitacional(0.3),
    velocidadMaxima(7.0),
    velocidadActual(0.0, 0.0),
    dano(5),
    hitboxActivo(true)
{
    hitbox = QRectF(0, 0, 16, 16);

    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(80);
    connect(timerAnimacion, &QTimer::timeout, this, &GravityBlast::actualizarAnimacion);

    cargarSprites();

    establecerVelocidad(0.0);
    establecerDano(dano);
    establecerAlcance(600.0);
    setZValue(1);
    setScale(2);


}

// ==============================================================
//                         DESTRUCTOR
// ==============================================================

GravityBlast::~GravityBlast()
{
    if (timerAnimacion) timerAnimacion->stop();

}

// ==============================================================
//                      INICIALIZACIÓN
// ==============================================================

void GravityBlast::cargarSprites()
{
    spriteGravityBlast1 = QPixmap(":/Piccolo/Sprites/piccolo/blastg1.png");
    spriteGravityBlast2 = QPixmap(":/Piccolo/Sprites/piccolo/blastg2.png");
    spriteGravityBlast3 = QPixmap(":/Piccolo/Sprites/piccolo/blastg3.png");
    spriteGravityBlast4 = QPixmap(":/Piccolo/Sprites/piccolo/blastg4.png");

    if (spriteGravityBlast1.isNull() || spriteGravityBlast2.isNull() ||
        spriteGravityBlast3.isNull() || spriteGravityBlast4.isNull()) {
    }

    spriteActual = spriteGravityBlast1;
}

void GravityBlast::iniciar(QPointF posicionInicial, QPointF direccion)
{
    setPos(posicionInicial);
    direccionMovimiento = direccion;
    direccionMovimientoQVector = QVector2D(direccion);

    activa = true;
    timerActualizacion->start();
    timerAnimacion->start();
}

// ==============================================================
//                    ACTUALIZACIÓN DE FÍSICA
// ==============================================================

void GravityBlast::actualizar()
{
    if (!objetivoGoku) {
        detener();
        return;
    }

    QPointF posActual = pos();
    QPointF posObjetivo = objetivoGoku->pos() + QPointF(0, 100);

    QVector2D direccionHaciaObjetivo = QVector2D(posObjetivo - posActual);

    if (direccionHaciaObjetivo.length() > 0) {
        direccionHaciaObjetivo.normalize();
    } else {
        detener();
        return;
    }

    velocidadActual += direccionHaciaObjetivo * fuerzaGravitacional;

    if (velocidadActual.length() > velocidadMaxima) {
        velocidadActual.normalize();
        velocidadActual *= velocidadMaxima;
    }

    direccionMovimiento = velocidadActual.toPointF();
}

void GravityBlast::actualizarFisica()
{
    if (!activa) return;

    actualizar();
    setPos(pos() + velocidadActual.toPointF());

    distanciaRecorrida += velocidadActual.length();

    if (distanciaRecorrida >= alcanceMaximo) {
        detener();
        return;
    }

    verificarLimites();
    actualizarHitbox();
    verificarColisiones();
}

// ==============================================================
//                      FINALIZACIÓN
// ==============================================================

void GravityBlast::detener()
{
    activa = false;
    timerActualizacion->stop();
    timerAnimacion->stop();
    emit habilidadTerminada(this);
    destruir();
}

void GravityBlast::destruir()
{
    activa = false;

    if (timerAnimacion) timerAnimacion->stop();
    if (timerActualizacion) timerActualizacion->stop();

    objetosGolpeados.clear();

    if (scene()) {
        scene()->removeItem(this);

    }

    deleteLater();

}

// ==============================================================
//                       QGRAPHICSITEM
// ==============================================================

QRectF GravityBlast::boundingRect() const
{
    return spriteActual.rect();
}

void GravityBlast::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!spriteActual.isNull()) {
        painter->drawPixmap(0, 0, spriteActual);
    } else {
        painter->setBrush(QBrush(Qt::magenta));
        painter->setPen(QPen(Qt::darkMagenta, 2));
        painter->drawEllipse(0, 0, 20, 20);
    }

    if (mostrarHitbox && hitboxActivo) {
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::transparent);
        painter->drawRect(hitbox);
    }
}

// ==============================================================
//                    GESTIÓN DE COLISIONES
// ==============================================================

void GravityBlast::actualizarHitbox()
{
    QRectF bounds = boundingRect();
    hitbox = QRectF(bounds.x() + (bounds.width() - 16) / 2, bounds.y() + (bounds.height() - 16) / 2, 16, 16);
}

void GravityBlast::verificarColisiones()
{
    QRectF hitboxEscena = mapRectToScene(hitbox);
    QList<QGraphicsItem*> itemsColision = scene()->items(hitboxEscena);

    for (QGraphicsItem* item : itemsColision) {
        if (item == this || objetosGolpeados.contains(item)) continue;

        if (Personaje* personaje = dynamic_cast<Personaje*>(item)) {
            if (personaje == parent()) continue;

            objetosGolpeados.append(item);
            personaje->recibirDanio(dano);
            detener();
            return;
        }
    }
}

// ==============================================================
//                      ANIMACIÓN
// ==============================================================

void GravityBlast::actualizarAnimacion()
{
    frameActual++;
    if (frameActual > 4) frameActual = 1;

    switch (frameActual) {
    case 1: spriteActual = spriteGravityBlast1; break;
    case 2: spriteActual = spriteGravityBlast2; break;
    case 3: spriteActual = spriteGravityBlast3; break;
    case 4: spriteActual = spriteGravityBlast4; break;
    }
    update();
}

// ==============================================================
//                      OTROS MÉTODOS
// ==============================================================

void GravityBlast::establecerObjetivo(Goku* objetivo)
{
    objetivoGoku = objetivo;

}

void GravityBlast::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;

}
