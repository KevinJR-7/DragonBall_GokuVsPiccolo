#include "blastb.h"
#include "piccolo.h"
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>
#include <random>

// Variable estática para mostrar hitbox
bool BlastB::mostrarHitbox = false;

BlastB::BlastB(QObject *parent)
    : Habilidad(parent)
    , frameActual(1)
    , tiempoVida(0.0)
    , tiempoMaximo(10.0) // 10 segundos de vida máxima
{
    // Configurar timer de animación
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(80); // 80ms por frame (animación rápida)
    connect(timerAnimacion, &QTimer::timeout, this, &BlastB::actualizarAnimacion);
    
    // Cargar sprites
    cargarSprites();
    
    // Configurar parámetros del atractor de Lorenz por defecto
    configurarCaos();
    
    // Configurar parámetros físicos por defecto
    establecerVelocidad(5.0);
    establecerDano(10);
    establecerAlcance(800.0);
    
    // Inicializar hitbox - hitbox extremadamente delgado en altura
    hitbox = QRectF(0, 0, 10, 1); // Hitbox muy delgado en altura: 10x1 píxeles
    daño = 10;
    hitboxActivo = true;
    
    // Inicializar variables del atractor
    dt = 0.01; // Paso de tiempo pequeño para mayor precisión
    escala = 3.0; // Escala para convertir coordenadas del atractor a píxeles
    
    // Inicializar posición en el atractor con valores aleatorios pequeños
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    
    x = dis(gen);
    y = dis(gen);
    z = dis(gen);
    
    // Inicializar trayectoria - limitar a 100 puntos para evitar consumo excesivo de memoria
    maxPuntosTrayectoria = 100;
    trayectoria.clear();
    
    qDebug() << "BlastB creado con trayectoria caótica";
}

BlastB::~BlastB()
{
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
}

void BlastB::cargarSprites()
{
    // Cargar sprites para la animación
    spriteBlastB1 = QPixmap(":/Goku/Sprites/goku/blastb1.png");
    spriteBlastB2 = QPixmap(":/Goku/Sprites/goku/blastb2.png");
    spriteBlastB3 = QPixmap(":/Goku/Sprites/goku/blastb3.png");
    spriteBlastB4 = QPixmap(":/Goku/Sprites/goku/blastb4.png");
    
    // Verificar que los sprites se cargaron correctamente
    if (spriteBlastB1.isNull() || spriteBlastB2.isNull() || 
        spriteBlastB3.isNull() || spriteBlastB4.isNull()) {
        qDebug() << "Error: No se pudieron cargar algunos sprites de BlastB";
    }
    
    // Establecer sprite inicial
    spriteActual = spriteBlastB1;
}

void BlastB::configurarCaos(qreal sigma, qreal rho, qreal beta)
{
    this->sigma = sigma;
    this->rho = rho;
    this->beta = beta;
    
    qDebug() << "Parámetros del atractor de Lorenz configurados:"
             << "σ=" << sigma << "ρ=" << rho << "β=" << beta;
}

void BlastB::crear(qreal x, qreal y, qreal dirX, qreal dirY, qreal velocidad, qreal alcance)
{
    // Configurar posición inicial y dirección
    posicionBase = QPointF(x, y);
    
    // Normalizar dirección base manualmente
    QPointF dir(dirX, dirY);
    qreal magnitud = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
    if (magnitud > 0) {
        direccionBase = QPointF(dir.x() / magnitud, dir.y() / magnitud);
    } else {
        direccionBase = QPointF(1.0, 0.0); // Dirección por defecto hacia la derecha
    }
    
    // Configurar propiedades físicas
    establecerVelocidad(velocidad);
    establecerAlcance(alcance);
    
    // Configurar el tamaño del proyectil (más grande)
    qreal escala = 1.2; // Tamaño del proyectil más grande
    setScale(escala);
    
    // Establecer posición inicial
    setPos(posicionBase);
    
    qDebug() << "BlastB creado en posición:" << x << "," << y 
             << "con dirección:" << dirX << "," << dirY;
}

