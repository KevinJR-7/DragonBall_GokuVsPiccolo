#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QPen>
#include <QBrush>

#include "personaje.h"

Personaje::Personaje(QObject *parent)
    : QObject{parent}, QGraphicsPixmapItem()
{
    // Inicializar valores por defecto de las propiedades del personaje.
    vida = 100;
    vidaMaxima = 100;
    velocidadMovimiento = 8;
    escalaSprite = 1.0;
    saltando = false;
    alturaOriginal = 0;
    moviendose = false;
    frameActual = 1;
    frameMaximo = 6;

    // Inicializar variables para la animación de salto.
    frameSaltoActual = 1;
    alturaMaximaAlcanzada = 0.0;

    // Inicializar dimensiones y offsets del hitbox.
    hitboxAncho = 30;
    hitboxAlto = 30;
    hitboxOffsetX = -10;
    hitboxOffsetY = 0;
    limitesEscena = QRectF(0, 0, 800, 600);

    // Inicializar la visualización del hitbox.
    hitboxVisible = false;
    hitboxVisual = nullptr;

    // Inicializar el almacenamiento de la trayectoria de salto.
    maxPuntosTrayectoriaSalto = 50;
    trayectoriaSalto.clear();

    // Inicializar variables de física de salto.
    velocidadVertical = 0.0;
    aceleracionVertical = 0.0;
    coeficienteResistencia = 0.1;
    masaPersonaje = 1.0;
    deltaT = 0.016;

    // Inicializar variables de movimiento horizontal durante el salto.
    posicionXSalto = 0.0;
    velocidadHorizontal = 0.0;
    velocidadHorizontalSalto = 0.0;
    offsetYSalto = 0.0;
    posicionBaseSalto = 0.0;

    setFlag(QGraphicsItem::ItemIsMovable);

    // Configurar el temporizador para las actualizaciones de la física de salto.
    jumpTimer = new QTimer(this);
    jumpTimer->setInterval(16);
    connect(jumpTimer, SIGNAL(timeout()), this, SLOT(actualizarSalto()));

    // Configurar el temporizador para las actualizaciones de la animación de inactividad (idle).
    baseFase = "/base";
    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200);
    connect(animacionTimer, SIGNAL(timeout()), this, SLOT(actualizarAnimacion()));
}

Personaje::~Personaje()
{
    // Detener y eliminar el temporizador de salto.
    if (jumpTimer) {
        jumpTimer->stop();
        delete jumpTimer;
    }
    // Detener y eliminar el temporizador de animación.
    if (animacionTimer) {
        animacionTimer->stop();
        delete animacionTimer;
    }
    // Eliminar el elemento visual del hitbox.
    if (hitboxVisual) {
        delete hitboxVisual;
        hitboxVisual = nullptr;
    }
}

void Personaje::saltar()
{
    // Inicia la secuencia de salto si el personaje no está ya saltando.
    if (!saltando) {
        saltando = true;
        alturaOriginal = this->pos().y();
        alturaMaximaAlcanzada = alturaOriginal;

        // Inicializar el seguimiento del movimiento de salto.
        posicionXSalto = this->pos().x();
        posicionBaseSalto = this->pos().y();
        offsetYSalto = 0.0;

        // Inicializar la física del salto.
        velocidadVertical = -velIn;
        aceleracionVertical = 0.0;
        tiempo = 0;
        frameSaltoActual = 1;

        // Aplicar el hitbox específico del salto.
        aplicarHitboxSalto();

        // Cambiar al primer sprite de salto.
        cambiarSprite("salto1");

        jumpTimer->start();
    }
}

void Personaje::cambiarSprite(const QString& direccion)
{
    // Guarda el centro actual del sprite, carga el nuevo sprite, lo escala y establece la nueva posición para mantener el centro.
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    if (!nuevoSprite.isNull()) {
        // Escalar el nuevo sprite.
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }
        setPixmap(nuevoSprite);

        // Recalcular la posición para mantener el centro.
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);
    } else {
        // Manejar el error de carga del sprite.
    }
}

void Personaje::cambiarSpriteCentrado(const QString& direccion)
{
    // Carga un nuevo sprite, lo escala y ajusta la posición para mantener el sprite centrado.
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    if (!nuevoSprite.isNull()) {
        // Escalar el nuevo sprite.
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }

        // Cambiar el sprite.
        setPixmap(nuevoSprite);

        // Calcular la nueva posición para mantener el centro.
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);

    } else {
        // Manejar el error de carga del sprite.
    }
}

