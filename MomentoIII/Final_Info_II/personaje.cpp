#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QGraphicsPixmapItem>

#include "personaje.h"

Personaje::Personaje(QObject *parent)
    : QObject{parent}, QGraphicsPixmapItem()
{
    // Inicializar valores por defecto
    vida = 100;
    vidaMaxima = 100;
    velocidadMovimiento = 8;
    escalaSprite = 1.0;
    saltando = false;
    alturaOriginal = 0;
    moviendose = false;
    frameActual = 1;
    frameMaximo = 6; // Por defecto 6 frames de animación idle
    
    setFlag(QGraphicsItem::ItemIsMovable);

    // Configurar timer para salto
    jumpTimer = new QTimer(this);
    jumpTimer->setInterval(16); // ~60 FPS
    connect(jumpTimer, SIGNAL(timeout()), this, SLOT(actualizarSalto()));
    
    // Configurar timer para animación idle
    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200); // Cambiar frame cada 200ms
    connect(animacionTimer, SIGNAL(timeout()), this, SLOT(actualizarAnimacion()));
}

Personaje::~Personaje()
{
    if (jumpTimer) {
        jumpTimer->stop();
        delete jumpTimer;
    }
    if (animacionTimer) {
        animacionTimer->stop();
        delete animacionTimer;
    }
}

void Personaje::saltar()
{
    if (!saltando) {
        saltando = true;
        alturaOriginal = this->pos().y();
        velIn = 15; // velocidad inicial del salto
        tiempo = 0;
        jumpTimer->start();
    }
}

void Personaje::cambiarSprite(const QString& direccion)
{
    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);
    
    if (!nuevoSprite.isNull()) {
        // Escalar el sprite si es necesario
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
        }
        setPixmap(nuevoSprite);
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}

void Personaje::iniciarAnimacionIdle()
{
    moviendose = false;
    frameActual = 1; // Reiniciar desde el primer frame
    if (!saltando && estaVivo()) { // Solo iniciar si no está saltando y está vivo
        animacionTimer->start();
    }
}

void Personaje::establecerEscala(qreal escala)
{
    escalaSprite = escala;
    // Aplicar la escala al sprite actual si existe
    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/base" + QString::number(frameActual) + ".png";
    QPixmap spriteActual(rutaSprite);
    
    if (!spriteActual.isNull()) {
        if (escala != 1.0) {
            spriteActual = spriteActual.scaled(
                spriteActual.width() * escala,
                spriteActual.height() * escala,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
        }
        setPixmap(spriteActual);
    }
}

void Personaje::morir()
{
    vida = 0;
    animacionTimer->stop();
    jumpTimer->stop();
    moviendose = false;
    saltando = false;
    
    // Aquí podrías cambiar a un sprite de muerte
    // cambiarSprite("muerto");
    
    emit personajeMuerto(this);
    emit vidaCambiada(vida, vidaMaxima);
}

void Personaje::establecerVida(int vidaMaxima)
{
    this->vidaMaxima = vidaMaxima;
    this->vida = vidaMaxima;
    emit vidaCambiada(vida, this->vidaMaxima);
}

void Personaje::establecerVelocidad(qreal velocidad)
{
    this->velocidadMovimiento = velocidad;
}

void Personaje::establecerNombre(const QString& nombre)
{
    this->nombre = nombre;
}

void Personaje::establecerCarpetaSprites(const QString& carpeta)
{
    this->carpetaSprites = carpeta;
}

void Personaje::actualizarSalto()
{
    if (saltando) {
        // Física del salto: y = y0 + v0*t - 0.5*g*t^2
        qreal gravedad = 9.8;
        qreal nuevaY = alturaOriginal - (velIn * tiempo - 0.5 * gravedad * tiempo * tiempo);
        
        setPos(this->pos().x(), nuevaY);
        tiempo += 0.05; // incremento de tiempo
        
        // Si el personaje ha vuelto al suelo o más abajo
        if (nuevaY >= alturaOriginal) {
            setPos(this->pos().x(), alturaOriginal);
            saltando = false;
            jumpTimer->stop();
            tiempo = 0;
            
            // Reiniciar animación idle si no se está moviendo
            if (!moviendose) {
                iniciarAnimacionIdle();
            }
        }
    }
}

void Personaje::actualizarAnimacion()
{
    if (!moviendose && !saltando && estaVivo()) {
        // Ciclar entre los frames de la animación idle
        frameActual++;
        if (frameActual > frameMaximo) {
            frameActual = 1;
        }
        
        QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/base" + QString::number(frameActual) + ".png";
        QPixmap nuevoSprite(rutaSprite);
        
        if (!nuevoSprite.isNull()) {
            // Escalar el sprite si es necesario
            if (escalaSprite != 1.0) {
                nuevoSprite = nuevoSprite.scaled(
                    nuevoSprite.width() * escalaSprite,
                    nuevoSprite.height() * escalaSprite,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                );
            }
            setPixmap(nuevoSprite);
        } else {
            qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
        }
    }
}
