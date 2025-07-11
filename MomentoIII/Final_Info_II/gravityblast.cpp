#include "gravityblast.h"
#include "piccolo.h" // Necesario para verificar colisiones con Piccolo
#include <QGraphicsScene>
#include <QDebug>
#include <QLineF> // Para calcular distancia y dirección
#include <QVector2D>

// Variable estática para mostrar hitbox
bool GravityBlast::mostrarHitbox = false;

GravityBlast::GravityBlast(QObject *parent)
    : Habilidad(parent)
    , frameActual(1)
    , objetivoGoku(nullptr)
    , fuerzaGravitacional(0.3) // Ajusta esta fuerza según sea necesario
    , velocidadMaxima(7.0) // Ajusta la velocidad máxima
    , velocidadActual(0.0, 0.0) // Inicialmente sin velocidad (QVector2D)
    , dano(5) // Daño de la habilidad, similar a BlastB
    , hitboxActivo(true)
{
    // Inicializar hitbox - Similar a BlastB, ajusta el tamaño según tu sprite
    hitbox = QRectF(0, 0, 16, 16); // Tamaño por defecto, se ajustará en actualizarHitbox()

    // Configurar timer de animación
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(80); // 80ms por frame (animación rápida), similar a BlastB
    connect(timerAnimacion, &QTimer::timeout, this, &GravityBlast::actualizarAnimacion);

    // Cargar sprites
    cargarSprites();

    // Configurar parámetros físicos por defecto para Habilidad base
    establecerVelocidad(0.0); // La velocidad base de Habilidad no se usará directamente para el movimiento
    establecerDano(dano); // Usar el daño definido para la instancia
    establecerAlcance(600.0); // Alcance máximo para que no dure infinitamente
    setZValue(1); // Asegurar que se dibuje encima de otros elementos

    setScale(2);

    qDebug() << "GravityBlast creado.";
}


GravityBlast::~GravityBlast()
{
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
    qDebug() << "GravityBlast destruido.";
    // deleteLater() se llamará en destruir()
}

void GravityBlast::cargarSprites()
{
    // Cargar sprites para la animación (asegúrate de que estas rutas sean correctas)
    // Puedes usar los mismos sprites que BlastB o crear unos nuevos
    spriteGravityBlast1 = QPixmap(":/Piccolo/Sprites/piccolo/blastg1.png");
    spriteGravityBlast2 = QPixmap(":/Piccolo/Sprites/piccolo/blastg2.png");
    spriteGravityBlast3 = QPixmap(":/Piccolo/Sprites/piccolo/blastg3.png");
    spriteGravityBlast4 = QPixmap(":/Piccolo/Sprites/piccolo/blastg4.png");

    if (spriteGravityBlast1.isNull() || spriteGravityBlast2.isNull() ||
        spriteGravityBlast3.isNull() || spriteGravityBlast4.isNull()) {
        qDebug() << "Error: No se pudieron cargar algunos sprites de GravityBlast";
    }

    spriteActual = spriteGravityBlast1; // Sprite inicial
}

void GravityBlast::iniciar(QPointF posicionInicial, QPointF direccion)
{
    setPos(posicionInicial);
    // Establecer la dirección inicial (aunque la gravedad la cambiará constantemente)
    direccionMovimiento = direccion; // Heredado de Habilidad
    direccionMovimientoQVector = QVector2D(direccion); // Para cálculos de velocidad

    activa = true;
    timerActualizacion->start(); // Inicia el timer de física de Habilidad
    timerAnimacion->start();     // Inicia el timer de animación
}

void GravityBlast::actualizar()
{
    if (!objetivoGoku) {
        qDebug() << "Error: No hay objetivo Goku para GravityBlast.";
        detener();
        return;
    }

    // 1. Obtener la posición actual del proyectil y del objetivo
    QPointF posActual = pos();
    QPointF posObjetivo = objetivoGoku->pos() + QPointF(0, 100); // Asumiendo que Goku::pos() devuelve su posición

    // 2. Calcular el vector dirección hacia el objetivo
    QVector2D direccionHaciaObjetivo = QVector2D(posObjetivo - posActual);

    // 3. Normalizar el vector dirección
    if (direccionHaciaObjetivo.length() > 0) {
        direccionHaciaObjetivo.normalize();
    } else {
        // Si el proyectil está exactamente sobre Goku, no aplicar fuerza de atracción
        // o aplicar una pequeña fuerza para que se aleje un poco si se desea
        detener(); // Opcional: Detener si ya alcanzó al objetivo
        return;
    }

    // 4. Aplicar la fuerza gravitacional para ajustar la velocidad
    velocidadActual += direccionHaciaObjetivo * fuerzaGravitacional;

    // 5. Limitar la velocidad máxima
    if (velocidadActual.length() > velocidadMaxima) {
        velocidadActual.normalize();
        velocidadActual *= velocidadMaxima;
    }

    // 6. Actualizar la `direccionMovimiento` de Habilidad con la nueva `velocidadActual`
    // Esto es para el cálculo de distancia, aunque el movimiento se hace en actualizarFisica()
    direccionMovimiento = velocidadActual.toPointF();
}

