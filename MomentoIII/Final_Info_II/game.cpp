#include "game.h"
#include "ui_game.h"
#include "piccolo.h"
#include "goku.h"

game::game(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::game)
{
    ui->setupUi(this);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);

    view->setScene(scene);
    view->resize(1000, 500);
    view->scale(2.0, 2.0);
    scene->setSceneRect(200, 200, 1000, 500);

    p = new Goku();
    scene->addItem(p);
    p->setPos(300, 300);

    pic = new piccolo();
    scene->addItem(pic);
    pic->setPos(400, 120);
    
    // Conectar señal de aterrizaje para movimiento continuo
    connect(p, &Personaje::personajeAterrizo, this, &game::verificarMovimientoContinuo);
    
    // Configurar timer para movimiento continuo
    movimientoTimer = new QTimer(this);
    movimientoTimer->setInterval(50); // 20 FPS para movimiento suave
    connect(movimientoTimer, &QTimer::timeout, this, &game::actualizarMovimiento);
    // No iniciar automáticamente, solo cuando sea necesario
    
    // Configurar límites de escena para las colisiones
    QRectF limitesJuego(200, 200, 1000, 500); // Mismos límites que la escena
    p->establecerLimitesEscena(limitesJuego);
    
    // Opcional: Escalar Goku (1.5 = 150% del tamaño original)
    // p->establecerEscala(1.5);

    // Agregar grilla temporal para debug de posicionamiento
    agregarGrillaDebug();
}

game::~game()
{
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
        // Alternar visualización de hitbox con tecla H
        if (p->estaHitboxVisible()) {
            p->ocultarHitbox();
            qDebug() << "Hitbox oculta";
        } else {
            p->mostrarHitbox();
            qDebug() << "Hitbox visible";
        }
    }
    if(e->key() == Qt::Key_G){ 
        // Alternar visualización de grilla con tecla G
        alternarGrillaDebug();
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
    // Debug para ver si el timer está funcionando
    qDebug() << "actualizarMovimiento() llamado - D:" << teclaD_presionada << "A:" << teclaA_presionada << "saltando:" << p->estaSaltando();
    
    // Solo aplicar movimiento si no está saltando
    if (!p->estaSaltando()) {
        if (teclaD_presionada) {
            qDebug() << "Ejecutando moverDerecha()";
            p->moverDerecha();
        }
        if (teclaA_presionada) {
            qDebug() << "Ejecutando moverIzquierda()";
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
