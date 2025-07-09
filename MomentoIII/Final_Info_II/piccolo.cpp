#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include "piccolo.h"

Piccolo::Piccolo(QObject *parent)
    : Personaje(parent)
{
    transform.scale(-1, 1); // Refleja sobre el eje X
    frameMaximo = 4; // Por defecto 4 frames de animación idle

    // Inicializar dirección horizontal
    ultimaDireccionHorizontal = "";

    // Inicializar animación de entrada
    animacionEntradaActiva = false;
    frameEntradaActual = 1;
    timerEntrada = new QTimer(this);
    timerEntrada->setInterval(180); // 180ms por frame
    connect(timerEntrada, &QTimer::timeout, this, &Piccolo::actualizarAnimacionEntrada);

    // Inicializar animación de Rayo
    animacionRayoActiva = false;
    frameRayoActual = 1;
    timerRayo = new QTimer(this);
    timerRayo->setInterval(150); // 120ms por frame de Rayo
    connect(timerRayo, &QTimer::timeout, this, &Piccolo::actualizarAnimacionRayo);

    // Configurar propiedades específicas de Piccolo
    establecerNombre("Piccolo");
    establecerCarpetaSprites("piccolo");
    establecerVida(150); // Piccolo tiene más vida
    establecerVelocidad(10); // Piccolo es rápido

    // Configurar física del salto específica para Piccolo
    establecerVelocidadSalto(65.0);      // Piccolo salta SÚPER alto (aumentado de 50 a 65)
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia

    // Configurar hitbox específica para Piccolo - más delgada y hacia la izquierda
    establecerHitbox(120, 160, 62, 68); // Hitbox: 20x40 (1/3 más delgada) con offset 12,25 (aún más a la izquierda)

    // Configurar hitbox de salto - menos largo, más a la izquierda y más abajo
    // establecerHitboxSalto(22, 20, 9, 20); // Hitbox de salto: más ancha (22), menos largo (20), más a la izquierda (9) y más abajo (20)

    // Hacer a Piccolo invisible al inicio - solo aparecerá en la animación de entrada
    setVisible(false);
    qDebug() << "Piccolo inicializado como invisible - aparecerá en 2 segundos";

    // Programar la animación de entrada para después de 2 segundos
    QTimer::singleShot(2000, this, &Piccolo::iniciarAnimacionEntrada);
}