void GravityBlast::actualizarFisica()
{
    if (!activa) return;

    // 1. Llamar al método `actualizar` para calcular la fuerza gravitacional y velocidad
    actualizar();

    // 2. Mover en la dirección y velocidad calculada
    setPos(pos() + velocidadActual.toPointF());

    // Actualizar distancia recorrida (para el alcance máximo de Habilidad)
    // Aunque la trayectoria sea curva, la distancia acumulada es útil
    distanciaRecorrida += velocidadActual.length();

    // Verificar si llegó al alcance máximo
    if (distanciaRecorrida >= alcanceMaximo) {
        qDebug() << "GravityBlast alcanzó su alcance máximo: " << alcanceMaximo;
        detener(); // Llama a detener, que luego llamará a destruir
        return;
    }

    // 3. Verificar límites de pantalla (heredado de Habilidad)
    verificarLimites();

    // 4. Actualizar hitbox después de mover el proyectil
    actualizarHitbox();

    // 5. Verificar colisiones
    verificarColisiones();
}

void GravityBlast::detener()
{
    activa = false;
    timerActualizacion->stop();
    timerAnimacion->stop();
    qDebug() << "GravityBlast detenido. Emitiendo habilidadTerminada.";
    emit habilidadTerminada(this);
    destruir(); // Llama al método de limpieza final
}

void GravityBlast::destruir()
{
    //if (!activa) return; // Ya está inactivo o en proceso de destrucción

    activa = false;
    // Asegurarse de que los timers estén detenidos (ya deberían estarlo desde detener())
    if (timerAnimacion) timerAnimacion->stop();
    if (timerActualizacion) timerActualizacion->stop();

    objetosGolpeados.clear(); // Limpiar lista de objetos golpeados

    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
        qDebug() << "GravityBlast removido de la escena.";
    }

    // Programar eliminación segura del objeto
    deleteLater();

    qDebug() << "GravityBlast destruido completamente.";
}

QRectF GravityBlast::boundingRect() const
{
    // Devolver el bounding rect del sprite actual
    // Si los sprites tienen diferentes tamaños, ajusta aquí
    return spriteActual.rect();
}

void GravityBlast::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!spriteActual.isNull()) {
        painter->drawPixmap(0, 0, spriteActual);
    } else {
        // Dibujar un círculo temporal si no se cargó ningún sprite
        painter->setBrush(QBrush(Qt::magenta)); // Puedes elegir otro color
        painter->setPen(QPen(Qt::darkMagenta, 2));
        painter->drawEllipse(0, 0, 20, 20); // Tamaño del círculo, similar a BlastB
    }

    if (mostrarHitbox && hitboxActivo) { // Si tienes una forma de activar/desactivar el debug de hitbox
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(QBrush(Qt::transparent));
        painter->drawRect(hitbox);
    }
}

void GravityBlast::establecerObjetivo(Goku* objetivo)
{
    objetivoGoku = objetivo;
    qDebug() << "Objetivo Goku establecido para GravityBlast.";
}

void GravityBlast::actualizarHitbox()
{
    // Actualizar posición del hitbox relativa al sprite
    // Puedes ajustar estos valores para que el hitbox se ajuste a tu sprite
    QRectF bounds = boundingRect();
    // Estos valores son un ejemplo; ajústalos para que el hitbox sea preciso para tu sprite
    // Para una BlastB típica, un hitbox pequeño centrado es común.
    hitbox = QRectF(bounds.x() + (bounds.width() - 16) / 2, bounds.y() + (bounds.height() - 16) / 2, 16, 16);
}

void GravityBlast::verificarColisiones()
{

    // Obtener hitbox en coordenadas de la escena
    QRectF hitboxEscena = mapRectToScene(hitbox);

    // Buscar colisiones con otros objetos
    QList<QGraphicsItem*> itemsColision = scene()->items(hitboxEscena);

    for (QGraphicsItem* item : itemsColision) {
        if (item == this) continue; // Ignorar a sí mismo

        // Verificar si ya golpeó este objeto
        if (objetosGolpeados.contains(item)) continue;

        // Verificar si el item es un Personaje (Goku, Piccolo, etc.)
        // Asumimos que los Personajes son QGraphicsPixmapItem o se pueden castear
        if (Personaje* personaje = dynamic_cast<Personaje*>(item)) {
            // Evitar colisiones con quien lanza el ataque (Piccolo)
            if (personaje == parent()) continue; // Suponiendo que Piccolo es el parent (QObject)

            // Marcar como golpeado
            objetosGolpeados.append(item);

            // Aplicar daño al personaje colisionado
            personaje->recibirDanio(dano); // Usa el valor de daño del GravityBlast

            detener(); // Detener el proyectil después de golpear
            return; // Terminar después de la primera colisión
        }
    }
}

void GravityBlast::actualizarAnimacion()
{
    frameActual++;
    if (frameActual > 4) { // Asumiendo 4 frames para la animación
        frameActual = 1;
    }

    switch (frameActual) {
    case 1: spriteActual = spriteGravityBlast1; break;
    case 2: spriteActual = spriteGravityBlast2; break;
    case 3: spriteActual = spriteGravityBlast3; break;
    case 4: spriteActual = spriteGravityBlast4; break;
    }
    update(); // Redibujar el ítem
}

void GravityBlast::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
    qDebug() << "Visualización de hitbox del GBlast:" << (mostrarHitbox ? "ACTIVADA" : "DESACTIVADA");
}
