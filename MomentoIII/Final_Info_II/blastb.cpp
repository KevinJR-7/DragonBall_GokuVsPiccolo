#include "blastb.h"
#include "piccolo.h"
#include <QGraphicsScene>
#include <cmath>
#include <random>

// ============== Variable estática ==============
bool BlastB::mostrarHitbox = false;

// ============== Constructor y Destructor ==============
BlastB::BlastB(QObject *parent)
    : Habilidad(parent), frameActual(1), tiempoVida(0.0), tiempoMaximo(10.0)
{
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(80);
    connect(timerAnimacion, &QTimer::timeout, this, &BlastB::actualizarAnimacion);

    cargarSprites();
    configurarCaos();

    establecerVelocidad(30.0);
    establecerDano(10);
    establecerAlcance(800.0);

    hitbox = QRectF(0, 0, 10, 1);
    daño = 10;
    hitboxActivo = true;

    dt = 0.01;
    escala = 6.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    x = dis(gen);
    y = dis(gen);
    z = dis(gen);

    maxPuntosTrayectoria = 100;
    trayectoria.clear();


}

BlastB::~BlastB()
{
    if (timerAnimacion) timerAnimacion->stop();
}

// ============== Inicialización y Control ==============
void BlastB::crear(qreal x, qreal y, qreal dirX, qreal dirY, qreal velocidad, qreal alcance)
{
    posicionBase = QPointF(x, y);

    QPointF dir(dirX, dirY);
    qreal magnitud = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
    direccionBase = (magnitud > 0) ? dir / magnitud : QPointF(1.0, 0.0);

    establecerVelocidad(velocidad);
    establecerAlcance(alcance);

    setScale(1.2);
    setPos(posicionBase);


}

void BlastB::iniciar(QPointF posicionInicial, QPointF direccion)
{
    posicionBase = posicionInicial;

    qreal magnitud = qSqrt(direccion.x() * direccion.x() + direccion.y() * direccion.y());
    direccionBase = (magnitud > 0) ? direccion / magnitud : QPointF(1.0, 0.0);

    this->posicionInicial = posicionInicial;
    direccionMovimiento = direccion;

    tiempoVida = 0.0;
    distanciaRecorrida = 0.0;

    setPos(posicionBase);
    activa = true;

    timerAnimacion->start();
    timerActualizacion->start();


}

void BlastB::detener()
{
    if (!activa) return;

    activa = false;
    if (timerAnimacion) timerAnimacion->stop();
    if (timerActualizacion) timerActualizacion->stop();



    emit habilidadTerminada(this);

    if (scene()) scene()->removeItem(this);
    deleteLater();
}

void BlastB::destruir()
{
    timerAnimacion->stop();
    trayectoria.clear();
    if (scene()) scene()->removeItem(this);

}

// ============== Actualización y Movimiento ==============
void BlastB::actualizar()
{
    if (!activa) return;

    tiempoVida += dt;
    if (tiempoVida >= tiempoMaximo) {
        detener();
        return;
    }

    calcularSiguientePosicion();
    verificarLimites();
}

void BlastB::actualizarFisica()
{
    actualizar();
    actualizarHitbox();
    verificarColisiones();
}

void BlastB::calcularSiguientePosicion()
{
    QPointF movimientoBase = direccionBase * velocidadHabilidad;
    QPointF offset(0, 0);

    if (tipoTrayectoria == LORENZ) {
        actualizarAtractorLorenz();
        offset = obtenerPosicionCaotica();
    } else if (tipoTrayectoria == ESPIRAL) {
        float velocidadRadial = 15.0f;
        float velocidadAngular = 10.0f;
        float r = velocidadRadial * tiempoVida;
        float theta = velocidadAngular * tiempoVida;
        offset.setX(r * cos(theta));
        offset.setY(r * sin(theta));
    }

    QPointF posicionFinal = posicionBase + movimientoBase + offset;
    posicionBase += movimientoBase * 0.1;

    setPos(posicionFinal);
    trayectoria.append(posicionFinal);
    if (trayectoria.size() > maxPuntosTrayectoria) trayectoria.removeFirst();

    distanciaRecorrida += velocidadHabilidad * 0.1;
}

