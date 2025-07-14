#include "game.h"
#include "ui_game.h"
#include "piccolo.h"
#include "goku.h"
#include "kamehameha.h"
#include "blastb.h"
#include <QResizeEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QApplication>
#include <QRandomGenerator>

game::game(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::game), fondoItem(nullptr), fondoActual(0)
{
    ui->setupUi(this);
    // Inicializar reproductor de música
    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);
    QString rutaBase = QCoreApplication::applicationDirPath();
    QString rutaSonido = rutaBase + "/sonidos/nivel1.wav";
    musicPlayer->setSource(QUrl::fromLocalFile(rutaSonido));
    musicPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(1.0);

    musicPlayer->play();

    // Inicializar lista de fondos disponibles
    fondosDisponibles << ":/Fondos/Sprites/gui_scenes/torneo.png"
                      << ":/Fondos/Sprites/gui_scenes/torneo2.png";

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);

    view->setScene(scene);

    // Configurar la vista
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setDragMode(QGraphicsView::NoDrag);

    // Hacer que la vista ocupe toda la ventana principal
    setCentralWidget(view);

    // Eliminar barra de estado y menú
    statusBar()->setVisible(false);
    menuBar()->setVisible(false);

    // Ocultar bordes y decoraciones de la ventana
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowState(Qt::WindowNoState);

    // Eliminar márgenes y bordes de la vista
    view->setContentsMargins(0, 0, 0, 0);
    view->setFrameStyle(QFrame::NoFrame);
    view->setStyleSheet("QGraphicsView { border: none; margin: 0px; padding: 0px; }");

    // Eliminar barras de scroll
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Eliminar márgenes de la ventana principal
    setContentsMargins(0, 0, 0, 0);

    // Configurar el fondo
    configurarFondo();

    // Inicializar y posicionar a Goku
    p = new Goku();
    scene->addItem(p);
    p->setPos(50, 250);

    // Escalar a Goku
    p->setScale(3.5);

    // Inicializar y posicionar a Piccolo
    pic = new Piccolo();
    scene->addItem(pic);
    pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);

    // Escalar a Piccolo
    pic->setScale(3.5);
    pic->establecerObjetivo(p);

    // Sprite de la cara de Goku
    carapersonaje = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/caragoku.png"));
    scene->addItem(carapersonaje);
    carapersonaje->setPos(20, 10);

    // Barra de vida de Goku
    barraVida = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/vida4.png"));
    scene->addItem(barraVida);
    int offsetX = carapersonaje->pixmap().width() + 10;
    barraVida->setPos(10 + offsetX, 10);

    // Barra Ki de Goku
    barraKi = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/kibar0.png"));
    scene->addItem(barraKi);
    int offsetY = barraVida->pixmap().height() + 5;
    barraKi->setPos(10 + offsetX, 10 + offsetY);

    // Conectar señal de vida de Goku a la barra
    connect(p, &Personaje::vidaCambiada, this, &game::actualizarBarraVida);
    connect(p, &Goku::personajeMuerto, this, &game::manejarDerrotaGoku);

    // Conectar señal de ki de Goku a la barra
    connect(p, &Goku::kiCambiado, this, &game::actualizarBarraKi);

    // Configurar transformación para reflejar sprites de Piccolo
    transform.scale(-1, 1);
    // Sprite de la cara de Piccolo
    carapersonaje2 = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/carapic.png"));
    scene->addItem(carapersonaje2);
    carapersonaje2->setPos(895, 10);

    // Barra de vida de Piccolo
    barraVida2 = new QGraphicsPixmapItem((QPixmap(":/Fondos/Sprites/gui_scenes/vida4.png")).transformed(transform));
    scene->addItem(barraVida2);
    int offsetX2 = (barraVida2->pixmap().width());
    barraVida2->setPos(895 - offsetX2, 10);

    // Barra Ki de Piccolo
    barraKi2 = new QGraphicsPixmapItem((QPixmap(":/Fondos/Sprites/gui_scenes/kibar5.png")).transformed(transform));
    scene->addItem(barraKi2);
    int offsetX3 = barraKi2->pixmap().width();
    int offsetY2 = barraVida2->pixmap().height() + 5;
    barraKi2->setPos(895 - offsetX3, 10 + offsetY2);

    // Conectar señal de vida de Piccolo a la barra
    connect(pic, &Personaje::vidaCambiada, this, &game::actualizarBarraVida2);

    // Configurar timer para movimiento continuo de Goku
    movimientoTimer = new QTimer(this);
    movimientoTimer->setInterval(50);
    connect(movimientoTimer, &QTimer::timeout, this, &game::actualizarMovimiento);

    // Inicializar contador de Piccolo
    cntPiccolo = 0;

    // Configurar timer para el movimiento de la IA de Piccolo
    piccoloIATimer = new QTimer(this);
    piccoloIATimer->setInterval(25);
    connect(piccoloIATimer, &QTimer::timeout, this, &game::piccoloActualizarMovimiento);
    piccoloIATimer->start();

    // Configurar límites de la escena para las colisiones
    QRectF limitesJuego = scene->sceneRect();
    p->establecerLimitesEscena(limitesJuego);

}