void Piccolo::moverDerecha()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva) {
        return;
    }

    qDebug() << "moverDerecha() - estaSaltando:" << estaSaltando() << "pos actual:" << this->pos().x();
    moviendose = true;
    ultimaDireccionHorizontal = "adelante"; // Rastrear dirección horizontal

    if (estaSaltando()) {
        // Durante el salto, usar el sistema de movimiento horizontal
        qreal velocidadSalto = velocidadMovimiento * 3; // 3x más rápido en el aire
        qDebug() << "Aplicando movimiento horizontal durante salto:" << velocidadSalto;
        aplicarMovimientoHorizontal(velocidadSalto);
    } else {
        // Movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior + velocidadMovimiento;
        qDebug() << "Movimiento suelo: de" << posicionAnterior << "a" << nuevaX;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("atras");
    }

    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Piccolo::moverIzquierda()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva ) {
        return;
    }

    qDebug() << "moverIzquierda() - estaSaltando:" << estaSaltando() << "pos actual:" << this->pos().x();
    moviendose = true;
    ultimaDireccionHorizontal = "atras"; // Rastrear dirección horizontal

    if (estaSaltando()) {
        // Durante el salto, usar el sistema de movimiento horizontal
        qreal velocidadSalto = velocidadMovimiento * 3; // 3x más rápido en el aire
        qDebug() << "Aplicando movimiento horizontal durante salto:" << -velocidadSalto;
        aplicarMovimientoHorizontal(-velocidadSalto);
    } else {
        // Movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior - velocidadMovimiento;
        qDebug() << "Movimiento suelo: de" << posicionAnterior << "a" << nuevaX;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("adelante");
    }

    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Piccolo::moverArriba()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva) {
        return;
    }

    moviendose = true;

    // Solo detener animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop(); // Detener animación idle
    }

    this->setPos(this->pos().x(), this->pos().y() - velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual

    // Solo cambiar sprite si no está saltando (mantener animación de salto)
    if (!estaSaltando()) {
        // Usar la dirección horizontal si existe, sino "adelante" por defecto
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

void Piccolo::moverAbajo()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva) {
        return;
    }

    moviendose = true;

    // Solo detener animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop(); // Detener animación idle
    }

    this->setPos(this->pos().x(), this->pos().y() + velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual

    // Solo cambiar sprite si no está saltando (mantener animación de salto)
    if (!estaSaltando()) {
        // Usar la dirección horizontal si existe, sino "adelante" por defecto
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

void Piccolo::iniciarAnimacionIdle()
{
    // Limpiar dirección horizontal al entrar en idle
    ultimaDireccionHorizontal = "";

    // Llamar al método de la clase padre
    Personaje::iniciarAnimacionIdle();
}

void Piccolo::actualizarAnimacionEntrada()
{
    if (animacionEntradaActiva) {
        frameEntradaActual++;

        if (frameEntradaActual <= 4) {
            // Mostrar entrada1, entrada2, entrada3, entrada 4 SIN cambiar posición
            QString rutaSprite = ":/Piccolo/Sprites/piccolo/entrada" + QString::number(frameEntradaActual) + ".png";
            QPixmap spriteEntrada(rutaSprite);
            if (!spriteEntrada.isNull()) {
                spriteEntrada = spriteEntrada.transformed(transform);
                setPixmap(spriteEntrada);
                qDebug() << "Animación entrada - frame:" << frameEntradaActual << "posición mantenida:" << pos();
            } else {
                qDebug() << "Error: No se pudo cargar sprite entrada" << frameEntradaActual << "desde" << rutaSprite;
            }
        } else {
            // Terminó la animación de entrada
            qDebug() << "Animación de entrada completada";
            timerEntrada->stop();
            animacionEntradaActiva = false;

            // Cambiar a la animación idle normal
            iniciarAnimacionIdle();
        }
    }
}

void Piccolo::iniciarAnimacionEntrada()
{
    qDebug() << "Iniciando animación de entrada de Piccolo después de 2 segundos";

    // Hacer visible a Piccolo al comenzar la animación de entrada
    setVisible(true);

    // Detener la animación idle si está activa
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionEntradaActiva = true;
    frameEntradaActual = 1;

    // Cargar el primer sprite de entrada SIN cambiar posición
    QString rutaSprite = ":/Piccolo/Sprites/piccolo/entrada1.png";
    QPixmap spriteEntrada(rutaSprite);
    if (!spriteEntrada.isNull()) {
        spriteEntrada = spriteEntrada.transformed(transform);
        setPixmap(spriteEntrada);
        qDebug() << "Sprite entrada1 cargado en posición:" << pos();
    } else {
        qDebug() << "Error: No se pudo cargar sprite entrada1 desde" << rutaSprite;
    }

    // Iniciar el timer de entrada
    timerEntrada->start();
}

void Piccolo::actualizarAnimacion()
{
    if (!moviendose && !saltando && estaVivo()) {
        // Ciclar entre los frames de la animación idle
        frameActual++;
        if (frameActual > frameMaximo) {
            frameActual = 1;
        }

        QString rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/base" + QString::number(frameActual) + ".png";
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
            nuevoSprite = nuevoSprite.transformed(transform);
            setPixmap(nuevoSprite);
        } else {
            qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
        }
    }
}

void Piccolo::iniciarCargaRayo()
{
    // Verificar que no esté ya cargando Rayo
    if (animacionRayoActiva) {
        qDebug() << "No se puede cargar Rayo - ya hay otra animación activa";
        return;
    }

    qDebug() << "Piccolo inicia carga de Rayo";

    // Guardar la posición actual del sprite quieto
    posicionInicialQuieto = pos();
    qDebug() << "Posición inicial de quieto guardada:" << posicionInicialQuieto;

    // Detener otras animaciones
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionRayoActiva = true;
    frameRayoActual = 1; // Volver a empezar desde Rayo1

    // Cargar el primer sprite de Rayo usando centrado automático
    // cambiarSpriteCentrado("Rayo1");
    cambiarSprite("rayo1");

    // Iniciar el timer de Rayo
    timerRayo->start();
}

void Piccolo::detenerCargaRayo()
{
    if (animacionRayoActiva) {
        qDebug() << "Piccolo detiene carga de Rayo prematuramente - frame actual:" << frameRayoActual;

        // Solo detener la carga sin lanzar el proyectil
        // El proyectil se lanza automáticamente al completar la animación

        animacionRayoActiva = false;
        timerRayo->stop();

        // Cambiar al sprite quieto usando centrado automático
        cambiarSpriteCentrado("quieto");

        // Restaurar la posición inicial exacta del sprite quieto
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());
        qDebug() << "Posición restaurada a la inicial de quieto:" << posicionInicialQuieto;

        // Configurar estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
        qDebug() << "Carga de Rayo cancelada - volvió a idle";
    }
}

