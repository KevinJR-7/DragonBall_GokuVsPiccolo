#include "kamehameha.h"
#include "piccolo.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>

// ==================== VARIABLE ESTÁTICA ====================
bool Kamehameha::mostrarHitbox = false;

// ==================== CONSTRUCTOR / DESTRUCTOR ====================
Kamehameha::Kamehameha(QObject *parent)
    : Habilidad(parent),
    x(0), y(0),
    dirX(1), dirY(0),
    velocidad(8.0),
    alcance(800.0),
    distanciaRecorrida(0),
    activo(false),
    spritesValidos(false),
    anchoTotal(150),
    altoTotal(50),
    usarHameha(false),
    daño(20),
    hitboxActivo(false)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Kamehameha::actualizar_timeout);

    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200);
    connect(animacionTimer, &QTimer::timeout, this, &Kamehameha::alternarSprites);

    maxPuntosTrayectoria = 100;
    trayectoria.clear();

    cargarSprites();
    setScale(2.33);

    qDebug() << "Kamehameha creado como proyectil simple";
}

Kamehameha::~Kamehameha()
{
    if (timer) timer->stop();
    qDebug() << "Kamehameha destruido";
}

// ==================== SPRITES Y ANIMACIÓN ====================
void Kamehameha::cargarSprites()
{
    hame1 = QPixmap(":/Goku/Sprites/goku/hame1.png");
    hame2 = QPixmap(":/Goku/Sprites/goku/hame2.png");
    hame3 = QPixmap(":/Goku/Sprites/goku/hame3.png");

    hameha1 = QPixmap(":/Goku/Sprites/goku/hameha1.png");
    hameha2 = QPixmap(":/Goku/Sprites/goku/hameha2.png");
    hameha3 = QPixmap(":/Goku/Sprites/goku/hameha3.png");

    bool hameValidos = !hame1.isNull() && !hame2.isNull() && !hame3.isNull();
    bool hamehaValidos = !hameha1.isNull() && !hameha2.isNull() && !hameha3.isNull();

    if (!hameValidos || !hamehaValidos) {
        if (hame1.isNull()) hame1 = QPixmap(50, 25); hame1.fill(Qt::blue);
        if (hame2.isNull()) hame2 = QPixmap(40, 25); hame2.fill(Qt::yellow);
        if (hame3.isNull()) hame3 = QPixmap(60, 25); hame3.fill(Qt::red);
        if (hameha1.isNull()) hameha1 = QPixmap(50, 25); hameha1.fill(Qt::cyan);
        if (hameha2.isNull()) hameha2 = QPixmap(40, 25); hameha2.fill(Qt::magenta);
        if (hameha3.isNull()) hameha3 = QPixmap(60, 25); hameha3.fill(Qt::green);
        spritesValidos = false;
    } else {
        spritesValidos = true;
    }

    float factorEscala = 0.35f;
    anchoTotal = (hame1.width() + hame2.width() + hame3.width()) * factorEscala;
    altoTotal = qMax(hame1.height(), qMax(hame2.height(), hame3.height())) * factorEscala;
}

void Kamehameha::iniciarAnimacion()
{
    usarHameha = false;
    animacionTimer->start();
    qDebug() << "Animación intercalada iniciada";
}

void Kamehameha::detenerAnimacion()
{
    if (animacionTimer->isActive()) {
        animacionTimer->stop();
        qDebug() << "Animación intercalada detenida";
    }
}

void Kamehameha::alternarSprites()
{
    usarHameha = !usarHameha;
    update(); // fuerza repaint
}

// ==================== FÍSICA Y MOVIMIENTO ====================
void Kamehameha::crear(float x, float y, float dirX, float dirY, float velocidad, float alcance)
{
    this->x = x; this->y = y;
    this->dirX = dirX; this->dirY = dirY;
    this->velocidad = velocidad;
    this->alcance = alcance;
    this->distanciaRecorrida = 0;
    this->activo = true;

    hitboxActivo = true;
    objetosGolpeados.clear();

    setPos(x, y);
    setZValue(100);

    timer->start(32);
    iniciarAnimacion();
}

void Kamehameha::mover()
{
    if (!activo) return;

    x += dirX * velocidad;
    y += dirY * velocidad;
    setPos(x, y);

    trayectoria.append(QPointF(x, y));
    if (trayectoria.size() > maxPuntosTrayectoria)
        trayectoria.removeFirst();

    distanciaRecorrida += velocidad;

    if (hitboxActivo) verificarColisiones();
    if (distanciaRecorrida >= alcance) destruir();
}

void Kamehameha::actualizar()
{
    mover();
}

void Kamehameha::actualizar_timeout()
{
    actualizar();
}

