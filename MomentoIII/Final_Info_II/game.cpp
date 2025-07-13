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

game::game(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::game), fondoItem(nullptr), fondoActual(0)
{
    ui->setupUi(this);

    // Inicializar lista de fondos disponibles
    fondosDisponibles << ":/Fondos/Sprites/gui_scenes/torneo.png"
                      << ":/Fondos/Sprites/gui_scenes/torneo2.png";
    
    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);

    view->setScene(scene);
    
    // Configurar la vista normalmente
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setDragMode(QGraphicsView::NoDrag);
    
    // Hacer que la vista ocupe toda la ventana principal
    setCentralWidget(view);
    
    // Eliminar barra de estado y menú
    statusBar()->setVisible(false);
    menuBar()->setVisible(false);
    
    // Ocultar todos los bordes y decoraciones de la ventana
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowState(Qt::WindowNoState);
    
    // Eliminar todos los márgenes y bordes de la vista
    view->setContentsMargins(0, 0, 0, 0);
    view->setFrameStyle(QFrame::NoFrame);
    view->setStyleSheet("QGraphicsView { border: none; margin: 0px; padding: 0px; }");
    
    // Eliminar barras de scroll para que no aparezcan
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Eliminar márgenes de la ventana principal también
    setContentsMargins(0, 0, 0, 0);
    
    // Configurar el fondo primero para obtener las dimensiones
    configurarFondo();

    p = new Goku();
    scene->addItem(p);
    p->setPos(50, 250); // Mucho más a la izquierda (150→50) y mismo nivel arriba
    
    // Hacer Goku más grande (3.5x el tamaño original - 350%)
    p->setScale(3.5);
    
    pic = new Piccolo();
    scene->addItem(pic);
    pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);

    // Hacer Piccolo más grande (3.5x el tamaño original - 350%)
    pic->setScale(3.5);
    pic->establecerObjetivo(p);

    // Sprite de la cara de Goku
    carapersonaje = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/caragoku.png"));
    scene->addItem(carapersonaje);
    carapersonaje->setPos(20, 10); // Fijo arriba a la izquierda

    // Barra de vida, a la derecha de la cara
    barraVida = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/vida4.png"));
    scene->addItem(barraVida);
    int offsetX = carapersonaje->pixmap().width() + 10; // 15 píxeles de espacio
    barraVida->setPos(10 + offsetX, 10);

    //Barra Ki
    barraKi = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/kibar0.png"));
    scene->addItem(barraKi);
    int offsetY = barraVida->pixmap().height() + 5; // Debajo de la barra de vida
    barraKi->setPos(10 + offsetX, 10 + offsetY);

    // Conectar signal de vida de Goku a la barra
    connect(p, &Personaje::vidaCambiada, this, &game::actualizarBarraVida);

    // Conectar singal de ki de goku a la barra
    connect(p, &Goku::kiCambiado, this, &game::actualizarBarraKi);

    //
    transform.scale(-1, 1); // Refleja sobre el eje X
    // Sprite de la cara de Piccolo
    carapersonaje2 = new QGraphicsPixmapItem(QPixmap(":/Fondos/Sprites/gui_scenes/carapic.png"));
    scene->addItem(carapersonaje2);
    carapersonaje2->setPos(895, 10); // Fijo arriba a la izquierda

    // Barra de vida, a la derecha de la cara
    barraVida2 = new QGraphicsPixmapItem((QPixmap(":/Fondos/Sprites/gui_scenes/vida4.png")).transformed(transform));
    scene->addItem(barraVida2);
    int offsetX2 = (barraVida2->pixmap().width()); // 15 píxeles de espacio
    barraVida2->setPos(895 - offsetX2, 10);

    //Barra Ki
    barraKi2 = new QGraphicsPixmapItem((QPixmap(":/Fondos/Sprites/gui_scenes/kibar5.png")).transformed(transform));
    scene->addItem(barraKi2);
    int offsetX3 = barraKi2->pixmap().width(); // Debajo de la barra de vida
    int offsetY2 = barraVida2->pixmap().height() + 5; // Debajo de la barra de vida
    barraKi2->setPos(895 - offsetX3, 10 + offsetY2);

    // Conectar signal de vida de Piccolo a la barra
    connect(pic, &Personaje::vidaCambiada, this, &game::actualizarBarraVida2);
    
    // Configurar timer para movimiento continuo
    movimientoTimer = new QTimer(this);
    movimientoTimer->setInterval(50); // 20 FPS para movimiento suave
    connect(movimientoTimer, &QTimer::timeout, this, &game::actualizarMovimiento);
    // No iniciar automáticamente, solo cuando sea necesario

    // Configurar timer para movimiento Piccolo
    // piccoloMovTimer = new QTimer(this);
    // piccoloMovTimer->setInterval(50); // 20 FPS para movimiento suave
    // connect(piccoloMovTimer, &QTimer::timeout, this, &game::piccoloActualizarMovimiento);
    //piccoloMovTimer->start();

    cntPiccolo = 0;

    piccoloIATimer = new QTimer(this);
    piccoloIATimer->setInterval(25);
    connect(piccoloIATimer, &QTimer::timeout, this, &game::piccoloActualizarMovimiento);
    piccoloIATimer->start();

    // Configurar límites de escena para las colisiones
    QRectF limitesJuego = scene->sceneRect(); // Usar exactamente los límites de la escena
    p->establecerLimitesEscena(limitesJuego);
    
    // Opcional: Escalar Goku (1.5 = 150% del tamaño original)
    // p->establecerEscala(1.5);

    // Agregar grilla temporal para debug de posicionamiento
    agregarGrillaDebug();
    
    // Opcional: Iniciar maximizado
    // showMaximized();
}