void Piccolo::actualizarAnimacionRayo()
{
    if (!animacionRayoActiva) return;

    frameRayoActual++;

    if (frameRayoActual <= 6) {
        // Mostrar Rayo1, Rayo2, ..., Rayo5 usando centrado automático
        QString spriteRayo = "rayo" + QString::number(frameRayoActual);
        cambiarSpriteCentrado(spriteRayo);
        qDebug() << "Animación Rayo - frame:" << frameRayoActual;
    } else {
        // Al llegar al frame 15, lanzar el Rayo automáticamente
        qDebug() << "Rayo completamente cargado - lanzando automáticamente";
        lanzarRayo();

        // Terminar la animación
        animacionRayoActiva = false;
        timerRayo->stop();

        // Volver al sprite quieto
        cambiarSpriteCentrado("quieto");
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());

        // Configurar estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
        qDebug() << "Animación Rayo terminada automáticamente";
    }
}

void Piccolo::lanzarRayo()
{
    qDebug() << "¡Lanzando Rayo!";

    // Crear el proyectil Rayo
    Rayo* rayo = new Rayo(this);

    // Obtener la posición actual de Piccolo
    QPointF posicionPiccolo = pos();

    // Calcular la posición de lanzamiento ajustada para Piccolo escalado 3.5x
    // Posición original: 60x35
    // Posición escalada: 210x140 (60*3.5 x 40*3.5) - bajado un poco más
    float posX = posicionPiccolo.x() - 150; // Mucho más a la derecha para Piccolo 3.5x
    float posY = posicionPiccolo.y() + 40; // Un poco más abajo que antes para mejor alineación

    // Determinar dirección (hacia la derecha por defecto)
    float direccionX = -1.0f; // Hacia la derecha
    float direccionY = 0.0f; // Horizontal

    // Configurar el proyectil
    float velocidad = 15.0f; // Velocidad del proyectil (más rápida)
    float alcance = 600.0f;  // Alcance del proyectil

    // Crear el proyectil
    rayo->crear(posX, posY, direccionX, direccionY, velocidad, alcance);

    // Agregar el proyectil a la escena
    if (scene()) {
        scene()->addItem(rayo);
        qDebug() << "Rayo agregado a la escena en posición:" << posX << "," << posY;
    } else {
        qDebug() << "Error: No se pudo agregar Rayo a la escena (scene es null)";
    }
}

void Piccolo::cambiarSprite(const QString& direccion)
{
    QString rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/" + direccion + ".png";
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
        nuevoSprite = nuevoSprite.transformed(transform);
        setPixmap(nuevoSprite);
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}

void Piccolo::cambiarSpriteCentrado(const QString& direccion)
{
    // Guardar la posición central actual
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

    QString rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/" + direccion + ".png";
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

        // Cambiar el sprite
        nuevoSprite = nuevoSprite.transformed(transform);
        setPixmap(nuevoSprite);

        // Calcular nueva posición para mantener el centro
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);

        qDebug() << "Sprite centrado cambiado a:" << direccion << "- Nueva pos:" << nuevaPos;
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}
