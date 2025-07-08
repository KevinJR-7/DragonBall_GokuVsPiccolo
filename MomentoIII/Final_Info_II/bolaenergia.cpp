#include "bolaenergia.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <cmath>

// Variable estática para mostrar hitbox
bool BolaEnergia::mostrarHitbox = false;

BolaEnergia::BolaEnergia(QObject *parent)
    : Habilidad(parent)
    , x(0), y(0)
    , dirX(1), dirY(0)
    , velocidad(12.0)  // Más rápida que el Kamehameha
    , alcance(500.0)
    , distanciaRecorrida(0)
    , activo(false)
    , spritesValidos(false)
    , ancho(50)
    , alto(50)
    , frameActual(1)
    , daño(25)  // Menos daño que el Kamehameha
    , hitboxActivo(false)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BolaEnergia::actualizar_timeout);
    
    // Timer para la animación de frames
    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(100); // Cambiar frame cada 100ms
    connect(animacionTimer, &QTimer::timeout, this, &BolaEnergia::siguienteFrame);
    
    cargarSprites();
    
    qDebug() << "BolaEnergia creada";
}

BolaEnergia::~BolaEnergia()
{
    if (timer) {
        timer->stop();
    }
    if (animacionTimer) {
        animacionTimer->stop();
    }
    qDebug() << "BolaEnergia destruida";
}

void BolaEnergia::cargarSprites()
{
    // Cargar sprites de la bola de energía
    bola1 = QPixmap(":/Goku/Sprites/goku/bola1.png");
    bola2 = QPixmap(":/Goku/Sprites/goku/bola2.png");
    bola3 = QPixmap(":/Goku/Sprites/goku/bola3.png");
    bola4 = QPixmap(":/Goku/Sprites/goku/bola4.png");
    bola5 = QPixmap(":/Goku/Sprites/goku/bola5.png");
    
    // Verificar si los sprites se cargaron correctamente
    bool bolasValidas = !bola1.isNull() && !bola2.isNull() && !bola3.isNull() && 
                        !bola4.isNull() && !bola5.isNull();
    
    if (!bolasValidas) {
        qDebug() << "Advertencia: Algunos sprites de bola de energía no se cargaron, usando placeholders";
        
        // Crear placeholders circulares de diferentes colores
        if (bola1.isNull()) {
            bola1 = QPixmap(40, 40);
            bola1.fill(Qt::blue);
        }
        if (bola2.isNull()) {
            bola2 = QPixmap(40, 40);
            bola2.fill(Qt::cyan);
        }
        if (bola3.isNull()) {
            bola3 = QPixmap(40, 40);
            bola3.fill(Qt::yellow);
        }
        if (bola4.isNull()) {
            bola4 = QPixmap(40, 40);
            bola4.fill(Qt::white);
        }
        if (bola5.isNull()) {
            bola5 = QPixmap(40, 40);
            bola5.fill(Qt::magenta);
        }
        
        spritesValidos = false;
    } else {
        spritesValidos = true;
        qDebug() << "Sprites de bola de energía cargados correctamente";
    }
    
    // Usar el tamaño del primer sprite como referencia
    ancho = bola1.width();
    alto = bola1.height();
}

void BolaEnergia::crear(float x, float y, float dirX, float dirY, float velocidad, float alcance)
{
    this->x = x;
    this->y = y;
    this->dirX = dirX;
    this->dirY = dirY;
    this->velocidad = velocidad;
    this->alcance = alcance;
    this->distanciaRecorrida = 0;
    this->activo = true;
    
    // Activar el hitbox
    hitboxActivo = true;
    objetosGolpeados.clear();
    
    // Posicionar el proyectil
    setPos(x, y);
    setZValue(100);
    
    // Iniciar el timer de actualización
    timer->start(16); // 60 FPS
    
    // Iniciar la animación de frames
    iniciarAnimacion();
    
    qDebug() << "BolaEnergia creada en posición:" << x << "," << y 
             << "dirección:" << dirX << "," << dirY 
             << "velocidad:" << velocidad;
}

void BolaEnergia::mover()
{
    if (!activo) return;
    
    // Mover el proyectil
    x += dirX * velocidad;
    y += dirY * velocidad;
    
    // Actualizar posición en la escena
    setPos(x, y);
    
    // Actualizar distancia recorrida
    distanciaRecorrida += velocidad;
    
    // Verificar colisiones si el hitbox está activo
    if (hitboxActivo) {
        verificarColisiones();
    }
    
    // Verificar si ha alcanzado el alcance máximo
    if (distanciaRecorrida >= alcance) {
        destruir();
        return;
    }
    
    // Verificar límites de pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenRect = screen->geometry();
        if (x < 0 || x > screenRect.width() || y < 0 || y > screenRect.height()) {
            destruir();
            return;
        }
    }
}

void BolaEnergia::actualizar()
{
    mover();
}

void BolaEnergia::actualizar_timeout()
{
    actualizar();
}

void BolaEnergia::dibujar(QPainter *painter)
{
    if (!activo) return;
}

bool BolaEnergia::estaActivo() const
{
    return activo;
}

void BolaEnergia::destruir()
{
    activo = false;
    timer->stop();
    
    // Detener la animación
    detenerAnimacion();
    
    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
    }
    
    qDebug() << "BolaEnergia destruida";
}