void Personaje::iniciarAnimacionIdle()
{
    // Inicia la animación de inactividad (idle) si el personaje no se está moviendo o saltando y está vivo.
    moviendose = false;
    frameActual = 1;
    if (estaVivo()) {
        animacionTimer->start();
    }
}

void Personaje::establecerEscala(qreal escala)
{
    // Establece la escala del sprite y la aplica al sprite actual.
    escalaSprite = escala;
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
    // Maneja la muerte del personaje: establece la vida a 0, detiene los temporizadores, restablece los estados de movimiento/salto, restaura el hitbox y emite señales.
    vida = 0;
    animacionTimer->stop();
    jumpTimer->stop();
    moviendose = false;
    saltando = false;
    velocidadHorizontal = 0.0;

    // Restaurar el hitbox normal.
    restaurarHitboxNormal();

    emit personajeMuerto(this);
    emit vidaCambiada(vida, vidaMaxima);
}

void Personaje::establecerVida(int vidaMaxima)
{
    // Establece la vida máxima y actual del personaje.
    this->vidaMaxima = vidaMaxima;
    this->vida = vidaMaxima;
    emit vidaCambiada(vida, this->vidaMaxima);
}

void Personaje::establecerVelocidad(qreal velocidad)
{
    // Establece la velocidad de movimiento del personaje.
    this->velocidadMovimiento = velocidad;
}

void Personaje::establecerNombre(const QString& nombre)
{
    // Establece el nombre del personaje.
    this->nombre = nombre;
}

void Personaje::establecerCarpetaSprites(const QString& carpeta)
{
    // Establece la carpeta de sprites para las animaciones del personaje.
    this->carpetaSprites = carpeta;
}

void Personaje::establecerFisicaSalto(qreal masa, qreal resistencia)
{
    // Configura la masa y la resistencia del aire para la física del salto.
    masaPersonaje = masa;
    coeficienteResistencia = resistencia;
}

void Personaje::establecerVelocidadSalto(qreal velocidadInicial)
{
    // Establece la velocidad vertical inicial para el salto.
    velIn = velocidadInicial;
}

void Personaje::establecerVelocidadHorizontalSalto(qreal velocidadH)
{
    // Establece la velocidad horizontal inicial para el salto.
    velocidadHorizontal = velocidadH;
}

void Personaje::aplicarMovimientoHorizontal(qreal deltaX)
{
    // Aplica el movimiento horizontal, especialmente durante un salto, respetando los límites de la escena.
    if (saltando) {
        posicionXSalto += deltaX;

        // Aplicar límites de pantalla para el movimiento horizontal.
        if (posicionXSalto < limitesEscena.left()) {
            posicionXSalto = limitesEscena.left();
        } else if (posicionXSalto > limitesEscena.right() - hitboxAncho) {
            posicionXSalto = limitesEscena.right() - hitboxAncho;
        }
    }
}

void Personaje::setPos(qreal x, qreal y)
{
    // Establece la posición del QGraphicsPixmapItem.
    QGraphicsPixmapItem::setPos(x, y);
}

void Personaje::pausarTimerSalto()
{
    // Pausa el temporizador de salto si está activo.
    if (jumpTimer && jumpTimer->isActive()) {
        jumpTimer->stop();
    }
}

void Personaje::reanudarTimerSalto()
{
    // Reanuda el temporizador de salto si el personaje está actualmente saltando.
    if (jumpTimer && saltando) {
        jumpTimer->start();
    }
}

// ==================== MÉTODOS DE SALTO ====================

