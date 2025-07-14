#include "kick.h"
#include "piccolo.h" // Necesario para verificar colisiones con Piccolo
#include <QGraphicsScene>
#include <QDebug>

// Variable estática para mostrar hitbox
bool Kick::mostrarHitbox = false;

Kick::Kick(QObject *parent)
    : Habilidad{parent}
    , frameActual(1)
    // , objetivoGoku(nullptr)
    , dano(10) // Daño de la habilidad, similar a BlastB
    , hitboxActivo(true)
{

    alcanceMaximo = 20*0.25;
    // Inicializar hitbox - Similar a BlastB, ajusta el tamaño según tu sprite
    hitbox = QRectF(0, 0, 500, 150); // Tamaño por defecto, se ajustará en actualizarHitbox()

    // Configurar timer de animación
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(50); // 80ms por frame (animación rápida), similar a BlastB
    connect(timerAnimacion, &QTimer::timeout, this, &Kick::actualizar);

    // Configurar parámetros físicos por defecto para Habilidad base
    establecerDano(dano); // Usar el daño definido para la instancia
    setZValue(1); // Asegurar que se dibuje encima de otros elementos

    setScale(1);
}

Kick::~Kick()
{
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
    qDebug() << "Kick destruido.";
    // deleteLater() se llamará en destruir()
}

void Kick::iniciar(QPointF posicionInicial, QPointF direccion)
{
    Q_UNUSED(direccion);
    setPos(posicionInicial);

    activa = true;
    timerAnimacion->start();     // Inicia el timer de animación
}

void Kick::actualizar()
{
    distanciaRecorrida += 1;
    if (distanciaRecorrida >= alcanceMaximo) {
        detener(); // Llama a detener, que luego llamará a destruir
        return;
    }
    verificarColisiones();
}

void Kick::detener()
{
    activa = false;
    timerAnimacion->stop();
    qDebug() << "Kick detenido. Emitiendo habilidadTerminada.";
    emit habilidadTerminada(this);
    destruir(); // Llama al método de limpieza final
}

void Kick::destruir()
{
    activa = false;
    // Asegurarse de que los timers estén detenidos (ya deberían estarlo desde detener())
    if (timerAnimacion) timerAnimacion->stop();

    objetosGolpeados.clear(); // Limpiar lista de objetos golpeados

    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
        qDebug() << "Kick removido de la escena.";
    }

    // Programar eliminación segura del objeto
    deleteLater();

    qDebug() << "Kick destruido completamente.";
}

void Kick::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QBrush(Qt::transparent));
    painter->setPen(QPen(Qt::transparent, 2));
    painter->drawRect(0, 0, 100, 100);

    if (mostrarHitbox && hitboxActivo) { // Si tienes una forma de activar/desactivar el debug de hitbox
        painter->setPen(QPen(Qt::red, 2));
        painter->drawRect(hitbox);
    }
}

// void Kick::establecerObjetivo(Goku* objetivo)
// {
//     objetivoGoku = objetivo;
//     qDebug() << "Objetivo Goku establecido para Kick.";
// }


void Kick::verificarColisiones()
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
        if (Personaje* personaje = dynamic_cast<Personaje*>(item)) {
            // Evitar colisiones con quien lanza el ataque (Piccolo)
            if (personaje == parent()) continue; // Suponiendo que Piccolo es el parent (QObject)

            // Marcar como golpeado
            objetosGolpeados.append(item);

            // Aplicar daño al personaje colisionado
            personaje->recibirDanio(dano); // Usa el valor de daño del Kick

            detener(); // Detener el proyectil después de golpear
            return; // Terminar después de la primera colisión
        }
    }
}

void Kick::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;

}

QRectF Kick::boundingRect() const
{
    return QRectF(0, 0, 100, 100); // Tamaño por defecto
}
