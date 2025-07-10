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
                      << "degradado"; // Opción especial para fondo degradado
    
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
    pic->setPos(700, 260);

    // Hacer Piccolo más grande (3.5x el tamaño original - 350%)
    pic->setScale(3.5);
    
    // Conectar señal de aterrizaje para movimiento continuo
    connect(p, &Personaje::personajeAterrizo, this, &game::verificarMovimientoContinuo);
    
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
    // Ignorar auto-repeat del teclado
    if (e->isAutoRepeat()) {
        return;
    }
    
    // Solo rastrear teclas de dirección y activar timer si es necesario
    if(e->key() == Qt::Key_D) { 
        teclaD_presionada = true;
        qDebug() << "Tecla D presionada - timer activo:" << movimientoTimer->isActive();
        if (!movimientoTimer->isActive()) {
            movimientoTimer->start();
            qDebug() << "Timer iniciado para D";
        }
    }
    if(e->key() == Qt::Key_A) { 
        teclaA_presionada = true;
        qDebug() << "Tecla A presionada - timer activo:" << movimientoTimer->isActive();
        if (!movimientoTimer->isActive()) {
            movimientoTimer->start();
            qDebug() << "Timer iniciado para A";
        }
    }
    if(e->key() == Qt::Key_W) { 
        teclaW_presionada = true;
        if (!movimientoTimer->isActive()) {
            movimientoTimer->start();
        }
    }
    if(e->key() == Qt::Key_S) { 
        teclaS_presionada = true;
        if (!movimientoTimer->isActive()) {
            movimientoTimer->start();
        }
    }
    
    // Salto direccional - no permitir si es invisible, durante animación de entrada, recargando ki o cargando Kamehameha
    if(e->key() == Qt::Key_Space && !p->estaSaltando() && p->isVisible() && !p->estaEnAnimacionEntrada() && !p->estaRecargandoKi() && !p->estaCargandoKamehameha()){ 
        saltoDireccional();
        qDebug() << "Salto direccional iniciado";
    }
    if(e->key() == Qt::Key_H){ 
        // Alternar visualización de todos los hitboxes con tecla H
        if (p->estaHitboxVisible()) {
            p->ocultarHitbox();
            // También ocultar hitboxes de Kamehameha y BlastB
            Kamehameha::alternarVisualizacionHitbox();
            BlastB::alternarVisualizacionHitbox();
            qDebug() << "Todos los hitboxes ocultos";
        } else {
            p->mostrarHitbox();
            // También mostrar hitboxes de Kamehameha y BlastB
            Kamehameha::alternarVisualizacionHitbox();
            BlastB::alternarVisualizacionHitbox();
            qDebug() << "Todos los hitboxes visibles";
        }
        // Alternar visualización de todos los hitboxes con tecla H
        if (pic->estaHitboxVisible()) {
            pic->ocultarHitbox();
            // También ocultar hitboxes de Kamehameha y BlastB
            Rayo::alternarVisualizacionHitbox();
            qDebug() << "Todos los hitboxes ocultos";
        } else {
            pic->mostrarHitbox();
            // También mostrar hitboxes de Kamehameha y BlastB
            Rayo::alternarVisualizacionHitbox();
            qDebug() << "Todos los hitboxes visibles";
        }
    }
    if(e->key() == Qt::Key_G){ 
        // Alternar visualización de grilla con tecla G
        alternarGrillaDebug();
    }
    if(e->key() == Qt::Key_B){ 
        // Cambiar al siguiente fondo con tecla B
        cambiarFondo();
    }
    if(e->key() == Qt::Key_F11){ 
        // Alternar pantalla completa con F11
        if (isFullScreen()) {
            showNormal();
            qDebug() << "Saliendo de pantalla completa";
        } else {
            showFullScreen();
            qDebug() << "Entrando en pantalla completa";
        }
    }
    if(e->key() == Qt::Key_K){ 
        // Iniciar recarga de ki al presionar K (solo si no está ya recargando)
        if (!teclaK_presionada && !p->estaRecargandoKi()) {
            teclaK_presionada = true;
            p->iniciarRecargaKi();
            qDebug() << "Recarga de ki iniciada (mantener K presionada)";
        }
    }
    if(e->key() == Qt::Key_J){ 
        // Iniciar carga de Kamehameha al presionar J (solo si no está ya cargando)
        if (!teclaJ_presionada && !p->estaCargandoKamehameha()) {
            teclaJ_presionada = true;
            p->iniciarCargaKamehameha();
            qDebug() << "Carga de Kamehameha iniciada (mantener J presionada)";
        }
    }
    if(e->key() == Qt::Key_L){ 
        // Iniciar animación de ráfaga al presionar L (solo si no está ya en ráfaga)
        if (!teclaL_presionada && !p->estaEnAnimacionRafaga()) {
            teclaL_presionada = true;
            p->iniciarAnimacionRafaga();
            qDebug() << "Animación de ráfaga iniciada (mantener L presionada)";
        }
    }
    // tecla para pruebitas
    if(e->key() == Qt::Key_Z){
        if (!piccoloJ_presionada && !p->estaCargandoKamehameha()){
            piccoloJ_presionada = true;
        }
    }
}