void Personaje::actualizarSalto()
{
    // Actualiza la posición y la animación del personaje durante un salto basándose en cálculos de física.
    if (saltando) {
        tiempo += deltaT;

        // Constantes físicas - gravedad reducida para un "salto épico de Goku".
        const qreal gravedad = 9.8 * 15;

        // Cálculo de la ecuación diferencial vertical.
        qreal fuerzaGravedad = gravedad;
        qreal fuerzaResistenciaVertical = coeficienteResistencia * velocidadVertical * qAbs(velocidadVertical) / masaPersonaje;

        // La resistencia del aire se opone al movimiento vertical.
        if (velocidadVertical < 0) {
            fuerzaResistenciaVertical = -fuerzaResistenciaVertical;
        }

        aceleracionVertical = fuerzaGravedad + fuerzaResistenciaVertical;

        // Integración numérica vertical.
        velocidadVertical += aceleracionVertical * deltaT;

        qreal deltaY = velocidadVertical * deltaT + 0.5 * aceleracionVertical * deltaT * deltaT;

        // Actualizar el offset vertical.
        offsetYSalto += deltaY;

        // Movimiento horizontal parabólico.
        qreal fuerzaResistenciaHorizontal = coeficienteResistencia * velocidadHorizontal * qAbs(velocidadHorizontal) / masaPersonaje;

        // La resistencia horizontal siempre se opone al movimiento.
        if (velocidadHorizontal > 0) {
            fuerzaResistenciaHorizontal = -fuerzaResistenciaHorizontal;
        } else if (velocidadHorizontal < 0) {
            fuerzaResistenciaHorizontal = -fuerzaResistenciaHorizontal;
        }

        // Actualizar la velocidad horizontal con la resistencia del aire.
        velocidadHorizontal += fuerzaResistenciaHorizontal * deltaT;

        // Calcular el desplazamiento horizontal.
        qreal deltaX = velocidadHorizontal * deltaT;

        // Actualizar la posición X del salto.
        posicionXSalto += deltaX;

        // Aplicar límites de pantalla.
        if (posicionXSalto < limitesEscena.left()) {
            posicionXSalto = limitesEscena.left();
            velocidadHorizontal = 0;
        } else if (posicionXSalto > limitesEscena.right() - hitboxAncho) {
            posicionXSalto = limitesEscena.right() - hitboxAncho;
            velocidadHorizontal = 0;
        }

        // Calcular la posición final.
        qreal posicionFinalX = posicionXSalto;
        qreal posicionFinalY = posicionBaseSalto + offsetYSalto;

        // Actualizar la posición completa.
        QGraphicsPixmapItem::setPos(posicionFinalX, posicionFinalY);

        // Añadir punto a la trayectoria para visualización.
        trayectoriaSalto.append(QPointF(posicionFinalX, posicionFinalY));

        // Limitar el tamaño de la trayectoria.
        if (trayectoriaSalto.size() > maxPuntosTrayectoriaSalto) {
            trayectoriaSalto.removeFirst();
        }

        // Actualizar la visualización del hitbox durante el salto.
        actualizarVisualizacionHitbox();

        // Actualizar la altura máxima alcanzada.
        if (posicionFinalY < alturaMaximaAlcanzada) {
            alturaMaximaAlcanzada = posicionFinalY;
        }

        // Lógica de animación de salto (7 sprites).
        int nuevoFrameSalto = frameSaltoActual;

        // Duraciones ajustadas para los sprites de salto.
        if (tiempo <= 0.077) {
            nuevoFrameSalto = 1;
        } else if (tiempo <= 0.154) {
            nuevoFrameSalto = 2;
        } else if (tiempo <= 0.231) {
            nuevoFrameSalto = 3;
        } else if (tiempo <= 0.308) {
            nuevoFrameSalto = 4;
        } else if (tiempo <= 0.385) {
            nuevoFrameSalto = 5;
        } else if (tiempo <= 0.440) {
            nuevoFrameSalto = 6;
        } else {
            nuevoFrameSalto = 7;
        }

        // Cambiar sprite solo si es necesario.
        if (nuevoFrameSalto != frameSaltoActual) {
            frameSaltoActual = nuevoFrameSalto;
            cambiarSprite("salto" + QString::number(frameSaltoActual));
        }

        // Verificar si ha aterrizado.
        if (posicionFinalY >= alturaOriginal && velocidadVertical >= 0) {
            // Aterrizaje con amortiguación.
            setPos(posicionXSalto, alturaOriginal);

            saltando = false;
            velocidadVertical = 0.0;
            aceleracionVertical = 0.0;
            velocidadHorizontal = 0.0;
            jumpTimer->stop();
            tiempo = 0;
            frameSaltoActual = 1;
            alturaMaximaAlcanzada = 0.0;

            // Restaurar el hitbox normal al aterrizar.
            restaurarHitboxNormal();

            // Limpiar la trayectoria de salto al aterrizar.
            trayectoriaSalto.clear();

            // Cambiar inmediatamente al sprite base1 al aterrizar.
            cambiarSprite("base1");



            // Reiniciar la animación de inactividad si no se está moviendo.
            if (!moviendose) {
                iniciarAnimacionIdle();
            }
        }
    }
}

void Personaje::actualizarAnimacion()
{
    // Actualiza el fotograma de la animación de inactividad (idle).
    if (!moviendose && !saltando && estaVivo()) {
        // Ciclar a través de los fotogramas de la animación de inactividad.
        frameActual++;
        if (frameActual > frameMaximo) {
            frameActual = 1;
        }

        QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + baseFase + QString::number(frameActual) + ".png";
        QPixmap nuevoSprite(rutaSprite);

        if (!nuevoSprite.isNull()) {
            // Escalar el sprite si es necesario.
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
            // Manejar el error de carga del sprite.
        }
    }
}