QRectF BolaEnergia::boundingRect() const
{
    return QRectF(0, 0, ancho, alto);
}

void BolaEnergia::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!activo) {
        return;
    }
    
    // Seleccionar el sprite según el frame actual
    QPixmap spriteActual;
    switch (frameActual) {
        case 1: spriteActual = bola1; break;
        case 2: spriteActual = bola2; break;
        case 3: spriteActual = bola3; break;
        case 4: spriteActual = bola4; break;
        case 5: spriteActual = bola5; break;
        default: spriteActual = bola1; break;
    }
    
    // Dibujar el sprite
    if (!spriteActual.isNull()) {
        painter->drawPixmap(0, 0, spriteActual);
    } else {
        // Fallback: dibujar círculo de color según el frame
        QColor colores[] = {Qt::blue, Qt::cyan, Qt::yellow, Qt::white, Qt::magenta};
        painter->setBrush(colores[frameActual - 1]);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(0, 0, ancho, alto);
    }
    
    // Dibujar hitbox si está habilitado
    if (mostrarHitbox && hitboxActivo) {
        QPen penHitbox(Qt::red, 2, Qt::DashLine);
        painter->setPen(penHitbox);
        painter->setBrush(Qt::NoBrush);
        
        painter->drawEllipse(5, 5, ancho - 10, alto - 10); // Hitbox un poco más pequeño
        qDebug() << "Hitbox de BolaEnergia dibujado";
    }
}

void BolaEnergia::iniciarAnimacion()
{
    frameActual = 1;
    animacionTimer->start();
    qDebug() << "Animación de BolaEnergia iniciada";
}

void BolaEnergia::detenerAnimacion()
{
    if (animacionTimer->isActive()) {
        animacionTimer->stop();
        qDebug() << "Animación de BolaEnergia detenida";
    }
}

void BolaEnergia::siguienteFrame()
{
    frameActual++;
    if (frameActual > 5) {
        frameActual = 1; // Volver al primer frame (animación cíclica)
    }
    
    // Forzar una actualización visual
    update();
}

// ==================== SISTEMA DE HITBOX ====================

QRectF BolaEnergia::getHitbox() const
{
    if (!activo || !hitboxActivo) {
        return QRectF();
    }
    
    // Hitbox circular un poco más pequeño que el sprite
    QPointF posicionGlobal = pos();
    float margen = 5.0f;
    
    return QRectF(posicionGlobal.x() + margen, posicionGlobal.y() + margen, 
                  ancho - (margen * 2), alto - (margen * 2));
}

void BolaEnergia::verificarColisiones()
{
    if (!scene() || !hitboxActivo) return;
    
    // Obtener el hitbox de la bola de energía
    QRectF hitboxBola = getHitbox();
    
    // Obtener todos los items en la escena que colisionan con el hitbox
    QList<QGraphicsItem*> itemsColisionando = scene()->items(hitboxBola);
    
    for (QGraphicsItem* item : itemsColisionando) {
        // No colisionar consigo mismo
        if (item == this) continue;
        
        // No golpear objetos que ya fueron golpeados
        if (objetosGolpeados.contains(item)) continue;
        
        // Verificar si realmente colisiona
        if (colisionaCon(item)) {
            procesarColision(item);
            objetosGolpeados.append(item);
            
            // Las bolas de energía se destruyen al golpear algo
            destruir();
            break;
        }
    }
}

bool BolaEnergia::colisionaCon(QGraphicsItem* item)
{
    if (!item || !hitboxActivo) return false;
    
    // Obtener los rectángulos de colisión
    QRectF hitboxBola = getHitbox();
    QRectF boundingRectItem = item->sceneBoundingRect();
    
    // Verificar intersección
    return hitboxBola.intersects(boundingRectItem);
}

void BolaEnergia::procesarColision(QGraphicsItem* item)
{
    qDebug() << "¡BolaEnergia colisionó con un objeto!";
    
    // Aquí puedes agregar lógica específica para diferentes tipos de objetos
    qDebug() << "Procesando colisión con objeto en posición:" << item->pos();
    qDebug() << "BolaEnergia causó" << daño << "puntos de daño";
}

void BolaEnergia::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
    qDebug() << "Visualización de hitbox de BolaEnergia:" << (mostrarHitbox ? "ACTIVADA" : "DESACTIVADA");
}

void BolaEnergia::iniciar(QPointF posicionInicial, QPointF direccion)
{
    qDebug() << "BolaEnergia::iniciar llamado con posición:" << posicionInicial << "dirección:" << direccion;
    
    // Convertir a nuestro sistema de coordenadas
    float x = posicionInicial.x();
    float y = posicionInicial.y();
    
    // Normalizar la dirección
    float magnitude = sqrt(direccion.x() * direccion.x() + direccion.y() * direccion.y());
    float dirX = (magnitude > 0) ? direccion.x() / magnitude : 1.0f;
    float dirY = (magnitude > 0) ? direccion.y() / magnitude : 0.0f;
    
    // Usar valores por defecto para velocidad y alcance
    float velocidad = 12.0f;
    float alcance = 500.0f;
    
    // Llamar al método crear existente
    crear(x, y, dirX, dirY, velocidad, alcance);
}

void BolaEnergia::detener()
{
    qDebug() << "BolaEnergia::detener llamado";
    destruir();
}
