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
        } else {
            pic->mostrarHitbox();
            Rayo::alternarVisualizacionHitbox();
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
        if (!piccoloJ_presionada && !p->estaCargandoKamehameha()) {
            piccoloJ_presionada = true;
        }
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
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        pic->iniciarAnimacionIdle();
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
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = true; // Diagonal izquierda
            piccoloW_presionada = true; // Adelante (diagonal)
        } else { // Si ya no está en esta fase, salta a la transición de la siguiente
            cntPiccolo = 499; // ¡Ajustado! Salta al tick anterior al case 500
            // Las variables de movimiento y animación idle ya se manejarán en el case 500
        }
        break;
    case 170: // Detiene movimiento 'W' y 'A' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloW_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 180: // Inicia movimiento D (¡DURACIÓN DOBLE!)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = true; // Derecha (movimiento lateral largo)
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 200: // Detiene movimiento D después de 20 ticks (duración doble)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 210: // Ataque al nivel del suelo (Inicia ataque - Ajustado por +20 de offset)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 246: // Detiene ataque (36 ticks después de 210)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 247: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 256: // Inicia S+A (Atrás-izquierda, Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = true; // Diagonal atrás izquierda
            piccoloS_presionada = true; // Atrás (diagonal)
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 266: // Detiene movimiento 'S' y 'A' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloS_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 267: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 280: // Inicia movimiento D (¡DURACIÓN DOBLE!)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = true; // Movimiento lateral largo derecha
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 300: // Detiene movimiento D después de 20 ticks (duración doble)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 310: // Ataque al nivel del suelo (Inicia ataque - Ajustado por +20)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 346: // Detiene ataque (36 ticks después de 310)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 347: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 350: // Inicia movimiento recto W (DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloW_presionada = true; // Adelante (movimiento recto)
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 360: // Detiene movimiento 'W' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloW_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 361: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 365: // Nuevo inicio de ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 401: // Detiene ataque (36 ticks después de 365)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 402: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 405: // Inicia S+D después del ataque (Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloS_presionada = true; // Atrás
            piccoloD_presionada = true; // Diagonal atrás derecha
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 415: // Detiene movimiento 'S' y 'D' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloS_presionada = false;
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 416: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 420: // NUEVO: Inicia movimiento A (¡DURACIÓN DOBLE!)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = true; // Izquierda (movimiento atravesado)
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 440: // NUEVO: Detiene movimiento A después de 20 ticks
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 441: // NUEVO: Ataque atravesado (J)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 477: // NUEVO: Detiene ataque (36 ticks después de 441)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 478: // NUEVO: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 499; // ¡Ajustado!
        }
        break;
    case 480: // Pequeña pausa antes de reiniciar el ciclo (Ajustado por offset)
        if (pic->getVida() <= (pic->getVidaMaxima() * 0.75)) { // Mismo condicional para la transición
            // Aquí se mantiene el setPos y el reinicio de flags, porque 480 es donde ya se decide la transición
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);
            pic->iniciarAnimacionIdle();
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
            cntPiccolo = 509; // Salta al inicio de la Fase 2 si la vida es menor o igual al 75%
        }
        break;
    case 500: // Ajuste del reinicio o transición de fase (Ajustado por offset)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.75)) {
            cntPiccolo = 159; // Repite la secuencia si la vida sigue alta
        }
        // --- Transición a la siguiente fase ---
        else if (pic->getVida() > (pic->getVidaMaxima() * 0.40)) {
            // Regresar a la posición original
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);
            pic->iniciarAnimacionIdle();

            cntPiccolo = 509; // Salta al *siguiente tick* del inicio de la Fase 2 (preparado para 510)
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
        }
        break;

        // --- Fase 2: 40% < Vida <= 75% (Ahora con un offset acumulado de +100) ---
    case 510: // Inicia movimiento (Izquierda) (¡DURACIÓN DOBLE!)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = true; // Izquierda
        } else { // Si ya no está en esta fase, salta a la transición de la siguiente
            cntPiccolo = 769; // ¡Ajustado! Salta al tick anterior al case 770
            // Las variables de movimiento y animación idle ya se manejarán en el case 770
        }
        break;
    case 530: // Detiene movimiento 'A' después de 20 ticks (duración doble)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 540: // Ataque al nivel del suelo (Inicia ataque - Ajustado por +20 de offset)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 576: // Detiene ataque (36 ticks después de 540)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 577: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 580: // Inicia movimiento W+D después del ataque (Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloW_presionada = true; // Adelante
            piccoloD_presionada = true; // Diagonal adelante derecha
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 590: // Detiene movimiento 'W' y 'D' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloW_presionada = false;
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 591: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 595: // Ataque original (Inicia ataque)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 631: // Detiene ataque (36 ticks después de 595)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 632: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 635: // Inicia S después del ataque (Solo Atrás, DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloS_presionada = true; // Atrás (movimiento recto)
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 645: // Detiene movimiento 'S' después de 10 ticks (duración normal)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloS_presionada = false;
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 646: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 650: // Otro movimiento 'A' (Izquierda) (DURACIÓN NORMAL)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = true; // Izquierda
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 660: // Detiene 'A' y ataca (Ajustado por offset)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloA_presionada = false;
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 696: // Detiene ataque (36 ticks después de 660)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 697: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 700: // Segundo ataque en esta sección (Inicia ataque)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = true; // Segundo Ataque
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 736: // Detiene segundo ataque (36 ticks después de 700)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 737: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 740: // Inicia D después de los ataques (Solo Derecha) (¡DURACIÓN DOBLE!)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = true; // Derecha
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 760: // Detiene D después de 20 ticks (duración doble)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 761: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40) && pic->getVida() <= (pic->getVidaMaxima() * 0.75)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 769; // ¡Ajustado!
        }
        break;
    case 770: // Ajuste del reinicio o transición de fase (Ajustado por offset)
        if (pic->getVida() > (pic->getVidaMaxima() * 0.40)) {
            cntPiccolo = 509; // Repite la secuencia si la vida sigue en esta fase
        }
        // --- Transición a la siguiente fase ---
        else if (pic->getVida() > 0) {
            // Regresar a la posición original
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y);
            pic->iniciarAnimacionIdle();

            cntPiccolo = 779; // Salta al *siguiente tick* del inicio de la Fase 3 (preparado para 780)
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
        }
        break;

        // --- Fase 3: Vida <= 40% (hasta que se acabe) (Ahora con un offset acumulado de +150) ---
    case 780: // Inicia movimiento (Adelante-derecha, Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloW_presionada = true; // Adelante (movimiento evasivo, diagonal)
            piccoloD_presionada = true; // Diagonal adelante derecha
        } else { // Si la vida es 0 o menos, asume que terminó y no hace nada aquí.
            cntPiccolo = 1069; // ¡Ajustado! Salta al tick anterior al case 1070
            // Opcional: Podrías añadir un case para el final del juego si Piccolo muere en esta fase
            // cntPiccolo = SOME_END_GAME_CASE; // Ejemplo
        }
        break;
    case 790: // Detiene movimiento 'W' y 'D' después de 10 ticks (duración normal)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloW_presionada = false;
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 791: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 795: // Ataque frecuente (Inicia ataque)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = true; // Ataque frecuente
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 831: // Detiene ataque (36 ticks después de 795)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = false; // Corregido el typo aquí
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 832: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 835: // Inicia S+A (Atrás-izquierda, Diagonal, DURACIÓN NORMAL)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloA_presionada = true; // Diagonal atrás izquierda
            piccoloS_presionada = true; // Atrás (movimiento evasivo, diagonal)
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 845: // Detiene movimiento 'S' y 'A' después de 10 ticks (duración normal)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloS_presionada = false;
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 846: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 850: // Otro ataque (Inicia ataque)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = true; // Otro ataque
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 886: // Detiene ataque (36 ticks después de 850)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 887: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 890: // Inicia W después del ataque (Solo Adelante, DURACIÓN NORMAL)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloW_presionada = true; // Adelante (movimiento recto)
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 900: // Detiene movimiento 'W' después de 10 ticks (duración normal)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloW_presionada = false;
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 901: // Nueva: Llama a idle después de un movimiento
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 905: // Ataque al nivel del suelo (Inicia ataque)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 941: // Detiene el ataque (36 ticks después de 905)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 942: // Nueva: Llama a idle después de un ataque
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 945: // NUEVO: Inicia movimiento D (¡DURACIÓN DOBLE!)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloD_presionada = true; // Derecha (movimiento atravesado)
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 965: // NUEVO: Detiene movimiento D después de 20 ticks
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloD_presionada = false;
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 966: // NUEVO: Ataque atravesado (J)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1002: // NUEVO: Detiene ataque (36 ticks después de 966)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1003: // NUEVO: Llama a idle después de un ataque
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1005: // NUEVO: Inicia movimiento A (¡DURACIÓN DOBLE!)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloA_presionada = true; // Izquierda (movimiento atravesado)
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1025: // NUEVO: Detiene movimiento A después de 20 ticks
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloA_presionada = false;
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1026: // NUEVO: Ataque atravesado (J)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = true; // Ataque
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1062: // NUEVO: Detiene ataque (36 ticks después de 1026)
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            piccoloJ_presionada = false;
            pic->detenerCargaRayo();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1063: // NUEVO: Llama a idle después de un ataque
        if (pic->getVida() > 0 && pic->getVida() <= (pic->getVidaMaxima() * 0.40)) {
            pic->iniciarAnimacionIdle();
        } else {
            cntPiccolo = 1069; // ¡Ajustado!
        }
        break;
    case 1070: // Reubicado el salto de fase para después del último ataque (Ajustado por offset)
        if (pic->getVida() > 0) {
            // Al finalizar la última fase de ataques o si la vida sigue en esta fase pero se completó el ciclo
            // Esto asegura que Piccolo regresa a la posición original y se resetea para el siguiente ciclo
            pic->setPos(POSICION_ORIGINAL_X, POSICION_ORIGINAL_Y); // Asegúrate que estas constantes existen
            pic->iniciarAnimacionIdle();
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
            cntPiccolo = 779; // Repite esta fase hasta que la vida llegue a 0
        }
        break;

        // --- Otros casos y lógica general ---
    case 578: // Lógica original para animacion Idle (Ajustado por offset en Fase 2)
        pic->iniciarAnimacionIdle();
        break;
    default:
        // Asegúrate de que las teclas estén desactivadas si no hay un movimiento específico en el caso
        if (cntPiccolo < 160 || (cntPiccolo > 500 && cntPiccolo < 510) || (cntPiccolo > 770 && cntPiccolo < 780) || cntPiccolo > 1070) {
            piccoloW_presionada = false;
            piccoloA_presionada = false;
            piccoloS_presionada = false;
            piccoloD_presionada = false;
            piccoloJ_presionada = false;
            pic->iniciarAnimacionIdle(); // Llama a idle en las pausas generales
        }
        break;
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
