// ==================== INICIALIZACIÓN ====================

#include "rayo.h"
#include "goku.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>

bool Rayo::mostrarHitbox = false;

Rayo::Rayo(QObject *parent)
    : Habilidad(parent), x(0), y(0), dirX(-1), dirY(0), velocidad(15.0), alcance(800.0),
    distanciaRecorrida(0), activo(false), spritesValidos(false), anchoTotal(150), altoTotal(50),
    usarHameha(false), contadorAnimacion(0), daño(10), hitboxActivo(false)
{
    transform.scale(-1, 1);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Rayo::actualizar_timeout);

    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200);
    connect(animacionTimer, &QTimer::timeout, this, &Rayo::alternarSprites);

    maxPuntosTrayectoria = 100;
    trayectoria.clear();

    cargarSprites();
    setScale(5);
}

Rayo::~Rayo() {
    if (timer) timer->stop();
}

// ==================== SPRITES ====================

void Rayo::cargarSprites() {
    hame1 = QPixmap(":/Piccolo/Sprites/piccolo/hame3.png").transformed(transform);
    hame2 = QPixmap(":/Piccolo/Sprites/piccolo/hame2.png").transformed(transform);
    hame3 = QPixmap(":/Piccolo/Sprites/piccolo/hame1.png").transformed(transform);
    hameha1 = QPixmap(":/Piccolo/Sprites/piccolo/hameha3.png").transformed(transform);
    hameha2 = QPixmap(":/Piccolo/Sprites/piccolo/hameha2.png").transformed(transform);
    hameha3 = QPixmap(":/Piccolo/Sprites/piccolo/hameha1.png").transformed(transform);

    bool hameOk = !hame1.isNull() && !hame2.isNull() && !hame3.isNull();
    bool hamehaOk = !hameha1.isNull() && !hameha2.isNull() && !hameha3.isNull();

    if (!hameOk || !hamehaOk) {
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

    float esc = 1.0f;
    anchoTotal = (hame1.width() + hame2.width() + hame3.width()) * esc;
    altoTotal = std::max({hame1.height(), hame2.height(), hame3.height()}) * esc;
}

// ==================== FUNCIONALIDAD PRINCIPAL ====================

void Rayo::crear(float x, float y, float dirX, float dirY, float velocidad, float alcance) {
    this->x = x; this->y = y; this->dirX = dirX; this->dirY = dirY;
    this->velocidad = velocidad; this->alcance = alcance;
    distanciaRecorrida = 0;
    activo = true;

    hitboxActivo = true;
    objetosGolpeados.clear();

    setPos(x, y);
    setZValue(100);
    timer->start(32);
    iniciarAnimacion();
}

void Rayo::mover() {
    if (!activo) return;
    x += dirX * velocidad;
    y += dirY * velocidad;
    setPos(x, y);
    distanciaRecorrida += velocidad;
    if (hitboxActivo) verificarColisiones();
    if (distanciaRecorrida >= alcance) { destruir(); return; }

    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    if (x < 0 || x > screenRect.width() || y < 0 || y > screenRect.height()) destruir();
}

void Rayo::actualizar() { mover(); }
void Rayo::actualizar_timeout() { actualizar(); }

// ==================== GESTIÓN DE ESTADO ====================

bool Rayo::estaActivo() const { return activo; }
void Rayo::destruir() {
    activo = false;
    timer->stop();
    detenerAnimacion();
    if (scene()) scene()->removeItem(this);
}

void Rayo::iniciar(QPointF posIni, QPointF dir) {
    float x = posIni.x(), y = posIni.y();
    float mag = std::hypot(dir.x(), dir.y());
    float dx = (mag > 0) ? dir.x() / mag : 1.0f;
    float dy = (mag > 0) ? dir.y() / mag : 0.0f;
    crear(x, y, dx, dy, 15.0f, 600.0f);
}

void Rayo::detener() { destruir(); }

// ==================== DIBUJO ====================

QRectF Rayo::boundingRect() const { return QRectF(0, 0, anchoTotal, altoTotal); }

void Rayo::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!activo) return;
    float esc = 1.0f, posX = 0, centroY = altoTotal / 2.0;
    QPixmap s1 = usarHameha ? hameha1 : hame1;
    QPixmap s2 = usarHameha ? hameha2 : hame2;
    QPixmap s3 = usarHameha ? hameha3 : hame3;

    auto dibujar = [&](QPixmap& sp, QColor color, int ancho, int alto) {
        if (!sp.isNull()) {
            QPixmap spScaled = sp.scaled(sp.width() * esc, sp.height() * esc, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            float y = centroY - spScaled.height() / 2.0;
            p->drawPixmap(posX, y, spScaled);
            posX += spScaled.width();
        } else {
            float y = centroY - alto / 2.0;
            p->fillRect(posX, y, ancho, alto, color);
            posX += ancho;
        }
    };

    dibujar(s1, usarHameha ? Qt::cyan : Qt::blue, 50, 25);
    dibujar(s2, usarHameha ? Qt::magenta : Qt::yellow, 40, 25);
    dibujar(s3, usarHameha ? Qt::green : Qt::red, 60, 25);

    if (mostrarHitbox && hitboxActivo) {
        QPen pen(Qt::red, 2, Qt::DashLine);
        p->setPen(pen);
        p->setBrush(Qt::NoBrush);
        float alto = altoTotal * 0.85f + 1.0f;
        float offsetY = altoTotal * 0.15f - 3.0f;
        p->drawRect(0, offsetY, anchoTotal, alto);
    }
}

// ==================== ANIMACIÓN ====================

void Rayo::iniciarAnimacion() {
    usarHameha = false;
    contadorAnimacion = 0;
    animacionTimer->start();
}

void Rayo::detenerAnimacion() {
    if (animacionTimer->isActive()) animacionTimer->stop();
}

void Rayo::alternarSprites() {
    usarHameha = !usarHameha;
    contadorAnimacion++;
    update();
}

// ==================== HITBOX ====================

QRectF Rayo::getHitbox() const {
    if (!activo || !hitboxActivo) return QRectF();
    float alto = altoTotal * 0.85f + 1.0f;
    float offsetY = altoTotal * 0.15f - 3.0f;
    return QRectF(pos().x(), pos().y() + offsetY, anchoTotal, alto);
}

void Rayo::verificarColisiones() {
    if (!scene() || !hitboxActivo) return;
    QRectF hitboxRayo = getHitbox();
    for (auto item : scene()->items(hitboxRayo)) {
        if (item == this) continue;
        if (colisionaCon(item)) procesarColision(item);
    }
}

bool Rayo::colisionaCon(QGraphicsItem* item) {
    if (!item || !hitboxActivo) return false;
    return getHitbox().intersects(item->sceneBoundingRect());
}

void Rayo::procesarColision(QGraphicsItem* item) {
    if (auto* goku = dynamic_cast<Goku*>(item)) {
        goku->recibirDanio(daño);
        destruir();
    }
}

void Rayo::alternarVisualizacionHitbox() {
    mostrarHitbox = !mostrarHitbox;
}