game::~game()
{
    // Limpiar fondo
    if (fondoItem) {
        scene->removeItem(fondoItem);
        delete fondoItem;
        fondoItem = nullptr;
    }
    
    delete ui;
}

void game::keyPressEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) return;

    // Si Goku está cargando ki, ignorar otras acciones excepto soltar la carga (por ejemplo, tecla K)
    if (p->estaRecargandoKi()) {
        // Permitir solo la tecla K (para detener la carga)
        if (e->key() != Qt::Key_K) {
            return;
        }
    }


    switch (e->key()) {
    // --- Movimiento ---
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

    // --- Ataques cuerpo a cuerpo ---
    case Qt::Key_X:
        p->golpear();
        break;
    case Qt::Key_C:
        p->patear();
        break;

    // --- Salto direccional ---
    case Qt::Key_Space:
        if (!p->estaSaltando() && p->isVisible() && !p->estaEnAnimacionEntrada() && !p->estaRecargandoKi() && !p->estaCargandoKamehameha()) {
            saltoDireccional();
        }
        break;

    // --- Visualización y debug ---
    case Qt::Key_H:
        // Alternar hitboxes
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
            qDebug() << "Todos los hitboxes ocultos";
        } else {
            pic->mostrarHitbox();
            Rayo::alternarVisualizacionHitbox();
            GravityBlast::alternarVisualizacionHitbox();
            qDebug() << "Todos los hitboxes visibles";
        }
        break;
    case Qt::Key_G:
        alternarGrillaDebug();
        break;
    case Qt::Key_B:
        cambiarFondo();
        break;

    // --- Pantalla completa ---
    case Qt::Key_F11:
        if (isFullScreen()) showNormal();
        else showFullScreen();
        break;

    // --- Ki y poderes ---
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

    // --- Pruebas ---
    case Qt::Key_Z:
        break;
    }
}
void game::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) return;

    switch (e->key()) {
    // --- Movimiento ---
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

    // --- Ki y poderes ---
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

    // --- Pruebas ---
    case Qt::Key_Z:
        if(pic->getFase() == false){
            cntPiccolo = 501; pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y); pic->setScale(5.5); pic->alternarFase();
        }
        break;
    }

    // --- Detener movimiento e iniciar idle si no hay teclas de movimiento presionadas ---
    if (!teclaD_presionada && !teclaA_presionada && !teclaW_presionada && !teclaS_presionada) {
        movimientoTimer->stop();
        if (!p->estaSaltando()) {
            p->iniciarAnimacionIdle();
        }
    }
}
void game::saltoDireccional()
{
    // Configurar velocidad horizontal inicial del salto según las teclas presionadas
    qreal velocidadHorizontalInicial = 0.0;
    qreal velocidadSaltoVertical = 65.0; // Velocidad de salto normal aumentada
    
    if (teclaD_presionada) {
        velocidadHorizontalInicial += 40.0; // Velocidad horizontal aumentada hacia la derecha
        qDebug() << "Salto parabólico hacia la DERECHA";
    }
    
    if (teclaA_presionada) {
        velocidadHorizontalInicial -= 40.0; // Velocidad horizontal aumentada hacia la izquierda
        qDebug() << "Salto parabólico hacia la IZQUIERDA";
    }
    
    if (teclaW_presionada) {
        velocidadSaltoVertical = 85.0; // Súper salto vertical aumentado
        qDebug() << "Súper salto vertical";
    }
    
    if (teclaS_presionada) {
        velocidadSaltoVertical = 45.0; // Salto corto aumentado
        qDebug() << "Salto corto";
    }
    
    // Configurar las velocidades antes de iniciar el salto
    p->establecerVelocidadSalto(velocidadSaltoVertical);
    p->establecerVelocidadHorizontalSalto(velocidadHorizontalInicial);
    
    // Iniciar el salto con las velocidades configuradas
    p->saltar();
    
    // Debug información del salto
    if (velocidadHorizontalInicial != 0.0) {
        qDebug() << "Salto parabólico iniciado - velV:" << velocidadSaltoVertical << "velH:" << velocidadHorizontalInicial;
    } else {
        qDebug() << "Salto vertical iniciado - vel:" << velocidadSaltoVertical;
    }
}