game::~game()
{
    // Limpiar el item de fondo
    if (fondoItem) {
        scene->removeItem(fondoItem);
        delete fondoItem;
        fondoItem = nullptr;
    }

    delete ui;
}

// Maneja los eventos de presión de teclas para el control de Goku y acciones de juego
void game::keyPressEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) return;

    // Si Goku está cargando ki, ignorar otras acciones excepto soltar la carga
    if (p->estaRecargandoKi()) {
        if (e->key() != Qt::Key_K) {
            return;
        }
    }

    switch (e->key()) {
    // Control de movimiento de Goku
    case Qt::Key_D:
        teclaD_presionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_A:
        teclaA_presionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_W:
        teclaW_presionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_S:
        teclaS_presionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;

        // Ataques cuerpo a cuerpo de Goku
    case Qt::Key_X:
        p->golpear();
        break;
    case Qt::Key_C:
        p->patear();
        break;

        // Salto direccional de Goku
    case Qt::Key_Space:
        if (!p->estaSaltando() && p->isVisible() && !p->estaEnAnimacionEntrada() && !p->estaRecargandoKi() && !p->estaCargandoKamehameha()) {
            saltoDireccional();
        }
        break;

        // Opciones de visualización y depuración
    case Qt::Key_H:
        // Alternar visualización de hitboxes
        if (p->estaHitboxVisible()) {
            p->ocultarHitbox();
            Kamehameha::alternarVisualizacionHitbox();
            BlastB::alternarVisualizacionHitbox();
        } else {
            p->mostrarHitbox();
            Kamehameha::alternarVisualizacionHitbox();
            BlastB::alternarVisualizacionHitbox();
        }
        if (pic->estaHitboxVisible()) {
            pic->ocultarHitbox();
            Rayo::alternarVisualizacionHitbox();
            GravityBlast::alternarVisualizacionHitbox();
            Kick::alternarVisualizacionHitbox();
        } else {
            pic->mostrarHitbox();
            Rayo::alternarVisualizacionHitbox();
            GravityBlast::alternarVisualizacionHitbox();
            Kick::alternarVisualizacionHitbox();
        }
        break;
    case Qt::Key_B:
        cambiarFondo();
        break;

        // Alternar pantalla completa
    case Qt::Key_F11:
        if (isFullScreen()) showNormal();
        else showFullScreen();
        break;

        // Habilidades de Ki y poderes de Goku
    case Qt::Key_K:
        if (!teclaK_presionada && !p->estaRecargandoKi()) {
            teclaK_presionada = true;
            p->iniciarRecargaKi();
        }
        break;
    case Qt::Key_J:
        if (!teclaJ_presionada && !p->estaCargandoKamehameha()) {
            teclaJ_presionada = true;
            p->iniciarCargaKamehameha();
        }
        break;
    case Qt::Key_L:
        if (!teclaL_presionada && !p->estaEnAnimacionRafaga()) {
            teclaL_presionada = true;
            p->iniciarAnimacionRafaga();
        }
        break;

    case Qt::Key_T:
        p->tp();
        break;
    }
}

