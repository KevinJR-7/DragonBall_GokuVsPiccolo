#include <QTimer>

#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug> // Se mantiene para depuración, pero los qDebug específicos se eliminarán.

#include "goku.h"
#include "game.h"

// Constructor de la clase Goku
Goku::Goku(QObject *parent)
    : Personaje(parent)
{
    // Inicializa la última dirección horizontal del personaje.
    ultimaDireccionHorizontal = "";

    // Configura la animación de entrada del personaje.
    animacionEntradaActiva = false;
    frameEntradaActual = 1;
    timerEntrada = new QTimer(this);
    timerEntrada->setInterval(180);
    connect(timerEntrada, &QTimer::timeout, this, &Goku::actualizarAnimacionEntrada);

    // Configura la animación de recarga de ki del personaje.
    animacionKiActiva = false;
    frameKiActual = 1;
    timerKi = new QTimer(this);
    timerKi->setInterval(150);
    connect(timerKi, &QTimer::timeout, this, &Goku::actualizarAnimacionKi);

    // Inicializa el sistema de ki del personaje.
    estadoKiActual = INICIO;
    frameDentroEstado = 1;
    kiActual = 0;
    kiMaximo = 100;
    velocidadRecargaKi = 10;

    // Configura los offsets para los sprites de ki.
    offsetKiX = -18.0;
    offsetKiY = -16.0;

    // Configura el temporizador para la recarga real de ki.
    timerRecargaKi = new QTimer(this);
    timerRecargaKi->setInterval(100);
    connect(timerRecargaKi, &QTimer::timeout, this, &Goku::recargarKi);

    // Configura la animación de Kamehameha del personaje.
    animacionKamehamehaActiva = false;
    frameKamehamehaActual = 1;
    timerKamehameha = new QTimer(this);
    timerKamehameha->setInterval(120);
    connect(timerKamehameha, &QTimer::timeout, this, &Goku::actualizarAnimacionKamehameha);

    // Inicializa la posición fija para la animación de Kamehameha.
    posicionFijaKamehameha = QPointF(0, 0);

    // Configura la animación de ráfaga del personaje.
    animacionRafagaActiva = false;
    frameRafagaActual = 1;
    timerRafaga = new QTimer(this);
    timerRafaga->setInterval(100);
    connect(timerRafaga, &QTimer::timeout, this, &Goku::actualizarAnimacionRafaga);

    // Configura propiedades específicas de Goku como nombre, carpeta de sprites, vida y velocidad.
    establecerNombre("Goku");
    establecerCarpetaSprites("goku");
    establecerVida(100);
    establecerVelocidad(10);

<<<<<<< HEAD
    // Configurar física del salto específica para Goku
    establecerVelocidadSalto(10000.0);      // Goku salta SÚPER alto (aumentado de 50 a 65)
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia
=======
    // Configura la física del salto específica para Goku.
    establecerVelocidadSalto(65.0);
    establecerFisicaSalto(0.7, 0.03);
>>>>>>> af87db498d8e2481f35966d5aea5ed90b109716a

    // Configura la hitbox y hitbox de salto específicas para Goku.
    establecerHitbox(70, 140, 42, 88);
    establecerHitboxSalto(77, 70, 32, 70);

    // Hace a Goku invisible al inicio y programa la animación de entrada.
    setVisible(false);
    QTimer::singleShot(2000, this, &Goku::iniciarAnimacionEntrada);
}

// Mueve al personaje hacia la derecha.
void Goku::moverDerecha()
{
    // Restringe el movimiento si el personaje no está visible o está en animaciones específicas.
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
        return;
    }
    if (animacionKiActiva) return;

    moviendose = true;
    ultimaDireccionHorizontal = "adelante";

    // Maneja el movimiento horizontal durante el salto o en el suelo.
    if (estaSaltando()) {
        qreal velocidadSalto = velocidadMovimiento * 3;
        aplicarMovimientoHorizontal(velocidadSalto);
    } else {
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior + velocidadMovimiento;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("adelante");
    }

    // Verifica los límites de la pantalla y actualiza la visualización de la hitbox.
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

// Mueve al personaje hacia la izquierda.
void Goku::moverIzquierda()
{
    // Restringe el movimiento si el personaje no está visible o está en animaciones específicas.
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
        return;
    }
    if (animacionKiActiva) return;

    moviendose = true;
    ultimaDireccionHorizontal = "atras";

    // Maneja el movimiento horizontal durante el salto o en el suelo.
    if (estaSaltando()) {
        qreal velocidadSalto = velocidadMovimiento * 3;
        aplicarMovimientoHorizontal(-velocidadSalto);
    } else {
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior - velocidadMovimiento;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("atras");
    }

    // Verifica los límites de la pantalla y actualiza la visualización de la hitbox.
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