void Personaje::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Dibuja el sprite y la trayectoria de salto si la visualización del hitbox está habilitada.
    QGraphicsPixmapItem::paint(painter, option, widget);

    // Visualizar la trayectoria de salto si la visualización del hitbox está habilitada.
    if (hitboxVisible && !trayectoriaSalto.isEmpty() && trayectoriaSalto.size() > 1) {
        // Configurar la pluma para la línea de la trayectoria de salto.
        QPen penTrayectoria;
        penTrayectoria.setColor(Qt::blue);
        penTrayectoria.setWidth(2);
        penTrayectoria.setStyle(Qt::DashLine);
        painter->setPen(penTrayectoria);

        // Dibujar líneas conectando todos los puntos de la trayectoria.
        for (int i = 0; i < trayectoriaSalto.size() - 1; ++i) {
            QPointF puntoInicio = mapFromScene(trayectoriaSalto[i]);
            QPointF puntoFin = mapFromScene(trayectoriaSalto[i + 1]);
            painter->drawLine(puntoInicio, puntoFin);
        }

        // Conectar el último punto de la trayectoria con la posición actual.
        if (!trayectoriaSalto.isEmpty()) {
            QPointF ultimoPunto = mapFromScene(trayectoriaSalto.last());
            QPointF posicionActual = QPointF(0, 0);
            painter->drawLine(ultimoPunto, posicionActual);
        }
    }
}

// ==================== MÉTODOS DE HITBOX ====================

void Personaje::establecerHitbox(qreal ancho, qreal alto, qreal offsetX, qreal offsetY)
{
    // Establece las dimensiones y offsets del hitbox normal, y los guarda como respaldo.
    hitboxAncho = ancho;
    hitboxAlto = alto;
    hitboxOffsetX = offsetX;
    hitboxOffsetY = offsetY;

    // Guardar como hitbox normal (respaldo).
    hitboxNormalAncho = ancho;
    hitboxNormalAlto = alto;
    hitboxNormalOffsetX = offsetX;
    hitboxNormalOffsetY = offsetY;
}

void Personaje::establecerHitboxSalto(qreal ancho, qreal alto, qreal offsetX, qreal offsetY)
{
    // Establece las dimensiones y offsets del hitbox de salto.
    hitboxSaltoAncho = ancho;
    hitboxSaltoAlto = alto;
    hitboxSaltoOffsetX = offsetX;
    hitboxSaltoOffsetY = offsetY;
}

void Personaje::aplicarHitboxSalto()
{
    // Aplica el hitbox específico del salto si el personaje está saltando.
    if (saltando) {
        hitboxAncho = hitboxSaltoAncho;
        hitboxAlto = hitboxSaltoAlto;
        hitboxOffsetX = hitboxSaltoOffsetX;
        hitboxOffsetY = hitboxSaltoOffsetY;
        actualizarVisualizacionHitbox();
    }
}

void Personaje::restaurarHitboxNormal()
{
    // Restaura las dimensiones y offsets del hitbox normal.
    hitboxAncho = hitboxNormalAncho;
    hitboxAlto = hitboxNormalAlto;
    hitboxOffsetX = hitboxNormalOffsetX;
    hitboxOffsetY = hitboxNormalOffsetY;
    actualizarVisualizacionHitbox();
}

QRectF Personaje::obtenerHitbox() const
{
    // Devuelve el hitbox del personaje en coordenadas locales.
    return QRectF(hitboxOffsetX, hitboxOffsetY, hitboxAncho, hitboxAlto);
}

QRectF Personaje::obtenerHitboxGlobal() const
{
    // Devuelve el hitbox del personaje en coordenadas de escena globales.
    QPointF posicion = this->pos();
    return QRectF(posicion.x() + hitboxOffsetX,
                  posicion.y() + hitboxOffsetY,
                  hitboxAncho,
                  hitboxAlto);
}

bool Personaje::colisionaCon(Personaje* otroPersonaje) const
{
    // Verifica la colisión con el hitbox de otro personaje.
    if (!otroPersonaje || otroPersonaje == this) {
        return false;
    }

    QRectF miHitbox = obtenerHitboxGlobal();
    QRectF otraHitbox = otroPersonaje->obtenerHitboxGlobal();

    bool colision = miHitbox.intersects(otraHitbox);

    return colision;
}