void BlastB::iniciar(QPointF posicionInicial, QPointF direccion)
{
    // Establecer posición y dirección
    posicionBase = posicionInicial;
    
    // Normalizar dirección manualmente
    qreal magnitud = qSqrt(direccion.x() * direccion.x() + direccion.y() * direccion.y());
    if (magnitud > 0) {
        direccionBase = QPointF(direccion.x() / magnitud, direccion.y() / magnitud);
    } else {
        direccionBase = QPointF(1.0, 0.0); // Dirección por defecto hacia la derecha
    }
    
    posicionInicial = posicionInicial;
    direccionMovimiento = direccion;
    
    // Resetear tiempo de vida
    tiempoVida = 0.0;
    distanciaRecorrida = 0.0;
    
    // Establecer posición inicial
    setPos(posicionBase);
    
    // Marcar como activa
    activa = true;
    
    // Iniciar timers
    timerAnimacion->start();
    timerActualizacion->start();
    
    qDebug() << "BlastB iniciado en posición:" << posicionInicial.x() << "," << posicionInicial.y();
}

void BlastB::actualizar()
{
    if (!activa) return;
    
    // Actualizar tiempo de vida
    tiempoVida += dt;
    
    // Verificar si debe terminar por tiempo
    if (tiempoVida >= tiempoMaximo) {
        detener();
        return;
    }
    
    // Actualizar posición usando el atractor de Lorenz
    calcularSiguientePosicion();
    
    // Verificar límites
    verificarLimites();
}

void BlastB::detener()
{
    if (!activa) return;
    
    activa = false;
    
    // Detener timers
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
    if (timerActualizacion) {
        timerActualizacion->stop();
    }
    
    qDebug() << "BlastB detenido";
    
    // Emitir señal de terminación
    emit habilidadTerminada(this);
    
    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
    }
    
    // Programar eliminación
    deleteLater();
}

void BlastB::calcularSiguientePosicion()
{
    // Actualizar el atractor de Lorenz
    actualizarAtractorLorenz();
    
    // Obtener la posición caótica
    QPointF posicionCaotica = obtenerPosicionCaotica();
    
    // Combinar movimiento base con trayectoria caótica
    QPointF movimientoBase = direccionBase * velocidadHabilidad;
    QPointF posicionFinal = posicionBase + movimientoBase + posicionCaotica;
    
    // Actualizar posición base para el siguiente frame
    posicionBase += movimientoBase * 0.1; // Movimiento base más lento
    
    // Establecer nueva posición
    setPos(posicionFinal);
    
    // Agregar punto a la trayectoria para visualización
    trayectoria.append(posicionFinal);
    
    // Limitar el número de puntos para evitar consumo excesivo de memoria
    if (trayectoria.size() > maxPuntosTrayectoria) {
        trayectoria.removeFirst();
    }
    
    // Actualizar distancia recorrida
    distanciaRecorrida += velocidadHabilidad * 0.1;
}

void BlastB::actualizarAtractorLorenz()
{
    // Ecuaciones diferenciales del atractor de Lorenz:
    // dx/dt = σ(y - x)
    // dy/dt = x(ρ - z) - y  
    // dz/dt = xy - βz
    
    dx = sigma * (y - x);
    dy = x * (rho - z) - y;
    dz = x * y - beta * z;
    
    // Integración usando método de Euler
    x += dx * dt;
    y += dy * dt;
    z += dz * dt;
}

QPointF BlastB::obtenerPosicionCaotica()
{
    // Convertir coordenadas del atractor a desplazamiento en pantalla
    qreal desplazamientoX = x * escala;
    qreal desplazamientoY = y * escala;
    
    return QPointF(desplazamientoX, desplazamientoY);
}

void BlastB::actualizarAnimacion()
{
    if (!activa) return;
    
    // Cambiar al siguiente frame
    frameActual++;
    if (frameActual > 4) {
        frameActual = 1;
    }
    
    // Actualizar sprite según el frame
    switch (frameActual) {
        case 1:
            spriteActual = spriteBlastB1;
            break;
        case 2:
            spriteActual = spriteBlastB2;
            break;
        case 3:
            spriteActual = spriteBlastB3;
            break;
        case 4:
            spriteActual = spriteBlastB4;
            break;
    }
    
    // Forzar repintado
    update();
}