// ============== Atractor de Lorenz ==============
void BlastB::actualizarAtractorLorenz()
{
    dx = sigma * (y - x);
    dy = x * (rho - z) - y;
    dz = x * y - beta * z;

    x += dx * dt;
    y += dy * dt;
    z += dz * dt;
}

QPointF BlastB::obtenerPosicionCaotica()
{
    return QPointF(x * escala, y * escala);
}

void BlastB::configurarCaos(qreal sigma, qreal rho, qreal beta)
{
    this->sigma = sigma;
    this->rho = rho;
    this->beta = beta;

}

// ============== Animación ==============
void BlastB::cargarSprites()
{
    QString base = ":/Goku/Sprites/" + carpetaSprites + "/";
    spriteBlastB1 = QPixmap(base + "blastb1.png");
    spriteBlastB2 = QPixmap(base + "blastb2.png");
    spriteBlastB3 = QPixmap(base + "blastb3.png");
    spriteBlastB4 = QPixmap(base + "blastb4.png");

    if (spriteBlastB1.isNull() || spriteBlastB2.isNull() ||
        spriteBlastB3.isNull() || spriteBlastB4.isNull()) {

    }

    spriteActual = spriteBlastB1;
}

void BlastB::actualizarAnimacion()
{
    if (!activa) return;

    frameActual = (frameActual % 4) + 1;
    switch (frameActual) {
    case 1: spriteActual = spriteBlastB1; break;
    case 2: spriteActual = spriteBlastB2; break;
    case 3: spriteActual = spriteBlastB3; break;
    case 4: spriteActual = spriteBlastB4; break;
    }

    update();
}

// ============== Colisiones ==============
void BlastB::verificarColisiones()
{
    if (!hitboxActivo || !scene()) return;

    QRectF hitboxEscena = mapRectToScene(hitbox);
    QList<QGraphicsItem*> itemsColision = scene()->items(hitboxEscena);

    for (QGraphicsItem* item : itemsColision) {
        if (item == this || objetosGolpeados.contains(item)) continue;

        if (item->type() == QGraphicsPixmapItem::Type) {
            objetosGolpeados.append(item);


            if (Piccolo* piccolo = dynamic_cast<Piccolo*>(item)) {
                piccolo->recibirDanio(daño);
                destruir();
                return;
            }
        }
    }
}

void BlastB::actualizarHitbox()
{
    QRectF bounds = boundingRect();
    hitbox = QRectF(bounds.x() + 4, bounds.y() + 16, bounds.width() - 8, bounds.height() - 32);
}

QRectF BlastB::obtenerHitbox() const
{
    return hitbox;
}

void BlastB::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
}

// ============== QGraphicsItem ==============
QRectF BlastB::boundingRect() const
{
    return !spriteActual.isNull() ? spriteActual.rect() : QRectF(0, 0, 16, 16);
}

void BlastB::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!spriteActual.isNull()) {
        painter->drawPixmap(0, 0, spriteActual);
    } else {
        painter->setBrush(Qt::cyan);
        painter->setPen(QPen(Qt::blue, 2));
        painter->drawEllipse(0, 0, 16, 16);
    }

    if (mostrarHitbox && hitboxActivo) {
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(hitbox);

        if (trayectoria.size() > 1) {
            painter->setPen(QPen(Qt::yellow, 1, Qt::DashLine));
            for (int i = 0; i < trayectoria.size() - 1; ++i) {
                QPointF a = mapFromScene(trayectoria[i]);
                QPointF b = mapFromScene(trayectoria[i + 1]);
                painter->drawLine(a, b);
            }
            if (!trayectoria.isEmpty()) {
                QPointF ultimo = mapFromScene(trayectoria.last());
                QPointF actual = mapFromScene(pos());
                painter->drawLine(ultimo, actual);
            }
        }
    }
}

// ============== Personalización externa ==============
void BlastB::setCarpetaSprites(const QString& carpeta)
{
    carpetaSprites = carpeta;
}

void BlastB::setTipoTrayectoria(TipoTrayectoria tipo)
{
    tipoTrayectoria = tipo;
}
