#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug> // Consider removing if all qDebug are to be removed as per instructions

#include "piccolo.h"
#include "goku.h"

Piccolo::Piccolo(QObject *parent)
    : Personaje(parent)
{
    // Inicialización de la fase del personaje
    fase = false;

    // Configuración para reflejar el sprite
    transform.scale(-1, 1);
    // Establece el número máximo de frames para la animación idle
    frameMaximo = 4;

    // Inicializa la dirección horizontal
    ultimaDireccionHorizontal = "";

    // Inicializa y configura la animación de entrada
    animacionEntradaActiva = false;
    frameEntradaActual = 1;
    timerEntrada = new QTimer(this);
    timerEntrada->setInterval(180);
    connect(timerEntrada, &QTimer::timeout, this, &Piccolo::actualizarAnimacionEntrada);

    // Inicializa y configura la animación de Rayo
    animacionRayoActiva = false;
    frameRayoActual = 1;
    timerRayo = new QTimer(this);
    timerRayo->setInterval(150);
    connect(timerRayo, &QTimer::timeout, this, &Piccolo::actualizarAnimacionRayo);

    // Inicializa y configura la animación de Gravity Blast
    animacionGravityBlastActiva = false;
    frameGravityBlastActual = 1;
    timerGravityBlast = new QTimer(this);
    timerGravityBlast->setInterval(200);
    connect(timerGravityBlast, &QTimer::timeout, this, &Piccolo::actualizarAnimacionGravityBlast);

    // Inicializa y configura la animación de Kick
    animacionKickActiva = false;
    frameKickActual = 1;
    timerKick = new QTimer(this);
    timerKick->setInterval(150);
    connect(timerKick, &QTimer::timeout, this, &Piccolo::actualizarAnimacionKick);
    kickAlta = true;

    // Inicializa el objetivo actual del personaje
    objetivoActual = nullptr;

    // Configura propiedades específicas del personaje Piccolo
    establecerNombre("Piccolo");
    establecerCarpetaSprites("piccolo");
    establecerVida(70);
    establecerVelocidad(10);

    // Configura la física de salto específica de Piccolo
    establecerVelocidadSalto(65.0);
    establecerFisicaSalto(0.7, 0.03);

    // Configura la hitbox específica de Piccolo
    establecerHitbox(120, 160, 62, 68);

    // Hace a Piccolo invisible al inicio y programa su animación de entrada
    setVisible(false);

    QTimer::singleShot(2000, this, &Piccolo::iniciarAnimacionEntrada);
}