void BlastB::actualizarFisica()
{
    // Llamar al método de actualización general
    actualizar();
    
    // Actualizar hitbox después de cambiar posición
    actualizarHitbox();
    
    // Verificar colisiones después de actualizar posición
    verificarColisiones();
}

// ==================== MÉTODOS DEL HITBOX ====================

void BlastB::actualizarHitbox()
{
    // Actualizar posición del hitbox relativa al sprite - reducir extremadamente la altura
    QRectF bounds = boundingRect();
    hitbox = QRectF(bounds.x() + 4, bounds.y() + 16, bounds.width() - 8, bounds.height() - 32);
}

void BlastB::verificarColisiones()
{
    if (!hitboxActivo || !scene()) {
        return;
    }
    
    // Obtener hitbox en coordenadas de la escena
    QRectF hitboxEscena = mapRectToScene(hitbox);
    
    // Buscar colisiones con otros objetos
    QList<QGraphicsItem*> itemsColision = scene()->items(hitboxEscena);
    
    for (QGraphicsItem* item : itemsColision) {
        if (item == this) continue; // Ignorar a sí mismo
        
        // Verificar si ya golpeó este objeto
        if (objetosGolpeados.contains(item)) continue;
        
        // Verificar si es un personaje u otro objeto colisionable
        if (item->type() == QGraphicsPixmapItem::Type) {
            // Marcar como golpeado
            objetosGolpeados.append(item);
            
            qDebug() << "BlastB colisionó con objeto, daño:" << daño;
            
            // Si el item es Piccolo, aplicar daño
            if (Piccolo* piccolo = dynamic_cast<Piccolo*>(item)) {
                piccolo->recibirDanio(daño); // Usa el valor de daño del Blast
                qDebug() << "Blast causó" << daño << "puntos de daño a Piccolo";
                destruir();
                return;
            }
            // Por ejemplo: static_cast<Personaje*>(item)->recibirDanio(daño);
        }
    }
}

void BlastB::destruir()
{
    //activo = false;
    timerAnimacion->stop();

    // Detener la animación intercalada
    //detenerAnimacion();

    // Limpiar trayectoria
    trayectoria.clear();

    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
    }

    qDebug() << "Blast destruido";
}

QRectF BlastB::obtenerHitbox() const
{
    return hitbox;
}

void BlastB::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
    qDebug() << "Visualización hitbox BlastB:" << (mostrarHitbox ? "ON" : "OFF");
}

// ==================== MÉTODOS DE QGRAPHICSITEM ====================

QRectF BlastB::boundingRect() const
{
    if (!spriteActual.isNull()) {
        return spriteActual.rect();
    }
    return QRectF(0, 0, 16, 16); // Tamaño por defecto
}

void BlastB::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!spriteActual.isNull()) {
        painter->drawPixmap(0, 0, spriteActual);
    } else {
        // Dibujar un círculo temporal si no hay sprite
        painter->setBrush(QBrush(Qt::cyan));
        painter->setPen(QPen(Qt::blue, 2));
        painter->drawEllipse(0, 0, 16, 16);
    }
    
    // Dibujar hitbox si está activada la visualización
    if (mostrarHitbox && hitboxActivo) {
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(QBrush(Qt::transparent));
        painter->drawRect(hitbox);
        
        // Dibujar la línea de trayectoria completa
        if (trayectoria.size() > 1) {
            painter->setPen(QPen(Qt::yellow, 1, Qt::DashLine));
            
            // Obtener posición actual del proyectil en coordenadas de la escena
            QPointF posicionActual = pos();
            
            // Dibujar líneas conectando todos los puntos de la trayectoria
            for (int i = 0; i < trayectoria.size() - 1; ++i) {
                QPointF puntoInicio = mapFromScene(trayectoria[i]);
                QPointF puntoFin = mapFromScene(trayectoria[i + 1]);
                painter->drawLine(puntoInicio, puntoFin);
            }
            
            // Conectar el último punto de la trayectoria con la posición actual
            if (!trayectoria.isEmpty()) {
                QPointF ultimoPunto = mapFromScene(trayectoria.last());
                QPointF posicionLocal = mapFromScene(posicionActual);
                painter->drawLine(ultimoPunto, posicionLocal);
            }
        }
    }
}