// Mueve al personaje hacia arriba.
void Goku::moverArriba()
{
    // Restringe el movimiento si el personaje no está visible o está en animaciones específicas.
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
        return;
    }
    if (animacionKiActiva) return;

    moviendose = true;

    // Detiene la animación idle si no está saltando.
    if (!estaSaltando()) {
        animacionTimer->stop();
    }

    // Actualiza la posición y verifica límites.
    this->setPos(this->pos().x(), this->pos().y() - velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();

    // Cambia el sprite si no está saltando.
    if (!estaSaltando()) {
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

// Mueve al personaje hacia abajo.
void Goku::moverAbajo()
{
    // Restringe el movimiento si el personaje no está visible o está en animaciones específicas.
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
        return;
    }
    if (animacionKiActiva) return;

    moviendose = true;

    // Detiene la animación idle si no está saltando.
    if (!estaSaltando()) {
        animacionTimer->stop();
    }

    // Actualiza la posición y verifica límites.
    this->setPos(this->pos().x(), this->pos().y() + velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();

    // Cambia el sprite si no está saltando.
    if (!estaSaltando()) {
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}


// Aplica daño al personaje.
void Goku::recibirDanio(int danio)
{
    moviendose = true;
    // Detiene la recarga de ki si está activa.
    if (animacionKiActiva) {
        detenerRecargaKi();
    }

    // Actualiza la vida del personaje, emite señal y cambia al sprite de herido.
    if (estaVivo()) {
        vida -= danio;
        if (vida < 0) vida = 0;
        emit vidaCambiada(vida, vidaMaxima);

        cambiarSprite("herido");

        // Regresa a la animación idle después de un tiempo.
        QTimer::singleShot(1000, this, [this]() {
            moviendose = false;
        });

        // Maneja la muerte del personaje si la vida llega a cero.
        if (vida <= 0) {
            morir();
        }
    }
}

// Inicia la animación idle del personaje.
void Goku::iniciarAnimacionIdle()
{
    // Limpia la dirección horizontal al entrar en idle.
    ultimaDireccionHorizontal = "";

    // Llama al método de la clase padre.
    Personaje::iniciarAnimacionIdle();
}

// Inicia la animación de entrada del personaje.
void Goku::iniciarAnimacionEntrada()
{
    // Hace visible a Goku y detiene otras animaciones.
    setVisible(true);

    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    // Configura y carga el primer sprite de la animación de entrada.
    animacionEntradaActiva = true;
    frameEntradaActual = 1;

    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/entrada1.png";
    QPixmap spriteEntrada(rutaSprite);
    if (!spriteEntrada.isNull()) {
        setPixmap(spriteEntrada);
    }
    // Inicia el temporizador de la animación de entrada.
    timerEntrada->start();
}

// Actualiza la animación de entrada del personaje.
void Goku::actualizarAnimacionEntrada()
{
    moviendose = true;
    if (animacionEntradaActiva) {
        frameEntradaActual++;

        // Muestra los sprites de entrada secuencialmente.
        if (frameEntradaActual <= 3) {
            QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/entrada" + QString::number(frameEntradaActual) + ".png";
            QPixmap spriteEntrada(rutaSprite);
            if (!spriteEntrada.isNull()) {
                setPixmap(spriteEntrada);
            }
        } else {
            // Detiene la animación al completarse.
            timerEntrada->stop();
            animacionEntradaActiva = false;
            moviendose = false;
        }
    }
}


// Inicia la recarga de ki del personaje.
void Goku::iniciarRecargaKi()
{
    // Verifica si el ki ya está completo.
    if (kiActual >= kiMaximo) {
        return;
    }

    // Guarda la posición original, detiene otras animaciones y activa la animación de ki.
    posicionOriginalKi = pos();

    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionKiActiva = true;

    // Configura el estado inicial y la velocidad de la animación.
    estadoKiActual = INICIO;
    frameDentroEstado = 1;

    int velocidadAnimacion = 150 - (int)(obtenerPorcentajeKi() * 1.2f);
    velocidadAnimacion = qMax(30, velocidadAnimacion);
    timerKi->setInterval(velocidadAnimacion);

    // Centra el primer sprite de ki y lo carga.
    posicionOriginalKi = pos();
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);
    cambiarSprite("ki1");
    QPointF nuevaPosicion = centroActual - QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);
    setPos(nuevaPosicion.x(), nuevaPosicion.y());

    // Inicia los temporizadores de animación y recarga de ki.
    timerKi->start();
    timerRecargaKi->start();
}

// Detiene la recarga de ki del personaje.
void Goku::detenerRecargaKi()
{
    if (animacionKiActiva) {
        // Desactiva la animación, detiene los temporizadores y vuelve al estado idle.
        animacionKiActiva = false;
        timerKi->stop();
        timerRecargaKi->stop();

        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Actualiza la animación de recarga de ki.
void Goku::actualizarAnimacionKi()
{
    if (!animacionKiActiva) return;

    QString spriteActual;

    // Controla los frames de la animación de ki según el estado (INICIO, BUCLE, FINAL).
    switch (estadoKiActual) {
    case INICIO:
        if (frameDentroEstado == 1) {
            spriteActual = "ki1";
        } else if (frameDentroEstado == 2) {
            spriteActual = "ki2";
        }

        frameDentroEstado++;
        if (frameDentroEstado > 2) {
            estadoKiActual = BUCLE;
            frameDentroEstado = 1;
        }
        break;

    case BUCLE: {
        int frameEnBucle = ((frameDentroEstado - 1) % 4) + 3;
        spriteActual = "ki" + QString::number(frameEnBucle);

        frameDentroEstado++;
        break;
    }

    case FINAL:
        if (frameDentroEstado == 1) {
            spriteActual = "ki7";
            frameDentroEstado++;
        } else if (frameDentroEstado == 2) {
            spriteActual = "ki8";
            frameDentroEstado++;
        } else {
            detenerRecargaKi();
            return;
        }
        break;
    }

    // Cambia el sprite actual.
    if (!spriteActual.isEmpty()) {
        cambiarSprite(spriteActual);
    }
}

// Establece la cantidad actual y máxima de ki.
void Goku::establecerKi(int ki, int kiMax) {
    kiActual = ki;
    kiMaximo = kiMax;
    if (kiActual < 0) kiActual = 0;
    if (kiActual > kiMaximo) kiActual = kiMaximo;
    emit kiCambiado(kiActual, kiMaximo);
}

// Recarga el ki del personaje.
void Goku::recargarKi()
{
    if (animacionKiActiva && kiActual < kiMaximo) {
        // Incrementa el ki y emite la señal de cambio.
        kiActual++;
        emit kiCambiado(kiActual, kiMaximo);
        // Cambia al estado FINAL si el ki alcanza el 95%.
        if (obtenerPorcentajeKi() >= 95.0f && estadoKiActual == BUCLE) {
            estadoKiActual = FINAL;
            frameDentroEstado = 1;
        }

        // Detiene la recarga si el ki está lleno.
        if (kiActual >= kiMaximo) {
            detenerRecargaKi();
        }
    }
}

// Inicia la carga del Kamehameha.
void Goku::iniciarCargaKamehameha()
{
    // Verifica si otras animaciones están activas.
    if (animacionKamehamehaActiva || animacionKiActiva) {
        return;
    }

    // Guarda la posición actual, detiene otras animaciones y activa la animación de Kamehameha.
    posicionFijaKamehameha = pos();

    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionKamehamehaActiva = true;
    frameKamehamehaActual = 1;

    // Carga el primer sprite de Kamehameha en una posición fija.
    cambiarSpriteKamehamehaFijo("kame1");

    // Inicia el temporizador del Kamehameha.
    timerKamehameha->start();
}

// Detiene la carga del Kamehameha prematuramente.
void Goku::detenerCargaKamehameha()
{
    if (animacionKamehamehaActiva) {
        // Desactiva la animación, detiene el temporizador y vuelve al estado idle.
        animacionKamehamehaActiva = false;
        timerKamehameha->stop();

        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Actualiza la animación de Kamehameha.
void Goku::actualizarAnimacionKamehameha()
{
    if (!animacionKamehamehaActiva) return;

    // Itera a través de los frames del Kamehameha.
    if (frameKamehamehaActual <= 12) {
        QString spriteKamehameha = "kame" + QString::number(frameKamehamehaActual);
        cambiarSpriteKamehamehaFijo(spriteKamehameha);

        // Lanza el proyectil en un frame específico.
        if (frameKamehamehaActual == 7) {
            lanzarKamehameha();
        }

        frameKamehamehaActual++;
    } else {
        // Termina la animación, detiene el temporizador y restaura la posición y estado idle.
        animacionKamehamehaActiva = false;
        timerKamehameha->stop();

        setPos(posicionFijaKamehameha.x(), posicionFijaKamehameha.y());
        cambiarSprite("quieto");

        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
    }
}

// Lanza el proyectil Kamehameha.
void Goku::lanzarKamehameha()
{
    int costoKi = 30;
    // Verifica si hay suficiente ki para lanzar.
    if (kiActual >= costoKi) {
        // Reduce el ki y actualiza la barra.
        kiActual -= costoKi;
        emit kiCambiado(kiActual, kiMaximo);

        // Crea y configura el proyectil Kamehameha.
        Kamehameha* kamehameha = new Kamehameha(this);

        QPointF posicionGoku = pos();

        float posX = posicionGoku.x() + 210;
        float posY = posicionGoku.y() + 140;

        float direccionX = 1.0f;
        float direccionY = 0.0f;

        float velocidad = 8.0f;
        float alcance = 600.0f;

        kamehameha->crear(posX, posY, direccionX, direccionY, velocidad, alcance);

        // Agrega el proyectil a la escena.
        if (scene()) {
            scene()->addItem(kamehameha);
        } else {
        }
    } else {
    }
}

// Inicia la animación de ráfaga.
void Goku::iniciarAnimacionRafaga()
{
    // Restringe el inicio si hay otras animaciones activas.
    if (animacionEntradaActiva || animacionKiActiva || animacionKamehamehaActiva) {
        return;
    }

    // Detiene la animación idle y configura la animación de ráfaga.
    if (animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionRafagaActiva = true;
    frameRafagaActual = 1;
    moviendose = true;

    // Inicia con el primer frame y el temporizador.
    cambiarSprite("bolas1");

    timerRafaga->start();
}

// Detiene la animación de ráfaga.
void Goku::detenerAnimacionRafaga()
{
    if (!animacionRafagaActiva) {
        return;
    }

    // Detiene el temporizador y desactiva la animación.
    timerRafaga->stop();

    animacionRafagaActiva = false;

    // Vuelve al estado idle.
    moviendose = false;
    frameActual = 1;
}

// Actualiza la animación de ráfaga.
void Goku::actualizarAnimacionRafaga()
{
    if (!animacionRafagaActiva) {
        return;
    }

    // Cicla a través de los frames de la ráfaga.
    frameRafagaActual++;
    if (frameRafagaActual > 5) {
        frameRafagaActual = 1;
    }

    // Cambia el sprite.
    QString spriteNombre = QString("bolas%1").arg(frameRafagaActual);
    cambiarSprite(spriteNombre);

    // Lanza proyectiles BlastB en frames específicos.
    if (frameRafagaActual == 2 || frameRafagaActual == 3 || frameRafagaActual == 4) {
        lanzarBlastB();
    }
}


// Lanza el proyectil BlastB.
void Goku::lanzarBlastB()
{
    int costoKi = 10;
    // Verifica si hay suficiente ki.
    if (kiActual >= costoKi) {
        // Reduce el ki y actualiza la barra.
        kiActual -= costoKi;
        emit kiCambiado(kiActual, kiMaximo);

        // Crea y configura el proyectil BlastB.
        BlastB* blastB = new BlastB(this);

        blastB->setCarpetaSprites(carpetaSprites);
        blastB->cargarSprites();

        // Determina el tipo de trayectoria según la carpeta de sprites.
        if (carpetaSprites == "goku2") {
            blastB->setTipoTrayectoria(BlastB::ESPIRAL);
        } else {
            blastB->setTipoTrayectoria(BlastB::LORENZ);
        }

        QPointF posicionGoku = pos();

        float posX = posicionGoku.x() + 140;
        float posY = posicionGoku.y() + 88;

        float direccionX = 1.0f;
        float direccionY = 0.0f;

        float velocidad = 6.0f;
        float alcance = 1000.0f;

        blastB->crear(posX, posY, direccionX, direccionY, velocidad, alcance);

        // Configura los parámetros caóticos.
        blastB->configurarCaos(10.0, 28.0, 8.0/3.0);

        // Agrega el proyectil a la escena y lo inicia.
        if (scene()) {
            scene()->addItem(blastB);
            blastB->iniciar(QPointF(posX, posY), QPointF(direccionX, direccionY));
        } else {
        }
    } else {
    }
}


// Cambia el sprite de Kamehameha manteniendo una posición fija.
void Goku::cambiarSpriteKamehamehaFijo(const QString& direccion)
{
    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
    QPixmap nuevoSprite(rutaSprite);

    if (!nuevoSprite.isNull()) {
        // Escala el sprite si es necesario.
        if (escalaSprite != 1.0) {
            nuevoSprite = nuevoSprite.scaled(
                nuevoSprite.width() * escalaSprite,
                nuevoSprite.height() * escalaSprite,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }

        // Establece el nuevo sprite y lo centra.
        setPixmap(nuevoSprite);

        QGraphicsPixmapItem::setPos(posicionFijaKamehameha);
    } else {
    }
}

// Maneja la lógica de la muerte del personaje.
void Goku::morir()
{
    // Detiene otras animaciones y movimiento.
    if (animacionTimer && animacionTimer->isActive())
        animacionTimer->stop();
    moviendose = false;

    // Inicia la animación de muerte.
    frameMuerteActual = 1;
    if (!timerMuerte) {
        timerMuerte = new QTimer(this);
        connect(timerMuerte, &QTimer::timeout, this, [this]() {
            // Muestra los frames de muerte secuencialmente.
            if (frameMuerteActual <= 4) {
                cambiarSprite(QString("muerto%1").arg(frameMuerteActual));
                frameMuerteActual++;
            } else {
                // Detiene el temporizador y emite la señal de personaje muerto.
                timerMuerte->stop();
                QTimer::singleShot(100, this, [this]() {
                    emit personajeMuerto(this);
                });
            }
        });
    }
    timerMuerte->start(200);
}


// Inicia la animación de golpe.
void Goku::golpear() {
    // Evita reiniciar la animación si ya está activa.
    if (timerGolpe && timerGolpe->isActive()) return;
    frameGolpe = 1;
    animarGolpe();
    // Conecta y inicia el temporizador de golpe.
    if (!timerGolpe) timerGolpe = new QTimer(this);
    connect(timerGolpe, &QTimer::timeout, this, &Goku::animarGolpe, Qt::UniqueConnection);
    timerGolpe->start(60);
}

// Anima el golpe del personaje.
void Goku::animarGolpe() {
    moviendose = true;
    // Detiene la animación al terminar los frames.
    if (frameGolpe > 3) {
        timerGolpe->stop();
        moviendose = false;
        return;
    }
    // Cambia el sprite al siguiente frame de golpe.
    cambiarSprite(QString("golpe%1").arg(frameGolpe));
    frameGolpe++;
}

// Inicia la animación de patada.
void Goku::patear() {
    // Evita reiniciar la animación si ya está activa.
    if (timerPatada && timerPatada->isActive()) return;
    framePatada = 1;
    animarPatada();
    // Conecta y inicia el temporizador de patada.
    if (!timerPatada) timerPatada = new QTimer(this);
    connect(timerPatada, &QTimer::timeout, this, &Goku::animarPatada, Qt::UniqueConnection);
    timerPatada->start(60);
}

// Anima la patada del personaje.
void Goku::animarPatada() {
    moviendose = true;
    // Detiene la animación al terminar los frames.
    if (framePatada > 3) {
        timerPatada->stop();
        moviendose = false;
        return;
    }
    // Cambia el sprite al siguiente frame de patada.
    cambiarSprite(QString("patada%1").arg(framePatada));
    framePatada++;
}


// Inicia la animación de teletransporte.
void Goku::tp() {
    // Evita iniciar la animación si ya está activa.
    if (animacionTeleportActiva) return;
    animacionTeleportActiva = true;
    frameTeleport = 1;
    animarTp();
    // Conecta y inicia el temporizador de teletransporte.
    if (!timerTeleport) timerTeleport = new QTimer(this);
    connect(timerTeleport, &QTimer::timeout, this, &Goku::animarTp, Qt::UniqueConnection);
    timerTeleport->start(200);
}

// Anima el teletransporte del personaje.
void Goku::animarTp() {
    moviendose = true;
    // Detiene la animación al terminar los frames.
    if (frameTeleport > 7) {
        timerTeleport->stop();
        animacionTeleportActiva = false;
        moviendose = false;
        return;
    }
    // Cambia el sprite al siguiente frame de teletransporte.
    cambiarSprite(QString("teleport%1").arg(frameTeleport));
    frameTeleport++;
}