// Maneja el movimiento de Piccolo hacia la derecha
void Piccolo::moverDerecha()
{
    // Bloquea el movimiento si es invisible, está en animación de entrada o recargando Ki
    if (!isVisible() || animacionGravityBlastActiva || animacionEntradaActiva) {
        return;
    }

    moviendose = true;
    ultimaDireccionHorizontal = "adelante";

    // Aplica movimiento horizontal durante el salto
    if (estaSaltando()) {
        qreal velocidadSalto = velocidadMovimiento * 3;
        aplicarMovimientoHorizontal(velocidadSalto);
    } else {
        // Aplica movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior + velocidadMovimiento;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("atras");
    }

    // Verifica límites y actualiza la visualización de la hitbox
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

// Maneja el movimiento de Piccolo hacia la izquierda
void Piccolo::moverIzquierda()
{
    // Bloquea el movimiento si es invisible, está en animación de entrada o recargando Ki
    if (!isVisible() || animacionGravityBlastActiva || animacionEntradaActiva ) {
        return;
    }

    moviendose = true;
    ultimaDireccionHorizontal = "atras";

    // Aplica movimiento horizontal durante el salto
    if (estaSaltando()) {
        qreal velocidadSalto = velocidadMovimiento * 3;
        aplicarMovimientoHorizontal(-velocidadSalto);
    } else {
        // Aplica movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior - velocidadMovimiento;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("adelante");
    }

    // Verifica límites y actualiza la visualización de la hitbox
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

// Maneja el movimiento de Piccolo hacia arriba
void Piccolo::moverArriba()
{
    // Bloquea el movimiento si es invisible, está en animación de entrada o recargando Ki
    if (!isVisible() || animacionGravityBlastActiva || animacionEntradaActiva) {
        return;
    }

    moviendose = true;

    // Detiene la animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop();
    }

    // Actualiza la posición y verifica límites
    this->setPos(this->pos().x(), this->pos().y() - velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();

    // Cambia el sprite si no está saltando
    if (!estaSaltando()) {
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

// Maneja el movimiento de Piccolo hacia abajo
void Piccolo::moverAbajo()
{
    // Bloquea el movimiento si es invisible, está en animación de entrada o recargando Ki
    if (!isVisible() || animacionGravityBlastActiva || animacionEntradaActiva) {
        return;
    }

    moviendose = true;

    // Detiene la animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop();
    }

    // Actualiza la posición y verifica límites
    this->setPos(this->pos().x(), this->pos().y() + velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();

    // Cambia el sprite si no está saltando
    if (!estaSaltando()) {
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

// Realiza un ataque de patada
void Piccolo::atacar()
{
    // Verifica si hay una escena de juego
    if (!scene()) {
        return;
    }

    // Crea y configura un nuevo objeto Kick
    Kick* patada = new Kick(this);
    patada->establecerEscena(scene());

    // Define la posición inicial de la patada según el tipo de patada
    QPointF posicionInicial;
    if(kickAlta) {posicionInicial = pos() + QPointF(-100, this->pixmap().height() * 3); }
    else{ posicionInicial = pos() + QPointF(-100, this->pixmap().height() * 4); }

    // Inicia y añade la patada a la escena
    patada->iniciar(posicionInicial, QPointF(0,0));
    scene()->addItem(patada);
}

// Maneja el daño recibido por Piccolo
void Piccolo::recibirDanio(int danio)
{
    // Reduce la vida si el personaje está vivo
    if (estaVivo()) {
        vida -= danio;
        if (vida < 0) vida = 0;
        emit vidaCambiada(vida, vidaMaxima);

        // Cambia al sprite de daño si el daño es mayor a 0 y no está en fase de transformación
        if(danio > 0 && !fase){
            cambiarSprite("herido");
        }

        // Vuelve a la animación idle después de un corto tiempo
        QTimer::singleShot(200, this, [this]() {
            iniciarAnimacionIdle();
        });

        // Si la vida llega a 0, el personaje muere
        if (vida <= 0) {
            morir();
        }
    }
}

// Inicia la animación idle del personaje
void Piccolo::iniciarAnimacionIdle()
{
    // Limpia la dirección horizontal al entrar en idle
    ultimaDireccionHorizontal = "";

    // Llama al método de la clase padre
    Personaje::iniciarAnimacionIdle();
}

// Actualiza los frames de la animación de entrada
void Piccolo::actualizarAnimacionEntrada()
{
    if (animacionEntradaActiva) {
        frameEntradaActual++;

        // Muestra los sprites de entrada secuencialmente
        if (frameEntradaActual <= 4) {
            QString rutaSprite = ":/Piccolo/Sprites/piccolo/entrada" + QString::number(frameEntradaActual) + ".png";
            QPixmap spriteEntrada(rutaSprite);
            if (!spriteEntrada.isNull()) {
                spriteEntrada = spriteEntrada.transformed(transform);
                setPixmap(spriteEntrada);
            }
        } else {
            // Finaliza la animación de entrada y cambia a idle
            timerEntrada->stop();
            animacionEntradaActiva = false;
            iniciarAnimacionIdle();
        }
    }
}

// Inicia la secuencia de animación de entrada
void Piccolo::iniciarAnimacionEntrada()
{
    // Hace a Piccolo visible al iniciar la animación
    setVisible(true);

    // Detiene la animación idle si está activa
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionEntradaActiva = true;
    frameEntradaActual = 1;

    // Carga el primer sprite de entrada
    QString rutaSprite = ":/Piccolo/Sprites/piccolo/entrada1.png";
    QPixmap spriteEntrada(rutaSprite);
    if (!spriteEntrada.isNull()) {
        spriteEntrada = spriteEntrada.transformed(transform);
        setPixmap(spriteEntrada);
    }

    // Inicia el timer de entrada
    timerEntrada->start();
}

// Actualiza la animación del personaje (principalmente idle)
void Piccolo::actualizarAnimacion()
{
    // Solo actualiza la animación si no se está moviendo, saltando y está vivo
    if (!moviendose && !saltando && estaVivo()) {
        // Cicla entre los frames de la animación idle
        frameActual++;
        if (frameActual > frameMaximo) {
            frameActual = 1;
        }

        // Carga el sprite correspondiente a la fase actual
        QString rutaSprite;
        if(!fase){ rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/base" + QString::number(frameActual) + ".png"; }
        else{ rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/base_giga" + QString::number(frameActual) + ".png";; }
        QPixmap nuevoSprite(rutaSprite);

        // Escala y transforma el sprite si es necesario
        if (!nuevoSprite.isNull()) {
            if (escalaSprite != 1.0) {
                nuevoSprite = nuevoSprite.scaled(
                    nuevoSprite.width() * escalaSprite,
                    nuevoSprite.height() * escalaSprite,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    );
            }
            if(fase == false){ nuevoSprite = nuevoSprite.transformed(transform); }

            setPixmap(nuevoSprite);
        }
    }
}

// Inicia la carga de la animación de Rayo
void Piccolo::iniciarCargaRayo()
{
    // Verifica si ya se está cargando Rayo o hay otra animación activa
    if (animacionRayoActiva || animacionGravityBlastActiva) {
        return;
    }

    // Guarda la posición actual
    posicionInicialQuieto = pos();

    // Detiene otras animaciones
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionRayoActiva = true;
    frameRayoActual = 1;

    // Cambia al primer sprite de Rayo
    cambiarSprite("rayo1");

    // Inicia el timer de Rayo
    timerRayo->start();
}

// Inicia la carga de la animación de Kick
void Piccolo::iniciarCargaKick()
{
    // Verifica si ya se está cargando Kick
    if (animacionKickActiva) {
        return;
    }

    // Guarda la posición actual
    posicionInicialQuieto = pos();

    // Detiene otras animaciones
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionKickActiva = true;
    frameKickActual = 1;

    // Carga el primer sprite de Kick
    if(kickAlta){ cambiarSprite("kick_baja1"); }
    else{ cambiarSprite("kick_alta1"); }

    // Inicia el timer de Kick
    timerKick->start();
}

// Detiene la carga de la animación de Rayo prematuramente
void Piccolo::detenerCargaRayo()
{
    if (animacionRayoActiva) {
        // Solo detiene la carga sin lanzar el proyectil
        animacionRayoActiva = false;
        timerRayo->stop();

        // Vuelve al sprite quieto y restaura la posición
        cambiarSpriteCentrado("quieto");
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());

        // Configura el estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Detiene la carga de la animación de Kick prematuramente
void Piccolo::detenerCargaKick()
{
    if (animacionKickActiva) {
        // Solo detiene la carga sin lanzar el proyectil
        animacionKickActiva = false;
        timerKick->stop();

        // Vuelve al sprite quieto y restaura la posición
        cambiarSpriteCentrado("quieto");
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());

        // Configura el estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Actualiza los frames de la animación de Rayo
void Piccolo::actualizarAnimacionRayo()
{
    if (!animacionRayoActiva) return;

    frameRayoActual++;

    // Muestra los sprites de Rayo secuencialmente
    if (frameRayoActual <= 6) {
        QString spriteRayo = "rayo" + QString::number(frameRayoActual);
        cambiarSpriteCentrado(spriteRayo);
    } else {
        // Lanza el Rayo automáticamente al final de la animación
        lanzarRayo();

        // Termina la animación y vuelve al estado idle
        animacionRayoActiva = false;
        timerRayo->stop();
        cambiarSpriteCentrado("quieto");
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Actualiza los frames de la animación de Kick
void Piccolo::actualizarAnimacionKick()
{
    if (!animacionKickActiva) return;

    frameKickActual++;

    // Muestra los sprites de Kick secuencialmente
    if (frameKickActual <= 4) {
        QString spriteKick;
        if(kickAlta){ spriteKick = "kick_alta" + QString::number(frameKickActual); }
        else{ spriteKick = "kick_baja" + QString::number(frameKickActual); }
        cambiarSprite(spriteKick);
    }
    else {
        // Termina la animación y vuelve al estado idle
        animacionKickActiva = false;
        timerKick->stop();
        cambiarSpriteCentrado("quieto");
        setPos(posicionInicialQuieto.x(), posicionInicialQuieto.y());
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
    // Lanza la patada en un frame específico de la animación
    if(frameKickActual == 3){
        atacar();
    }
}

// Lanza un proyectil de Rayo
void Piccolo::lanzarRayo()
{
    // Crea y configura el proyectil Rayo
    Rayo* rayo = new Rayo(this);

    // Obtiene la posición actual de Piccolo
    QPointF posicionPiccolo = pos();

    // Calcula la posición de lanzamiento ajustada para el Rayo
    float posX = posicionPiccolo.x() - 150;
    float posY = posicionPiccolo.y() + 40;

    // Define la dirección y propiedades del proyectil
    float direccionX = -1.0f;
    float direccionY = 0.0f;
    float velocidad = 15.0f;
    float alcance = 600.0f;

    // Crea y añade el proyectil a la escena
    rayo->crear(posX, posY, direccionX, direccionY, velocidad, alcance);
    if (scene()) {
        scene()->addItem(rayo);
    }
}

// Lanza un proyectil Gravity Blast hacia un objetivo
void Piccolo::lanzarGravityBlast(Goku* gokuTarget)
{
    // Verifica si hay una escena de juego
    if (!scene()) {
        return;
    }

    // Determina el objetivo
    Goku* targetToUse = gokuTarget ? gokuTarget : objetivoActual;

    // Verifica si hay un objetivo
    if (!targetToUse) {
        return;
    }

    // Crea y configura un nuevo Gravity Blast
    GravityBlast* blast = new GravityBlast(this);
    blast->establecerEscena(scene());
    blast->establecerObjetivo(targetToUse);

    // Define la posición inicial del Gravity Blast
    QPointF posicionInicial = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

    // Define la dirección inicial basada en la última dirección horizontal
    QPointF direccionInicial(1.0, 0.0);
    if (ultimaDireccionHorizontal == "atras") {
        direccionInicial.setX(-1.0);
    } else if (ultimaDireccionHorizontal == "adelante") {
        direccionInicial.setX(1.0);
    } else {
        if (transform.m11() < 0) {
            direccionInicial.setX(-1.0);
        } else {
            direccionInicial.setX(1.0);
        }
    }

    // Inicia y añade el Gravity Blast a la escena
    blast->iniciar(posicionInicial, direccionInicial);
    scene()->addItem(blast);
}

// Establece el objetivo actual para Piccolo
void Piccolo::establecerObjetivo(Goku* objetivo)
{
    objetivoActual = objetivo;
}

// Inicia la carga del ataque Gravity Blast
void Piccolo::iniciarCargaGravityBlast()
{
    // Verifica si ya se está cargando Gravity Blast o hay otra animación en curso
    if (animacionGravityBlastActiva) return;
    if (animacionRayoActiva || estaEnAnimacionEntrada()) {
        return;
    }

    animacionGravityBlastActiva = true;
    frameGravityBlastActual = 1;
    timerGravityBlast->start();

    // Lanza el primer Gravity Blast si hay un objetivo
    if (objetivoActual) {
        lanzarGravityBlast(objetivoActual);
    }
}

// Detiene la carga del ataque Gravity Blast
void Piccolo::detenerCargaGravityBlast()
{
    if (!animacionGravityBlastActiva) return;

    animacionGravityBlastActiva = false;
    timerGravityBlast->stop();

    // Vuelve al estado idle
    iniciarAnimacionIdle();
}

// Actualiza los frames y lanza Gravity Blasts continuamente
void Piccolo::actualizarAnimacionGravityBlast()
{
    if (!animacionGravityBlastActiva) return;

    frameGravityBlastActual++;

    // Cicla los frames de la animación
    if (frameGravityBlastActual > 4) {
        frameGravityBlastActual = 1;
    }

    // Lanza un Gravity Blast periódicamente
    if (frameGravityBlastActual % 2 == 0) {
        if (objetivoActual) {
            lanzarGravityBlast(objetivoActual);
        }
    }
    update();
}

// Alterna entre las dos fases de Piccolo
void Piccolo::alternarFase()
{
    // Cambia de fase 1 a fase 2
    if (!fase) {
        fase = true;
        cambiarSprite("base_giga1");
        setPos(pos().x()-470, pos().y() - 510);
    } else {
        // Cambia de fase 2 a fase 1
        fase = false;
        cambiarSprite("base1");
        setPos(pos().x()+470, pos().y() + 510);
    }
}

// Maneja la lógica de la muerte o transformación de Piccolo
void Piccolo::morir()
{
    // Si no está en fase 2, se transforma
    if(!fase)
    {
        fase = true;
        baseFase = "base_giga";
        cambiarSprite("base_giga1");
        setScale(5.5);
        vida = vidaMaxima;
        recibirDanio(0);
    }
    else{
        // Si ya está en fase 2, el personaje muere
        vida = 0;
        animacionTimer->stop();
        jumpTimer->stop();
        moviendose = false;
        saltando = false;
        velocidadHorizontal = 0.0;

        // Restaura la hitbox normal
        restaurarHitboxNormal();

        // Emite señales de muerte y cambio de vida
        emit personajeMuerto(this);
        emit vidaCambiada(vida, vidaMaxima);
    }
}

// Cambia el sprite del personaje
void Piccolo::cambiarSprite(const QString& direccion)
{
    // Construye la ruta del sprite
    QString rutaSprite;
    rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    // Escala y transforma el sprite si es necesario
    if (!nuevoSprite.isNull()) {
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }
        if(fase == false){ nuevoSprite = nuevoSprite.transformed(transform); }
        setPixmap(nuevoSprite);
    }
}

// Cambia el sprite del personaje manteniendo su centro
void Piccolo::cambiarSpriteCentrado(const QString& direccion)
{
    // Guarda la posición central actual
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

    // Construye la ruta del sprite
    QString rutaSprite;
    rutaSprite = ":/Piccolo/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    // Escala y transforma el sprite si es necesario
    if (!nuevoSprite.isNull()) {
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }

        // Cambia el sprite
        if(fase == false){ nuevoSprite = nuevoSprite.transformed(transform); }
        setPixmap(nuevoSprite);

        // Calcula la nueva posición para mantener el centro
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);
    }
}