// Maneja los eventos de liberación de teclas
void game::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) return;

    switch (e->key()) {
    // Detener movimiento de Goku
    case Qt::Key_D:
        teclaD_presionada = false;
        break;
    case Qt::Key_A:
        teclaA_presionada = false;
        break;
    case Qt::Key_W:
        teclaW_presionada = false;
        break;
    case Qt::Key_S:
        teclaS_presionada = false;
        break;

        // Detener carga de Ki y poderes de Goku
    case Qt::Key_K:
        if (teclaK_presionada) {
            teclaK_presionada = false;
            p->detenerRecargaKi();
        }
        break;
    case Qt::Key_J:
        if (teclaJ_presionada) {
            teclaJ_presionada = false;
            p->detenerCargaKamehameha();
        }
        break;
    case Qt::Key_L:
        if (teclaL_presionada) {
            teclaL_presionada = false;
            p->detenerAnimacionRafaga();
        }
        break;
    }

    // Detener movimiento e iniciar animación de idle si no hay teclas de movimiento presionadas
    if (!teclaD_presionada && !teclaA_presionada && !teclaW_presionada && !teclaS_presionada) {
        movimientoTimer->stop();
        if (!p->estaSaltando()) {
            p->iniciarAnimacionIdle();
        }
    }
}

// Calcula y ejecuta un salto direccional para Goku basado en las teclas de movimiento presionadas
void game::saltoDireccional()
{
    qreal velocidadHorizontalInicial = 0.0;
    qreal velocidadSaltoVertical = 65.0;

    if (teclaD_presionada) {
        velocidadHorizontalInicial += 40.0;
    }

    if (teclaA_presionada) {
        velocidadHorizontalInicial -= 40.0;
    }

    if (teclaW_presionada) {
        velocidadSaltoVertical = 85.0;
    }

    if (teclaS_presionada) {
        velocidadSaltoVertical = 45.0;
    }

    // Configurar velocidades antes de iniciar el salto
    p->establecerVelocidadSalto(velocidadSaltoVertical);
    p->establecerVelocidadHorizontalSalto(velocidadHorizontalInicial);

    // Iniciar el salto
    p->saltar();
}

// Actualiza el movimiento continuo de Goku basado en las teclas de movimiento presionadas
void game::actualizarMovimiento()
{
    // Solo aplicar movimiento si Goku no está saltando
    if (!p->estaSaltando()) {
        if (teclaD_presionada) {
            p->moverDerecha();
        }
        if (teclaA_presionada) {
            p->moverIzquierda();
        }
        if (teclaW_presionada) {
            p->moverArriba();
        }
        if (teclaS_presionada) {
            p->moverAbajo();
        }
    }
}

// Actualiza la barra de vida de Goku según su vida actual
void game::actualizarBarraVida(int vidaActual, int vidaMaxima) {
    vidaActual = ((vidaActual * 4) + vidaMaxima -1) / vidaMaxima;
    barraVida->setPixmap(QPixmap(QString(":/Fondos/Sprites/gui_scenes/vida%1.png").arg(vidaActual)));
}

// Actualiza la barra de vida de Piccolo según su vida actual y maneja su derrota
void game::actualizarBarraVida2(int vidaActual, int vidaMaxima) {
    if (vidaActual <= 0 && !piccoloDerrotado) {
        piccoloDerrotado = true;
        manejarDerrotaPiccolo();
    }
    else{ piccoloDerrotado = false; }
    vidaActual = ((vidaActual * 4) + vidaMaxima -1) / vidaMaxima;
    barraVida2->setPixmap((QPixmap(QString(":/Fondos/Sprites/gui_scenes/vida%1.png").arg(vidaActual))).transformed(transform));
    piccoloW_presionada = false;
    piccoloA_presionada = false;
    piccoloS_presionada = false;
    piccoloD_presionada = false;
    piccoloJ_presionada = false;
    cntPiccolo -= 3;
}

// Actualiza la barra de Ki de Goku según su Ki actual
void game::actualizarBarraKi(int kiActual, int /*kiMaximo*/) {
    int spriteIndex = kiActual / 20;
    if (spriteIndex > 5) spriteIndex = 5;
    barraKi->setPixmap(QPixmap(QString(":/Fondos/Sprites/gui_scenes/kibar%1.png").arg(spriteIndex)));
}