void game::actualizarMovimiento()
{
    // Debug para ver si el timer está funcionando (reducido)
    static int debugCounter = 0;
    if (debugCounter % 60 == 0) { // Debug cada 60 frames (1 segundo aprox)
        qDebug() << "actualizarMovimiento() - D:" << teclaD_presionada << "A:" << teclaA_presionada << "saltando:" << p->estaSaltando();
    }
    debugCounter++;
    
    // Solo aplicar movimiento si no está saltando
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


void game::actualizarBarraVida(int vidaActual, int vidaMaxima) {
    vidaActual = ((vidaActual * 4) + vidaMaxima -1) / vidaMaxima;
    barraVida->setPixmap(QPixmap(QString(":/Fondos/Sprites/gui_scenes/vida%1.png").arg(vidaActual)));
}

void game::actualizarBarraVida2(int vidaActual, int vidaMaxima) {
    if (vidaActual <= 0 && !piccoloDerrotado) {
        piccoloDerrotado = true;
        manejarDerrotaPiccolo();
    }
    vidaActual = ((vidaActual * 4) + vidaMaxima -1) / vidaMaxima;
    barraVida2->setPixmap((QPixmap(QString(":/Fondos/Sprites/gui_scenes/vida%1.png").arg(vidaActual))).transformed(transform));
    piccoloW_presionada = false;
    piccoloA_presionada = false;
    piccoloS_presionada = false;
    piccoloD_presionada = false;
    piccoloJ_presionada = false;
    cntPiccolo -= 3;
}

void game::actualizarBarraKi(int kiActual, int /*kiMaximo*/) {
    int spriteIndex = kiActual / 20; // 0-19:0, 20-39:1, ..., 100:5
    if (spriteIndex > 5) spriteIndex = 5;
    barraKi->setPixmap(QPixmap(QString(":/Fondos/Sprites/gui_scenes/kibar%1.png").arg(spriteIndex)));
}

// <<<<<<<<<<<< IMPLEMENTACIÓN DE LOS CAMBIOS DE PICCOLO >>>>>>>>>>>>>>><

void game::manejarDerrotaPiccolo() {
    if (nivelActual == 1) {
        nivelActual = 2;

        // Cambiar fondo
        cambiarFondo();
        p->establecerCarpetaSprites("goku2");


        // Mostrar mensaje opcional
        qDebug() << "¡Nivel 2 comenzado!";
    } else {
        // Juego terminado
        qDebug() << "¡Ganaste el juego!";


    }
}


void game::piccoloActualizarMovimiento()
{
    // Debug para ver si el timer está funcionando
    qDebug() << "piccoloActualizarMovimiento() llamado - D:" << teclaD_presionada << "A:" << teclaA_presionada << "saltando:" << p->estaSaltando();

    // Código para hacer los cambios de movimiento de Piccolo
    switch (cntPiccolo) {
    case 10: // Inicia con animación idle
        pic->iniciarAnimacionIdle();
        break;
    // --- Fase 1: Vida > 75% ---
    case 160: // Inicia movimiento: Adelante-izquierda (Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > 0) {
            piccoloA_presionada = true; // Diagonal izquierda
            piccoloW_presionada = true; // Adelante (diagonal)
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 170: // Detiene movimiento 'W' y 'A' después de 10 ticks (duración normal)
        if (pic->getVida() > 0) {
            piccoloW_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 171: // NUEVO: Pausa de 10 ticks después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 181: // Inicia movimiento D (¡DURACIÓN DOBLE!) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloD_presionada = true; // Derecha (movimiento lateral largo)
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 201: // Detiene movimiento D después de 20 ticks (duración doble)
        if (pic->getVida() > 0) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 202: // NUEVO: Pausa de 10 ticks después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    // --- REEMPLAZO: Ataque rápido de 1 tick (Gravity Blast) ---
    case 212: // **NUEVO ATAQUE RÁPIDO: Inicia Gravity Blast (1 tick)** - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloL_presionada = true; // Inicia el ataque de 1 tick
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 213: // **NUEVO ATAQUE RÁPIDO: Detiene Gravity Blast (1 tick después)**
        if (pic->getVida() > 0) {
            piccoloL_presionada = false;
            pic->detenerCargaGravityBlast(); // Detiene la carga del Gravity Blast
            pic->iniciarAnimacionIdle(); // Vuelve a idle inmediatamente
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    // --- FIN REEMPLAZO ---
    case 214: // NUEVO: Pausa de 10 ticks después del ataque rápido
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 224: // Inicia S+A (Atrás-izquierda, Diagonal, DURACIÓN NORMAL) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloA_presionada = true; // Diagonal atrás izquierda
            piccoloS_presionada = true; // Atrás (diagonal)
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 234: // Detiene movimiento 'S' y 'A' después de 10 ticks (duración normal)
        if (pic->getVida() > 0) {
            piccoloS_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 235: // Llama a idle después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 245: // Inicia movimiento D (¡DURACIÓN DOBLE!) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloD_presionada = true; // Movimiento lateral largo derecha
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 265: // Detiene movimiento D después de 20 ticks (duración doble)
        if (pic->getVida() > 0) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 266: // NUEVO: Pausa de 10 ticks después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 276: // Ataque al nivel del suelo (Inicia ataque - Ajustado por +20 +10)
        if (pic->getVida() > 0) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 312: // Detiene ataque (36 ticks después de 276)
        if (pic->getVida() > 0) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 313: // Llama a idle después de un ataque
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 314: // NUEVO: Pausa de 10 ticks después de un ataque
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 324: // Inicia movimiento recto W (DURACIÓN NORMAL) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloW_presionada = true; // Adelante (movimiento recto)
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 334: // Detiene movimiento 'W' después de 10 ticks (duración normal)
        if (pic->getVida() > 0) {
            piccoloW_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 335: // Llama a idle después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 345: // Nuevo inicio de ataque - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 381: // Detiene ataque (36 ticks después de 345)
        if (pic->getVida() > 0) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 382: // Llama a idle después de un ataque
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 383: // NUEVO: Pausa de 10 ticks después de un ataque
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 393: // Inicia S+D después del ataque (Diagonal, DURACIÓN NORMAL) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloS_presionada = true; // Atrás
            piccoloD_presionada = true; // Diagonal atrás derecha
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 403: // Detiene movimiento 'S' y 'D' después de 10 ticks (duración normal)
        if (pic->getVida() > 0) {
            piccoloS_presionada = false;
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 404: // Llama a idle después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 414: // NUEVO: Inicia movimiento A (¡DURACIÓN DOBLE!) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloA_presionada = true; // Izquierda (movimiento atravesado)
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 434: // NUEVO: Detiene movimiento A después de 20 ticks
        if (pic->getVida() > 0) {
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 435: // NUEVO: Pausa de 10 ticks después de un movimiento
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 445: // NUEVO: Ataque atravesado (J) - Ajustado para la pausa
        if (pic->getVida() > 0) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 481: // NUEVO: Detiene ataque (36 ticks después de 445)
        if (pic->getVida() > 0) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 482: // NUEVO: Llama a idle después de un ataque
        if (pic->getVida() > 0) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 483: // Pequeña pausa antes de reiniciar el ciclo (Ajustado por offset)
        if (pic->getVida() > 0) { // Si Piccolo sigue vivo, repite la secuencia de la Fase 1
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y); // Reset position if needed
            pic->iniciarAnimacionIdle(); // Ensure idle animation
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
            piccoloL_presionada = false;
            cntPiccolo = 159; // Vuelve al inicio de la Fase 1
        } else {
            cntPiccolo = 499; // Salta al último case si no tiene vida
        }
        break;
    case 500: // Este case ya no se usará para transición, solo para el ciclo principal de Fase 1
        // La lógica de repetición para la Fase 1 si Piccolo sigue vivo está ahora en case 483.
        // Si cntPiccolo llega aquí, es un caso de seguridad, o si Piccolo murió antes del 483.
        // Si Piccolo murió, el timer eventualmente se detendrá o no hará nada.
        // Si por alguna razón llega aquí y sigue vivo, se redirige al inicio.
        if (pic->getVida() > 0) {
            cntPiccolo = 159; // Asegura que vuelva a la Fase 1 si se salta el 483 por algún motivo.
            pic->setScale(5.5);
        }
        else{ pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y); pic->alternarFase(); } // Se llama aquí si la vida es 0 o menos.
        break;

        // Todas las fases superiores a la primera han sido eliminadas.
        // La lógica de repetición ahora está contenida dentro de la Fase 1.
    }

    if (pic->isVisible() || !pic->estaEnAnimacionEntrada()) {
        cntPiccolo++;
    }

    // Movimeintos Piccolo
    if (piccoloD_presionada) {
        qDebug() << "Piccolo moverDerecha()";
        pic->moverDerecha();
        //piccoloD_presionada = false;
    }
    if (piccoloA_presionada) {
        qDebug() << "Piccolo moverIzquierda()";
        pic->moverIzquierda();
        //piccoloA_presionada = false;
    }
    if (piccoloW_presionada) {
        pic->moverArriba();
        //piccoloW_presionada = false;
    }
    if (piccoloS_presionada) {
        pic->moverAbajo();
        //piccoloS_presionada = false;
    }
    if (piccoloJ_presionada) {
        pic->iniciarCargaRayo();
        //piccoloJ_presionada = false;
    }
    if (piccoloL_presionada) {
        //pic->iniciarCargaRayo();
        pic->iniciarCargaGravityBlast();
        //piccoloJ_presionada = false;
    }
}


void game::agregarGrillaDebug()
{
    // Crear grilla de 10x10 píxeles para ayudar con el posicionamiento
    QPen penGrilla(Qt::lightGray, 0.5, Qt::DotLine);
    
    // Obtener límites de la escena
    QRectF limites = scene->sceneRect();
    
    // Líneas verticales cada 10 píxeles
    for (int x = limites.left(); x <= limites.right(); x += 10) {
        QGraphicsLineItem* linea = scene->addLine(x, limites.top(), x, limites.bottom(), penGrilla);
        linea->setZValue(-1000); // Poner muy atrás
        lineasGrilla.append(linea);
    }
    
    // Líneas horizontales cada 10 píxeles
    for (int y = limites.top(); y <= limites.bottom(); y += 10) {
        QGraphicsLineItem* linea = scene->addLine(limites.left(), y, limites.right(), y, penGrilla);
        linea->setZValue(-1000); // Poner muy atrás
        lineasGrilla.append(linea);
    }
    
    // Líneas más gruesas cada 50 píxeles para mejor referencia
    QPen penGrillaGruesa(Qt::gray, 1.0, Qt::DotLine);
    
    for (int x = limites.left(); x <= limites.right(); x += 50) {
        QGraphicsLineItem* linea = scene->addLine(x, limites.top(), x, limites.bottom(), penGrillaGruesa);
        linea->setZValue(-999);
        lineasGrilla.append(linea);
    }
    
    for (int y = limites.top(); y <= limites.bottom(); y += 50) {
        QGraphicsLineItem* linea = scene->addLine(limites.left(), y, limites.right(), y, penGrillaGruesa);
        linea->setZValue(-999);
        lineasGrilla.append(linea);
    }
    
    qDebug() << "Grilla de debug agregada - líneas cada 10px (finas) y 50px (gruesas). Presiona G para alternar.";
}

void game::alternarGrillaDebug()
{
    grillaVisible = !grillaVisible;
    
    for (QGraphicsLineItem* linea : lineasGrilla) {
        linea->setVisible(grillaVisible);
    }
    
    qDebug() << "Grilla de debug" << (grillaVisible ? "mostrada" : "oculta");
}

void game::alternarHitboxKamehameha()
{
    // Alternar hitbox del personaje
    if (p->estaHitboxVisible()) {
        p->ocultarHitbox();
        qDebug() << "Hitbox del personaje oculto";
    } else {
        p->mostrarHitbox();
        qDebug() << "Hitbox del personaje visible";
    }
    
    // Alternar hitbox de Kamehameha
    Kamehameha::alternarVisualizacionHitbox();
    
    // Alternar hitbox de BlastB
    BlastB::alternarVisualizacionHitbox();
    
    qDebug() << "Todos los hitboxes alternados";
}

void game::configurarFondo()
{
    // Limpiar fondo anterior si existe
    if (fondoItem) {
        scene->removeItem(fondoItem);
        delete fondoItem;
        fondoItem = nullptr;
    }
    
    QPixmap fondoPixmap;
    QString fondoActualPath = fondosDisponibles[fondoActual];
    
    if (fondoActualPath == "degradado") {
        // Crear fondo degradado
        qDebug() << "Creando fondo degradado";
        
        // Crear un pixmap del tamaño de la escena
        QRectF escenaRect = scene->sceneRect();
        fondoPixmap = QPixmap(escenaRect.width(), escenaRect.height());
        
        // Crear degradado
        QPainter painter(&fondoPixmap);
        QLinearGradient gradient(0, 0, 0, escenaRect.height());
        gradient.setColorAt(0, QColor(135, 206, 235)); // Azul cielo
        gradient.setColorAt(0.7, QColor(255, 255, 255)); // Blanco
        gradient.setColorAt(1, QColor(34, 139, 34)); // Verde hierba
        
        painter.fillRect(fondoPixmap.rect(), gradient);
        
        // Agregar algunas nubes simples
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 180)); // Blanco semi-transparente
        
        // Nube 1
        painter.drawEllipse(100, 50, 80, 40);
        painter.drawEllipse(140, 40, 60, 30);
        painter.drawEllipse(120, 60, 50, 25);
        
        // Nube 2
        painter.drawEllipse(300, 80, 100, 50);
        painter.drawEllipse(350, 70, 70, 35);
        painter.drawEllipse(330, 90, 60, 30);
        
        painter.end();
        
        qDebug() << "Fondo degradado creado con dimensiones:" << fondoPixmap.size();
    } else {
        // Intentar cargar imagen de fondo desde recursos
        fondoPixmap = QPixmap(fondoActualPath);
        
        if (fondoPixmap.isNull()) {
            qDebug() << "No se pudo cargar el fondo:" << fondoActualPath << "- usando degradado";
            // Crear fondo degradado como respaldo
            QRectF escenaRect = scene->sceneRect();
            fondoPixmap = QPixmap(escenaRect.width(), escenaRect.height());
            
            QPainter painter(&fondoPixmap);
            QLinearGradient gradient(0, 0, 0, escenaRect.height());
            gradient.setColorAt(0, QColor(135, 206, 235)); // Azul cielo
            gradient.setColorAt(0.7, QColor(255, 255, 255)); // Blanco
            gradient.setColorAt(1, QColor(34, 139, 34)); // Verde hierba
            painter.fillRect(fondoPixmap.rect(), gradient);
            painter.end();
        } else {
            qDebug() << "Fondo cargado exitosamente:" << fondoActualPath << "- dimensiones:" << fondoPixmap.size();
        }
    }
    
    // Obtener el tamaño exacto de la imagen
    QSize tamañoImagen = fondoPixmap.size();
    
    // Reducir la imagen a la mitad
    QSize tamañoReducido = QSize(tamañoImagen.width() / 2, tamañoImagen.height() / 2);
    fondoPixmap = fondoPixmap.scaled(tamañoReducido, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Hacer la escena exactamente del tamaño de la imagen reducida
    scene->setSceneRect(0, 0, tamañoReducido.width(), tamañoReducido.height());
    
    // Colocar la imagen reducida directamente en la escena
    fondoItem = new QGraphicsPixmapItem(fondoPixmap);
    fondoItem->setPos(0, 0);
    fondoItem->setZValue(-1000);
    scene->addItem(fondoItem);
    
    // Hacer la ventana exactamente del tamaño de la imagen reducida
    // Sin bordes, no necesitamos píxeles extra
    resize(tamañoReducido);
    
    // Centrar la ventana en la pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - tamañoReducido.width()) / 2;
    int y = (screenGeometry.height() - tamañoReducido.height()) / 2;
    move(x, y);
    
    // Configurar la vista correctamente desde el inicio
    view->setSceneRect(scene->sceneRect());
    view->resetTransform();
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    
    // Forzar un refresh completo de la ventana
    QApplication::processEvents();
    update();
    view->update();
    
    // Forzar reposicionamiento para que se acomode correctamente
    QTimer::singleShot(100, this, [this, tamañoReducido]() {
        resize(tamañoReducido);
        view->resetTransform();
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        update();
        view->update();
    });
    
    qDebug() << "Ventana y imagen del mismo tamaño - Original:" << tamañoImagen << "- Reducido:" << tamañoReducido;
    
    qDebug() << "Fondo configurado y agregado a la escena";
}

void game::cambiarFondo()
{
    // Cambiar al siguiente fondo
    fondoActual = (fondoActual + 1) % fondosDisponibles.size();
    
    qDebug() << "Cambiando al fondo" << (fondoActual + 1) << "de" << fondosDisponibles.size() << ":" << fondosDisponibles[fondoActual];
    
    // Configurar el nuevo fondo
    configurarFondo();
}

void game::resizeEvent(QResizeEvent *event)
{
    // Llamar al método padre primero
    QMainWindow::resizeEvent(event);
    
    // Manejar el redimensionamiento (incluyendo maximizar)
    if (view && scene && fondoItem) {
        // Resetear cualquier transformación previa
        view->resetTransform();
        
        // Ajustar la vista según el estado de la ventana
        if (isFullScreen()) {
            // En pantalla completa, ignorar aspect ratio para llenar toda la pantalla
            view->setSceneRect(scene->sceneRect());
            view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
        } else {
            // En ventana normal, mantener aspect ratio
            view->setSceneRect(scene->sceneRect());
            view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }
        
        // Actualizar los límites del personaje
        if (p) {
            QRectF limitesJuego = scene->sceneRect();
            p->establecerLimitesEscena(limitesJuego);
        }
        
        qDebug() << "Ventana redimensionada - Tamaño:" << size() << "- FullScreen:" << isFullScreen();
    }
}
