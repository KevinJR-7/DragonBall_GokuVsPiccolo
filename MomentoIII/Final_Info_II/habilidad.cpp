#include "habilidad.h"
#include <QGraphicsScene>

Habilidad::Habilidad(QObject *parent)
    : QObject(parent), QGraphicsItem()
{
    // Inicializar propiedades por defecto
    velocidadHabilidad = 10.0;
    danoHabilidad = 10;
    alcanceMaximo = 800.0;
    distanciaRecorrida = 0.0;
    activa = false;
    escenaJuego = nullptr;
    
    // Crear timer para física
    timerActualizacion = new QTimer(this);
    timerActualizacion->setInterval(16); // ~60 FPS
    connect(timerActualizacion, &QTimer::timeout, this, &Habilidad::actualizarFisica);
    
    qDebug() << "Habilidad creada con valores por defecto";
}

Habilidad::~Habilidad()
{
    if (timerActualizacion && timerActualizacion->isActive()) {
        timerActualizacion->stop();
    }
    qDebug() << "Habilidad destruida";
}

void Habilidad::establecerVelocidad(qreal velocidad)
{
    velocidadHabilidad = velocidad;
    qDebug() << "Velocidad de habilidad establecida a:" << velocidad;
}

void Habilidad::establecerDano(int dano)
{
    danoHabilidad = dano;
    qDebug() << "Daño de habilidad establecido a:" << dano;
}

void Habilidad::establecerAlcance(qreal alcance)
{
    alcanceMaximo = alcance;
    qDebug() << "Alcance de habilidad establecido a:" << alcance;
}

void Habilidad::establecerEscena(QGraphicsScene* escena)
{
    escenaJuego = escena;
    if (escena) {
        escena->addItem(this);
        qDebug() << "Habilidad agregada a la escena";
    }
}

void Habilidad::actualizarFisica()
{
    if (!activa) return;
    
    // Mover en la dirección establecida
    qreal deltaX = direccionMovimiento.x() * velocidadHabilidad;
    qreal deltaY = direccionMovimiento.y() * velocidadHabilidad;
    
    setPos(pos().x() + deltaX, pos().y() + deltaY);
    
    // Actualizar distancia recorrida
    distanciaRecorrida += qSqrt(deltaX * deltaX + deltaY * deltaY);
    
    // Verificar si llegó al alcance máximo
    if (distanciaRecorrida >= alcanceMaximo) {
        qDebug() << "Habilidad alcanzó su alcance máximo:" << alcanceMaximo;
        detener();
        emit habilidadTerminada(this);
        return;
    }
    
    // Verificar límites de pantalla
    verificarLimites();
    
    // Llamar actualización específica de la habilidad
    actualizar();
}

void Habilidad::verificarLimites()
{
    if (!escenaJuego) return;
    
    QRectF limites = escenaJuego->sceneRect();
    QPointF posicion = pos();
    
    // Si la habilidad sale completamente de la pantalla, terminarla
    if (posicion.x() < limites.left() - 100 || 
        posicion.x() > limites.right() + 100 ||
        posicion.y() < limites.top() - 100 || 
        posicion.y() > limites.bottom() + 100) {
        
        qDebug() << "Habilidad salió de los límites de la pantalla";
        detener();
        emit habilidadTerminada(this);
    }
}