// Maneja la derrota de Goku, detiene el juego y muestra una pantalla de Game Over
void game::manejarDerrotaGoku()
{
    // Detener timers de movimiento
    movimientoTimer->stop();
    piccoloIATimer->stop();

    // Ocultar a Goku
    if (p) p->setVisible(false);

    // Crear un overlay negro para el efecto de fade-out
    QGraphicsRectItem* overlayNegro = new QGraphicsRectItem();
    overlayNegro->setRect(0, 0, scene->width(), scene->height());
    overlayNegro->setBrush(QBrush(Qt::black));
    overlayNegro->setZValue(999);
    overlayNegro->setOpacity(0.0);
    scene->addItem(overlayNegro);

    // Timer para el efecto de fade-out
    QTimer* timerFade = new QTimer(this);
    int* alphaStep = new int(0);
    connect(timerFade, &QTimer::timeout, this, [=]() mutable {
        float opacidad = *alphaStep / 20.0f;
        overlayNegro->setOpacity(opacidad);
        (*alphaStep)++;

        // Cuando el fade-out termina, salir de la aplicación
        if (*alphaStep > 20) {
            timerFade->stop();
            delete timerFade;
            delete alphaStep;
            QApplication::quit();
        }
    });

    timerFade->start(50);
}

// Maneja la derrota de Piccolo y la transición entre niveles
void game::manejarDerrotaPiccolo()
{
    if (nivelActual == 1) {
        // Crear overlay negro para el efecto de fade-out
        QGraphicsRectItem* overlayNegro = new QGraphicsRectItem();
        overlayNegro->setRect(0, 0, scene->width(), scene->height());
        overlayNegro->setBrush(QBrush(Qt::black));
        overlayNegro->setZValue(999);
        overlayNegro->setOpacity(0.0);
        scene->addItem(overlayNegro);

        // Timer para el efecto de fade-out
        QTimer* timerFade = new QTimer(this);
        int* alphaStep = new int(0);

        connect(timerFade, &QTimer::timeout, this, [=]() mutable {
            float opacidad = *alphaStep / 20.0f;
            overlayNegro->setOpacity(opacidad);
            (*alphaStep)++;

            if (*alphaStep > 20) {
                timerFade->stop();
                delete timerFade;
                delete alphaStep;

                // Cambiar a nivel 2
                nivelActual = 2;
                cambiarFondo();
                p->establecerCarpetaSprites("goku2");

                // Cambiar música
                musicPlayer->stop();

                QString rutaBase = QCoreApplication::applicationDirPath();
                QString rutaSonido2 = rutaBase + "/sonidos/nivel2.wav";
                musicPlayer->setSource(QUrl::fromLocalFile(rutaSonido2));
                musicPlayer->play();

                // Efecto de fade-in
                int* fadeInStep = new int(20);
                QTimer* timerFadeIn = new QTimer();

                connect(timerFadeIn, &QTimer::timeout, this, [=]() mutable {
                    float opacidad = *fadeInStep / 20.0f;
                    overlayNegro->setOpacity(opacidad);
                    (*fadeInStep)--;

                    if (*fadeInStep < 0) {
                        timerFadeIn->stop();
                        scene->removeItem(overlayNegro);
                        delete overlayNegro;
                        delete timerFadeIn;
                        delete fadeInStep;
                    }
                });

                timerFadeIn->start(50);
            }
        });

        timerFade->start(50);
    }
    else {
        // Manejar fin del juego (victoria)
        movimientoTimer->stop();
        piccoloIATimer->stop();

        // Crear overlay negro para el efecto de fade-out
        QGraphicsRectItem* overlayNegro = new QGraphicsRectItem();
        overlayNegro->setRect(0, 0, scene->width(), scene->height());
        overlayNegro->setBrush(QBrush(Qt::black));
        overlayNegro->setZValue(999);
        overlayNegro->setOpacity(0.0);
        scene->addItem(overlayNegro);

        // Timer para el efecto de fade-out
        QTimer* timerFade = new QTimer(this);
        int* alphaStep = new int(0);
        connect(timerFade, &QTimer::timeout, this, [=]() mutable {
            float opacidad = *alphaStep / 20.0f;
            overlayNegro->setOpacity(opacidad);
            (*alphaStep)++;

            // Cuando el fade-out termina, salir de la aplicación
            if (*alphaStep > 20) {
                timerFade->stop();
                delete timerFade;
                delete alphaStep;
                QApplication::quit();
            }
        });

        timerFade->start(50);
    }
}

