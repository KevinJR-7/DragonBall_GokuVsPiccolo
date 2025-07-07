#include "kamehameha.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QDesktopWidget>
#include <cmath>

Kamehameha::Kamehameha(QObject *parent)
    : Habilidad(parent)
    , x(0), y(0)
    , dirX(1), dirY(0)
    , velocidad(8.0)
    , alcance(800.0)
    , distanciaRecorrida(0)
    , activo(false)
    , spritesValidos(false)
    , anchoTotal(150)
    , altoTotal(50)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Kamehameha::actualizar_timeout);
    
    cargarSprites();
    
    qDebug() << "Kamehameha creado como proyectil simple";
}

Kamehameha::~Kamehameha()
{
    if (timer) {
        timer->stop();
    }
    qDebug() << "Kamehameha destruido";
}

void Kamehameha::cargarSprites()
{
    // Cargar sprites del Kamehameha
    hame1 = QPixmap(":/Goku/Sprites/goku/hame1.png");
    hame2 = QPixmap(":/Goku/Sprites/goku/hame2.png");
    hame3 = QPixmap(":/Goku/Sprites/goku/hame3.png");
    
    // Verificar si los sprites se cargaron correctamente
    if (hame1.isNull() || hame2.isNull() || hame3.isNull()) {
        qDebug() << "Advertencia: Algunos sprites del Kamehameha no se cargaron, usando placeholders";
        if (hame1.isNull()) {
            hame1 = QPixmap(50, 25);
            hame1.fill(Qt::blue);
        }
        if (hame2.isNull()) {
            hame2 = QPixmap(40, 25);
            hame2.fill(Qt::yellow);
        }
        if (hame3.isNull()) {
            hame3 = QPixmap(60, 25);
            hame3.fill(Qt::red);
        }
        spritesValidos = false;
    } else {
        spritesValidos = true;
        qDebug() << "Sprites del Kamehameha cargados correctamente";
    }
    
    // Calcular dimensiones del rayo compuesto
    anchoTotal = hame1.width() + hame2.width() + hame3.width();
    altoTotal = qMax(hame1.height(), qMax(hame2.height(), hame3.height()));
}

void Kamehameha::crear(float x, float y, float dirX, float dirY, float velocidad, float alcance)
{
    this->x = x;
    this->y = y;
    this->dirX = dirX;
    this->dirY = dirY;
    this->velocidad = velocidad;
    this->alcance = alcance;
    this->distanciaRecorrida = 0;
    this->activo = true;
    
    // Posicionar el proyectil
    setPos(x, y);
    setZValue(100);
    
    // Iniciar el timer de actualización
    timer->start(16); // 60 FPS
    
    qDebug() << "Kamehameha creado en posición:" << x << "," << y 
             << "dirección:" << dirX << "," << dirY 
             << "velocidad:" << velocidad;
}

void Kamehameha::mover()
{
    if (!activo) return;
    
    // Mover el proyectil
    x += dirX * velocidad;
    y += dirY * velocidad;
    
    // Actualizar posición en la escena
    setPos(x, y);
    
    // Actualizar distancia recorrida
    distanciaRecorrida += velocidad;
    
    // Verificar si ha alcanzado el alcance máximo
    if (distanciaRecorrida >= alcance) {
        destruir();
        return;
    }
    
    // Verificar límites de pantalla
    if (x < -200 || x > 1200 || y < -200 || y > 800) {
        destruir();
        return;
    }
}

void Kamehameha::actualizar()
{
    mover();
}

void Kamehameha::actualizar_timeout()
{
    actualizar();
}

void Kamehameha::dibujar(QPainter *painter)
{
    if (!activo) return;
    
    // El dibujo real se hace en paint()
}

void Kamehameha::construirRayo()
{
    // Método para construir el rayo compuesto
    // La lógica de dibujo está en paint()
}

bool Kamehameha::estaActivo() const
{
    return activo;
}

void Kamehameha::destruir()
{
    activo = false;
    timer->stop();
    
    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
    }
    
    qDebug() << "Kamehameha destruido";
}

QRectF Kamehameha::boundingRect() const
{
    return QRectF(0, 0, anchoTotal, altoTotal);
}

void Kamehameha::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!activo) {
        return;
    }
    
    qreal posX = 0;
    
    // Dibujar hame1 (inicio del rayo)
    if (!hame1.isNull()) {
        painter->drawPixmap(posX, 0, hame1);
        posX += hame1.width();
    } else {
        painter->fillRect(posX, 0, 50, 25, Qt::blue);
        posX += 50;
    }
    
    // Dibujar hame2 (cuerpo del rayo)
    if (!hame2.isNull()) {
        painter->drawPixmap(posX, 0, hame2);
        posX += hame2.width();
    } else {
        painter->fillRect(posX, 0, 40, 25, Qt::yellow);
        posX += 40;
    }
    
    // Dibujar hame3 (final del rayo)
    if (!hame3.isNull()) {
        painter->drawPixmap(posX, 0, hame3);
    } else {
        painter->fillRect(posX, 0, 60, 25, Qt::red);
    }
}

void Kamehameha::establecerPotencia(int potencia)
{
    Q_UNUSED(potencia)
    // Método para futura implementación
}

void Kamehameha::establecerSegmentos(int segmentos)
{
    Q_UNUSED(segmentos)
    // Método para futura implementación
}