bool Personaje::colisionaCon(const QRectF& rectangulo) const
{
    // Verifica la colisión con un rectángulo dado.
    QRectF miHitbox = obtenerHitboxGlobal();
    return miHitbox.intersects(rectangulo);
}

void Personaje::verificarLimitesPantalla(const QRectF& limitesEscena)
{
    // Verifica y ajusta la posición del personaje para mantenerse dentro de los límites de la pantalla.
    QPointF posicionActual = this->pos();
    QRectF hitboxGlobal = obtenerHitboxGlobal();

    qreal nuevaX = posicionActual.x();
    qreal nuevaY = posicionActual.y();

    // Verificar límite izquierdo.
    if (hitboxGlobal.left() < limitesEscena.left()) {
        nuevaX = limitesEscena.left() - hitboxOffsetX;
    }

    // Verificar límite derecho.
    if (hitboxGlobal.right() > limitesEscena.right()) {
        nuevaX = limitesEscena.right() - hitboxAncho - hitboxOffsetX;
    }

    // Verificar límite superior.
    if (hitboxGlobal.top() < limitesEscena.top()) {
        nuevaY = limitesEscena.top() - hitboxOffsetY;
    }

    // Verificar límite inferior (solo si no está saltando).
    if (!saltando && hitboxGlobal.bottom() > limitesEscena.bottom()) {
        nuevaY = limitesEscena.bottom() - hitboxAlto - hitboxOffsetY;
    }

    // Aplicar nueva posición si ha cambiado.
    if (nuevaX != posicionActual.x() || nuevaY != posicionActual.y()) {
        setPos(nuevaX, nuevaY);
    }
}

void Personaje::establecerLimitesEscena(const QRectF& limites)
{
    // Establece los límites de la escena para el personaje.
    limitesEscena = limites;
}

// ==================== MÉTODOS DE VISUALIZACIÓN DE HITBOX ====================

void Personaje::mostrarHitbox(bool mostrar)
{
    // Alterna la visibilidad del hitbox del personaje para depuración.
    hitboxVisible = mostrar;

    if (mostrar) {
        // Crear o actualizar la visualización del hitbox.
        if (!hitboxVisual) {
            hitboxVisual = new QGraphicsRectItem();

            // Configurar la apariencia del hitbox.
            QPen pen(Qt::red, 1);
            pen.setStyle(Qt::DashLine);
            hitboxVisual->setPen(pen);

            // Sin relleno para el hitbox.
            hitboxVisual->setBrush(Qt::NoBrush);

            // Añadir a la escena si el personaje ya está en una escena.
            if (this->scene()) {
                this->scene()->addItem(hitboxVisual);
            }
        }

        // Actualizar la posición y el tamaño del visual del hitbox.
        actualizarVisualizacionHitbox();
        hitboxVisual->setVisible(true);

    } else {
        // Ocultar el hitbox.
        if (hitboxVisual) {
            hitboxVisual->setVisible(false);
        }
    }
}

void Personaje::ocultarHitbox()
{
    // Oculta la visualización del hitbox del personaje.
    mostrarHitbox(false);
}

void Personaje::actualizarVisualizacionHitbox()
{
    // Actualiza la posición y el tamaño del rectángulo visual del hitbox.
    if (hitboxVisual && hitboxVisible) {
        // Actualizar la posición y el tamaño del rectángulo visual.
        QRectF hitboxGlobal = obtenerHitboxGlobal();
        hitboxVisual->setRect(hitboxGlobal);

        // Asegurar que esté en el nivel Z correcto (encima del personaje).
        hitboxVisual->setZValue(this->zValue() + 1);
    }
}

void Personaje::cambiarSpriteConOffset(const QString& direccion, qreal offsetX, qreal offsetY)
{
    // Cambia el sprite del personaje sin alterar su posición, permitiendo offsets específicos del sprite.
    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    if (!nuevoSprite.isNull()) {
        // Escalar el sprite si es necesario.
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }

        // Cambiar el sprite sin mover la posición.
        setPixmap(nuevoSprite);

    } else {
        // Manejar el error de carga del sprite.
    }
}

void Personaje::moverDerecha(){}
void Personaje::moverIzquierda(){}
void Personaje::moverArriba(){}
void Personaje::moverAbajo(){}
void Personaje::atacar(){}
void Personaje::iniciarAnimacionEntrada(){}
void Personaje::actualizarAnimacionEntrada(){}
void Personaje::kiCambiado(int kiActual = 0, int kiMaximo = 0) {}

void Personaje::recibirDanio(int danio){
    vida -= danio;
    if (vida < 0) vida = 0;
    emit vidaCambiada(vida, vidaMaxima);
}