// Implementa la lógica de movimiento y ataque de la IA de Piccolo
void game::piccoloActualizarMovimiento()
{
    // Lógica para los movimientos y ataques de Piccolo
    switch (cntPiccolo) {
    case 10:
        pic->iniciarAnimacionIdle();
        break;
    // Fase 1: Movimientos y ataques de Piccolo
    case 160:
        if (!pic->getFase()) {
            piccoloA_presionada = true;
            piccoloW_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 170:
        if (!pic->getFase()) {
            piccoloW_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 171:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 181:
        if (!pic->getFase()) {
            piccoloD_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 201:
        if (!pic->getFase()) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 202:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 212:
        if (!pic->getFase()) {
            piccoloL_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 213:
        if (!pic->getFase()) {
            piccoloL_presionada = false;
            pic->detenerCargaGravityBlast();
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 214:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 224:
        if (!pic->getFase()) {
            piccoloA_presionada = true;
            piccoloS_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 234:
        if (!pic->getFase()) {
            piccoloS_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 235:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 245:
        if (!pic->getFase()) {
            piccoloD_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 265:
        if (!pic->getFase()) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 266:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 276:
        if (!pic->getFase()) {
            piccoloJ_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 312:
        if (!pic->getFase()) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 313:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 314:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 324:
        if (!pic->getFase()) {
            piccoloW_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 334:
        if (!pic->getFase()) {
            piccoloW_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 335:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 345:
        if (!pic->getFase()) {
            piccoloJ_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 381:
        if (!pic->getFase()) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 382:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 383:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 393:
        if (!pic->getFase()) {
            piccoloS_presionada = true;
            piccoloD_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 403:
        if (!pic->getFase()) {
            piccoloS_presionada = false;
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 404:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 414:
        if (!pic->getFase()) {
            piccoloA_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 434:
        if (!pic->getFase()) {
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 435:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 445:
        if (!pic->getFase()) {
            piccoloJ_presionada = true;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 481:
        if (!pic->getFase()) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 482:
        if (!pic->getFase()) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499;
        }
        break;
    case 483:
        if (!pic->getFase()) {
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);
            pic->iniciarAnimacionIdle();
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
            piccoloL_presionada = false;
            cntPiccolo = 159;
        } else {
            cntPiccolo = 499;
        }
        break;
    case 500:
        if (!pic->getFase()) {
            cntPiccolo = 159;
        }
        else{
            pic->setPos(POSICION_ORIGINAL_X-470, POSICION_ORIGINAL_Y - 510);
        }
        break;

    case 510:
        pic->setPos(POSICION_ORIGINAL_X-470, POSICION_ORIGINAL_Y - 510);
        pic->iniciarAnimacionIdle();
        break;
    case 520:
        pic->iniciarAnimacionIdle();
        break;

    case 620:
        if (pic->getVida() > 0) {
            pic->setkickAlta(!(pic->getkickAlta()));
            piccoloK_presionada = true;
        } else {
            cntPiccolo = 659;
        }
        break;
    case 637:
        if (pic->getVida() > 0) {
            piccoloK_presionada = false;
            pic->detenerCargaKick();
        } else {
            cntPiccolo = 659;
        }
        break;
    case 641:
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 659;
        }
        break;
    case 642:
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 659;
        }
        break;
    case 643:
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
            piccoloK_presionada = false;
            cntPiccolo = 509;
        } else {
            cntPiccolo = 659;
        }
        break;
    case 660:
        if (pic->getVida() > 0) {
            cntPiccolo = 509;
        }
        break;
    }

    if (pic->isVisible() || !pic->estaEnAnimacionEntrada()) {
        cntPiccolo++;
    }

    // Aplicar movimientos de Piccolo
    if (piccoloD_presionada) {
        pic->moverDerecha();
    }
    if (piccoloA_presionada) {
        pic->moverIzquierda();
    }
    if (piccoloW_presionada) {
        pic->moverArriba();
    }
    if (piccoloS_presionada) {
        pic->moverAbajo();
    }
    if (piccoloJ_presionada) {
        pic->iniciarCargaRayo();
    }
    if (piccoloL_presionada) {
        pic->iniciarCargaGravityBlast();
    }
    if (piccoloK_presionada) {
        pic->iniciarCargaKick();
    }
}

// Alterna la visibilidad de los hitboxes de los personajes y ataques
void game::alternarHitboxKamehameha()
{
    // Alternar hitbox del personaje
    if (p->estaHitboxVisible()) {
        p->ocultarHitbox();
    } else {
        p->mostrarHitbox();
    }

    // Alternar hitbox de Kamehameha
    Kamehameha::alternarVisualizacionHitbox();

    // Alternar hitbox de BlastB
    BlastB::alternarVisualizacionHitbox();
}

// Configura la imagen de fondo de la escena
void game::configurarFondo()
{
    // Limpiar fondo anterior
    if (fondoItem) {
        scene->removeItem(fondoItem);
        delete fondoItem;
        fondoItem = nullptr;
    }

    QPixmap fondoPixmap;
    QString fondoActualPath = fondosDisponibles[fondoActual];

    // Si la ruta es "degradado", crear un fondo degradado
    if (fondoActualPath == "degradado") {
        QRectF escenaRect = scene->sceneRect();
        fondoPixmap = QPixmap(escenaRect.width(), escenaRect.height());

        QPainter painter(&fondoPixmap);
        QLinearGradient gradient(0, 0, 0, escenaRect.height());
        gradient.setColorAt(0, QColor(135, 206, 235));
        gradient.setColorAt(0.7, QColor(255, 255, 255));
        gradient.setColorAt(1, QColor(34, 139, 34));

        painter.fillRect(fondoPixmap.rect(), gradient);

        // Agregar nubes
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 180));

        painter.drawEllipse(100, 50, 80, 40);
        painter.drawEllipse(140, 40, 60, 30);
        painter.drawEllipse(120, 60, 50, 25);

        painter.drawEllipse(300, 80, 100, 50);
        painter.drawEllipse(350, 70, 70, 35);
        painter.drawEllipse(330, 90, 60, 30);

        painter.end();
    } else {
        // Cargar imagen de fondo desde recursos
        fondoPixmap = QPixmap(fondoActualPath);

        // Si no se puede cargar, usar un degradado como respaldo
        if (fondoPixmap.isNull()) {
            QRectF escenaRect = scene->sceneRect();
            fondoPixmap = QPixmap(escenaRect.width(), escenaRect.height());

            QPainter painter(&fondoPixmap);
            QLinearGradient gradient(0, 0, 0, escenaRect.height());
            gradient.setColorAt(0, QColor(135, 206, 235));
            gradient.setColorAt(0.7, QColor(255, 255, 255));
            gradient.setColorAt(1, QColor(34, 139, 34));
            painter.fillRect(fondoPixmap.rect(), gradient);
            painter.end();
        }
    }

    // Reducir la imagen de fondo a la mitad
    QSize tamañoImagen = fondoPixmap.size();
    QSize tamañoReducido = QSize(tamañoImagen.width() / 2, tamañoImagen.height() / 2);
    fondoPixmap = fondoPixmap.scaled(tamañoReducido, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Establecer el tamaño de la escena al tamaño de la imagen reducida
    scene->setSceneRect(0, 0, tamañoReducido.width(), tamañoReducido.height());

    // Colocar la imagen reducida en la escena
    fondoItem = new QGraphicsPixmapItem(fondoPixmap);
    fondoItem->setPos(0, 0);
    fondoItem->setZValue(-1000);
    scene->addItem(fondoItem);

    // Redimensionar la ventana al tamaño de la imagen reducida
    resize(tamañoReducido);

    // Centrar la ventana en la pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - tamañoReducido.width()) / 2;
    int y = (screenGeometry.height() - tamañoReducido.height()) / 2;
    move(x, y);

    // Configurar la vista
    view->setSceneRect(scene->sceneRect());
    view->resetTransform();
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Forzar actualización de la ventana
    QApplication::processEvents();
    update();
    view->update();

    // Forzar reposicionamiento después de un breve retraso
    QTimer::singleShot(100, this, [this, tamañoReducido]() {
        resize(tamañoReducido);
        view->resetTransform();
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        update();
        view->update();
    });
}

// Cambia la imagen de fondo al siguiente fondo disponible
void game::cambiarFondo()
{
    fondoActual = (fondoActual + 1) % fondosDisponibles.size();
    configurarFondo();
}

// Maneja el evento de redimensionamiento de la ventana
void game::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Ajustar la vista y los límites del personaje al redimensionar la ventana
    if (view && scene && fondoItem) {
        view->resetTransform();

        if (isFullScreen()) {
            view->setSceneRect(scene->sceneRect());
            view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        } else {
            view->setSceneRect(scene->sceneRect());
            view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }

        if (p) {
            QRectF limitesJuego = scene->sceneRect();
            p->establecerLimitesEscena(limitesJuego);
        }
    }
}