void game::keyReleaseEvent(QKeyEvent *e)
{
    // Ignorar auto-repeat del teclado
    if (e->isAutoRepeat()) {
        return;
    }
    
    // Rastrear cuando se sueltan las teclas y detener timer si es necesario
    if(e->key() == Qt::Key_D) { 
        teclaD_presionada = false;
    }
    if(e->key() == Qt::Key_A) { 
        teclaA_presionada = false;
    }
    if(e->key() == Qt::Key_W) { 
        teclaW_presionada = false;
    }
    if(e->key() == Qt::Key_S) { 
        teclaS_presionada = false;
    }
    if(e->key() == Qt::Key_K) { 
        // Detener recarga de ki al soltar K
        if (teclaK_presionada) {
            teclaK_presionada = false;
            p->detenerRecargaKi();
            qDebug() << "Tecla K liberada - deteniendo recarga de ki";
        }
    }
    if(e->key() == Qt::Key_J) { 
        // Detener carga de Kamehameha al soltar J
        if (teclaJ_presionada) {
            teclaJ_presionada = false;
            p->detenerCargaKamehameha();
            qDebug() << "Tecla J liberada - deteniendo carga de Kamehameha";
        }
    }
    if(e->key() == Qt::Key_L) { 
        // Detener animación de ráfaga al soltar L
        if (teclaL_presionada) {
            teclaL_presionada = false;
            p->detenerAnimacionRafaga();
            qDebug() << "Tecla L liberada - deteniendo animación de ráfaga";
        }
    }
    // tecla para pruebitas
    if(e->key() == Qt::Key_Z)
    {
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        pic->iniciarAnimacionIdle();
    }
    
    // Si no hay teclas presionadas, detener timer e iniciar idle
    if (!teclaD_presionada && !teclaA_presionada && !teclaW_presionada && !teclaS_presionada) {
        qDebug() << "Deteniendo timer - no hay teclas presionadas";
        movimientoTimer->stop();
        if (!p->estaSaltando()) {
            qDebug() << "Iniciando animación idle";
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

void game::verificarMovimientoContinuo()
{
    // Este método ya no es necesario, el timer se encarga del movimiento continuo
    qDebug() << "Personaje aterrizó - el timer se encargará del movimiento continuo";
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

void game::piccoloActualizarMovimiento()
{
    // Debug para ver si el timer está funcionando
    qDebug() << "piccoloActualizarMovimiento() llamado - D:" << teclaD_presionada << "A:" << teclaA_presionada << "saltando:" << p->estaSaltando();

    // Codigo para hacer los cambios de movimeinto de piccolo
    switch(cntPiccolo)
    {
    case 160:
        piccoloA_presionada = true;
        break;
    case 200:
        piccoloA_presionada = false;
        piccoloJ_presionada = true;
        break;
    case 236:
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        piccoloD_presionada = true;
        break;
    case 276:
        piccoloD_presionada = false;
        piccoloW_presionada = true;
        break;
    case 300:
        piccoloW_presionada = false;
        piccoloJ_presionada = true;
        break;
    case 336:
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        piccoloS_presionada = true;
        break;
    case 360:
        piccoloS_presionada = false;
        piccoloJ_presionada = true;
        break;
    case 396:
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        piccoloW_presionada = true;
        break;
    case 420:
        piccoloW_presionada = false;
        piccoloA_presionada = true;
        break;
    case 440:
        piccoloA_presionada = false;
        piccoloJ_presionada = true;
        break;
    case 476:
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        piccoloD_presionada = true;
        break;
    case 496:
        piccoloD_presionada = false;
        piccoloJ_presionada = true;
        break;
    case 532:
        piccoloJ_presionada = false;
        pic->detenerCargaRayo();
        piccoloS_presionada = true;
        break;
    case 556:
        piccoloS_presionada = false;
        break;
    case 558:
        pic->iniciarAnimacionIdle();
        break;
    case 600:
        if(pic->getVida() > (pic->getVidaMaxima())/2){ cntPiccolo = 159; }
        break;
    }
    if(pic->isVisible() || !pic->estaEnAnimacionEntrada()){ cntPiccolo++; }

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