bool Kamehameha::estaActivo() const
{
    return activo;
}

void Kamehameha::destruir()
{
    activo = false;
    timer->stop();
    detenerAnimacion();
    trayectoria.clear();
    if (scene()) scene()->removeItem(this);
    qDebug() << "Kamehameha destruido";
}

// ==================== INTERFAZ HABILIDAD ====================
void Kamehameha::iniciar(QPointF posicionInicial, QPointF direccion)
{
    float x = posicionInicial.x();
    float y = posicionInicial.y();

    float magnitude = std::sqrt(direccion.x() * direccion.x() + direccion.y() * direccion.y());
    float dirX = (magnitude > 0) ? direccion.x() / magnitude : 1.0f;
    float dirY = (magnitude > 0) ? direccion.y() / magnitude : 0.0f;

    float velocidad = 8.0f;
    float alcance = 600.0f;

    crear(x, y, dirX, dirY, velocidad, alcance);
}

void Kamehameha::detener()
{
    destruir();
}

// ==================== INTERFAZ QGRAPHICSITEM ====================
QRectF Kamehameha::boundingRect() const
{
    return QRectF(0, 0, anchoTotal, altoTotal);
}

void Kamehameha::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!activo) return;

    float factorEscala = 0.35f;
    qreal posX = 0;
    qreal centroY = altoTotal / 2.0;

    QPixmap sprite1 = usarHameha ? hameha1 : hame1;
    QPixmap sprite2 = usarHameha ? hameha2 : hame2;
    QPixmap sprite3 = usarHameha ? hameha3 : hame3;

    auto dibujar = [&](QPixmap &sprite, QColor colorPlaceholder, qreal anchoPlaceholder) {
        if (!sprite.isNull()) {
            QPixmap scaled = sprite.scaled(sprite.width() * factorEscala, sprite.height() * factorEscala, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            qreal y = centroY - scaled.height() / 2.0;
            painter->drawPixmap(posX, y, scaled);
            posX += scaled.width();
        } else {
            qreal alto = 25 * factorEscala;
            qreal y = centroY - alto / 2.0;
            painter->fillRect(posX, y, anchoPlaceholder, alto, colorPlaceholder);
            posX += anchoPlaceholder;
        }
    };

    dibujar(sprite1, usarHameha ? Qt::cyan : Qt::blue, 50 * factorEscala);
    dibujar(sprite2, usarHameha ? Qt::magenta : Qt::yellow, 40 * factorEscala);
    dibujar(sprite3, usarHameha ? Qt::green : Qt::red, 60 * factorEscala);

    if (mostrarHitbox && hitboxActivo) {
        QPen penHitbox(Qt::red, 2, Qt::DashLine);
        painter->setPen(penHitbox);
        painter->setBrush(Qt::NoBrush);

        float alto = altoTotal * 0.85f + 1.0f;
        float offsetY = altoTotal * 0.15f - 1.0f;
        painter->drawRect(0, offsetY, anchoTotal, alto);
    }

    if (mostrarHitbox && trayectoria.size() > 1) {
        QPen pen(Qt::green, 2, Qt::DashLine);
        painter->setPen(pen);
        for (int i = 0; i < trayectoria.size() - 1; ++i)
            painter->drawLine(mapFromScene(trayectoria[i]), mapFromScene(trayectoria[i + 1]));
        painter->drawLine(mapFromScene(trayectoria.last()), QPointF(0, 0));
    }
}

// ==================== HITBOX Y COLISIONES ====================
QRectF Kamehameha::getHitbox() const
{
    if (!activo || !hitboxActivo) return QRectF();

    float alto = altoTotal * 0.85f + 1.0f;
    float offsetY = altoTotal * 0.15f - 1.0f;
    return QRectF(pos().x(), pos().y() + offsetY, anchoTotal, alto);
}

void Kamehameha::verificarColisiones()
{
    if (!scene() || !hitboxActivo) return;

    QRectF hitboxK = getHitbox();
    QList<QGraphicsItem*> items = scene()->items(hitboxK);

    for (QGraphicsItem* item : items) {
        if (item == this || objetosGolpeados.contains(item)) continue;
        if (colisionaCon(item)) {
            procesarColision(item);
            objetosGolpeados.append(item);
        }
    }
}

bool Kamehameha::colisionaCon(QGraphicsItem* item)
{
    return hitboxActivo && getHitbox().intersects(item->sceneBoundingRect());
}

void Kamehameha::procesarColision(QGraphicsItem* item)
{
    if (Piccolo* piccolo = dynamic_cast<Piccolo*>(item)) {
        piccolo->recibirDanio(daño);
        destruir();
    }
}

// ==================== UTILIDADES ====================
void Kamehameha::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
}
