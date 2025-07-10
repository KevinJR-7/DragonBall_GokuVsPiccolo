#include <QTimer>

#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include "goku.h"

Goku::Goku(QObject *parent)
    : Personaje(parent)
{
    // Inicializar dirección horizontal
    ultimaDireccionHorizontal = "";

    // Inicializar animación de entrada
    animacionEntradaActiva = false;
    frameEntradaActual = 1;
    timerEntrada = new QTimer(this);
    timerEntrada->setInterval(180); // 180ms por frame
    connect(timerEntrada, &QTimer::timeout, this, &Goku::actualizarAnimacionEntrada);
    
    // Inicializar animación de recarga de ki
    animacionKiActiva = false;
    frameKiActual = 1;
    timerKi = new QTimer(this);
    timerKi->setInterval(150); // 150ms por frame de ki
    connect(timerKi, &QTimer::timeout, this, &Goku::actualizarAnimacionKi);
    
    // Inicializar sistema de ki
    estadoKiActual = INICIO;
    frameDentroEstado = 1;
    kiActual = 0;
    kiMaximo = 100;
    velocidadRecargaKi = 10; // 10 puntos de ki por segundo
    
    // Configurar offsets para sprites de ki (ajustar según sea necesario)
    offsetKiX = -18.0; // 10 píxeles más a la izquierda (era -8.0)
    offsetKiY = -16.0; // 10 píxeles más arriba (era -6.0)
    
    // Timer para recarga real de ki (separado de la animación)
    timerRecargaKi = new QTimer(this);
    timerRecargaKi->setInterval(100); // 100ms = 0.1 segundos
    connect(timerRecargaKi, &QTimer::timeout, this, &Goku::recargarKi);
    
    // Inicializar animación de Kamehameha
    animacionKamehamehaActiva = false;
    frameKamehamehaActual = 1;
    timerKamehameha = new QTimer(this);
    timerKamehameha->setInterval(120); // 120ms por frame de Kamehameha
    connect(timerKamehameha, &QTimer::timeout, this, &Goku::actualizarAnimacionKamehameha);
    
    // Posición fija para la animación de Kamehameha
    posicionFijaKamehameha = QPointF(0, 0);
    
    // Inicializar animación de ráfaga
    animacionRafagaActiva = false;
    frameRafagaActual = 1;
    timerRafaga = new QTimer(this);
    timerRafaga->setInterval(100); // 100ms por frame de ráfaga
    connect(timerRafaga, &QTimer::timeout, this, &Goku::actualizarAnimacionRafaga);
    
    // Configurar propiedades específicas de Goku
    establecerNombre("Goku");
    establecerCarpetaSprites("goku");
    establecerVida(4); // Goku tiene más vida
    establecerVelocidad(10); // Goku es rápido
    
    // Configurar física del salto específica para Goku
    establecerVelocidadSalto(65.0);      // Goku salta SÚPER alto (aumentado de 50 a 65)
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia
    
    // Configurar hitbox específica para Goku - escalada 3.5x
    // Hitbox original: 20x40, offset 12x25
    // Hitbox escalada: 70x140, offset 42x88
    establecerHitbox(70, 140, 42, 88);
    
    // Configurar hitbox de salto - escalada 3.5x  
    // Hitbox salto original: 22x20, offset 9x20
    // Hitbox salto escalada: 77x70, offset 32x70
    establecerHitboxSalto(77, 70, 32, 70);
    
    // Hacer a Goku invisible al inicio - solo aparecerá en la animación de entrada
    setVisible(false);
    qDebug() << "Goku inicializado como invisible - aparecerá en 2 segundos";
    
    // Programar la animación de entrada para después de 2 segundos
    QTimer::singleShot(2000, this, &Goku::iniciarAnimacionEntrada);
}

void Goku::moverDerecha()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
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
        cambiarSprite("adelante");
    }
    
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Goku::moverIzquierda()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
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
        cambiarSprite("atras");
    }
    
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Goku::moverArriba()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
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

void Goku::moverAbajo()
{
    // No permitir movimiento si es invisible, durante animación de entrada o recargando ki
    if (!isVisible() || animacionEntradaActiva || animacionKiActiva) {
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

void Goku::atacar()
{
    if (estaVivo()) {
        qDebug() << nombre << " está atacando con Kamehameha";

        cambiarSprite("atacando"); //
        
        emit personajeAtaco(this);
        
        // Volver a la animación idle después de un tiempo
        QTimer::singleShot(500, this, [this]() {
            if (!moviendose) {
                iniciarAnimacionIdle();
            }
        });
    }
}

void Goku::recibirDanio(int danio)
{
    if (estaVivo()) {
        vida -= danio;
        if (vida < 0) vida = 0;
        emit vidaCambiada(vida, vidaMaxima);

        // Cambia al sprite de daño
        cambiarSprite("herido");

        // Después de 200 ms, vuelve a la animación idle
        QTimer::singleShot(1000, this, [this]() {
            iniciarAnimacionIdle();
        });

        if (vida <= 0) {
            morir();
        }
    }
}
void Goku::iniciarAnimacionIdle()
{
    // Limpiar dirección horizontal al entrar en idle
    ultimaDireccionHorizontal = "";
    
    // Llamar al método de la clase padre
    Personaje::iniciarAnimacionIdle();
}

void Goku::iniciarAnimacionEntrada()
{
    qDebug() << "Iniciando animación de entrada de Goku después de 2 segundos";
    
    // Hacer visible a Goku al comenzar la animación de entrada
    setVisible(true);
    
    // Detener la animación idle si está activa
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }
    
    animacionEntradaActiva = true;
    frameEntradaActual = 1;
    
    // Cargar el primer sprite de entrada SIN cambiar posición
    QString rutaSprite = ":/Goku/Sprites/goku/entrada1.png";
    QPixmap spriteEntrada(rutaSprite);
    if (!spriteEntrada.isNull()) {
        setPixmap(spriteEntrada);
        qDebug() << "Sprite entrada1 cargado en posición:" << pos();
    } else {
        qDebug() << "Error: No se pudo cargar sprite entrada1 desde" << rutaSprite;
    }
    
    // Iniciar el timer de entrada
    timerEntrada->start();
}

void Goku::actualizarAnimacionEntrada()
{
    if (animacionEntradaActiva) {
        frameEntradaActual++;
        
        if (frameEntradaActual <= 3) {
            // Mostrar entrada1, entrada2, entrada3 SIN cambiar posición
            QString rutaSprite = ":/Goku/Sprites/goku/entrada" + QString::number(frameEntradaActual) + ".png";
            QPixmap spriteEntrada(rutaSprite);
            if (!spriteEntrada.isNull()) {
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

void Goku::iniciarRecargaKi()
{
    // Verificar si el ki ya está completo
    if (kiActual >= kiMaximo) {
        qDebug() << "Ki ya está completo (" << kiActual << "/" << kiMaximo << ") - no se puede recargar más";
        return;
    }
    
    qDebug() << "Goku inicia recarga de ki - Ki actual:" << kiActual << "/" << kiMaximo;
    
    // Guardar la posición exacta antes de cambiar cualquier sprite
    posicionOriginalKi = pos();
    qDebug() << "Posición original guardada:" << posicionOriginalKi;
    
    // Detener otras animaciones
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }
    
    animacionKiActiva = true;
    
    // Configurar estado inicial y velocidad según ki actual
    estadoKiActual = INICIO;
    frameDentroEstado = 1;
    
    // Ajustar velocidad de animación según ki actual
    int velocidadAnimacion = 150 - (int)(obtenerPorcentajeKi() * 1.2f); // Más rápido con más ki
    velocidadAnimacion = qMax(30, velocidadAnimacion); // Mínimo 30ms
    timerKi->setInterval(velocidadAnimacion);
    qDebug() << "Velocidad animación ki:" << velocidadAnimacion << "ms (ki al" << obtenerPorcentajeKi() << "%)";
    
    // Guardar la posición actual antes de cambiar sprites
    posicionOriginalKi = pos();
    qDebug() << "Guardando posición antes de ki:" << posicionOriginalKi;
    
    // Cargar el primer sprite de ki y centrarlo UNA SOLA VEZ
    // Guardar el centro actual del sprite "quieto"
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);
    
    // Cambiar al sprite ki1
    cambiarSprite("ki1");
    
    // Calcular nueva posición para que ki1 tenga el mismo centro que "quieto"
    QPointF nuevaPosicion = centroActual - QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);
    setPos(nuevaPosicion.x(), nuevaPosicion.y());

    qDebug() << "Sprite ki1 centrado en posición:" << nuevaPosicion;
    
    // Iniciar los timers
    timerKi->start();
    timerRecargaKi->start();
}

void Goku::detenerRecargaKi()
{
    if (animacionKiActiva) {
        qDebug() << "Goku detiene recarga de ki - Ki final:" << kiActual << "/" << kiMaximo;
        
        animacionKiActiva = false;
        timerKi->stop();
        timerRecargaKi->stop();
        
        // Volver exactamente a la posición original
        setPos(posicionOriginalKi.x(), posicionOriginalKi.y());

        
        // Configurar estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
        qDebug() << "Goku volvió a idle en posición original";
    }
}

void Goku::actualizarAnimacionKi()
{
    if (!animacionKiActiva) return;
    
    QString spriteActual;
    
    switch (estadoKiActual) {
        case INICIO:
            // Frames 1, 2 (una sola vez)
            if (frameDentroEstado == 1) {
                spriteActual = "ki1";
                qDebug() << "Estado INICIO - Frame 1";
            } else if (frameDentroEstado == 2) {
                spriteActual = "ki2";
                qDebug() << "Estado INICIO - Frame 2";
            }
            
            frameDentroEstado++;
            if (frameDentroEstado > 2) {
                // Cambiar a estado BUCLE
                qDebug() << "Cambiando de INICIO a BUCLE";
                estadoKiActual = BUCLE;
                frameDentroEstado = 1;
            }
            break;
            
        case BUCLE: {
            // Frames 3, 4, 5, 6 (repetir infinitamente)
            int frameEnBucle = ((frameDentroEstado - 1) % 4) + 3; // Ciclo: 3,4,5,6
            spriteActual = "ki" + QString::number(frameEnBucle);
            qDebug() << "Estado BUCLE - Frame" << frameEnBucle << "(iteración" << (frameDentroEstado-1)/4 + 1 << ")";
            
            frameDentroEstado++;
            // En estado BUCLE no cambiamos de estado automáticamente
            // El cambio se hace en recargarKi() cuando ki >= 95%
            break;
        }
            
        case FINAL:
            // Frames 7, 8 (una sola vez y terminar)
            if (frameDentroEstado == 1) {
                spriteActual = "ki7";
                qDebug() << "Estado FINAL - Frame 7";
                frameDentroEstado++;
            } else if (frameDentroEstado == 2) {
                spriteActual = "ki8";
                qDebug() << "Estado FINAL - Frame 8";
                frameDentroEstado++;
            } else {
                // Después del frame 8, terminar la animación
                qDebug() << "Animación de ki completada - terminando después del frame 8";
                detenerRecargaKi();
                return;
            }
            break;
    }

        if (!spriteActual.isEmpty()) {
            cambiarSprite(spriteActual);
            qDebug() << "Animación Ki - frame:" << spriteActual << "posición mantenida:" << pos();
        }
}

void Goku::establecerKi(int ki, int kiMax) {
    kiActual = ki;
    kiMaximo = kiMax;
    if (kiActual < 0) kiActual = 0;
    if (kiActual > kiMaximo) kiActual = kiMaximo;
    emit kiCambiado(kiActual, kiMaximo);
}

void Goku::recargarKi()
{
    if (animacionKiActiva && kiActual < kiMaximo) {
        // Incrementar ki (1 punto cada 100ms = 10 puntos por segundo)
        kiActual++;
        qDebug() << "Ki recargado:" << kiActual << "/" << kiMaximo << "(" << obtenerPorcentajeKi() << "%)";
        // EMITIR LA SEÑAL AQUÍ
        emit kiCambiado(kiActual, kiMaximo);
        // Verificar si pasamos al estado final (95% o más)
        if (obtenerPorcentajeKi() >= 95.0f && estadoKiActual == BUCLE) {
            qDebug() << "Cambiando a estado FINAL - ki al 95%";
            estadoKiActual = FINAL;
            frameDentroEstado = 1; // Reiniciar para frames 7, 8
        }
        
        // Si el ki está lleno, detener la recarga
        if (kiActual >= kiMaximo) {
            qDebug() << "Ki completamente lleno - deteniendo recarga";
            detenerRecargaKi();
        }
    }
}

void Goku::iniciarCargaKamehameha()
{
    // Verificar que no esté ya cargando Kamehameha o recargando ki
    if (animacionKamehamehaActiva || animacionKiActiva) {
        qDebug() << "No se puede cargar Kamehameha - ya hay otra animación activa";
        return;
    }
    
    qDebug() << "Goku inicia carga de Kamehameha";
    
    // Guardar la posición actual como posición fija para toda la animación (usar posición idle)
    posicionFijaKamehameha = pos();
    qDebug() << "Posición fija guardada (idle):" << posicionFijaKamehameha;
    
    // Detener otras animaciones
    if (animacionTimer && animacionTimer->isActive()) {
        animacionTimer->stop();
    }

    animacionKamehamehaActiva = true;
    frameKamehamehaActual = 1; // Volver a empezar desde kame1

    // Cargar el primer sprite de Kamehameha usando la función de posición fija
    cambiarSpriteKamehamehaFijo("kame1");
    qDebug() << "Sprite kame1 cargado en posición fija:" << posicionFijaKamehameha;

    // Iniciar el timer de Kamehameha
    timerKamehameha->start();
}

void Goku::detenerCargaKamehameha()
{
    if (animacionKamehamehaActiva) {
        qDebug() << "Goku detiene carga de Kamehameha prematuramente - frame actual:" << frameKamehamehaActual;
        
        // Solo detener la carga sin lanzar el proyectil
        // El proyectil se lanza automáticamente al completar la animación
        
        animacionKamehamehaActiva = false;
        timerKamehameha->stop();
        
        // Restaurar posición y sprite idle
        setPos(posicionFijaKamehameha.x(), posicionFijaKamehameha.y());
        cambiarSprite("quieto");

        // Configurar estado idle
        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
        qDebug() << "Carga de Kamehameha cancelada - volvió a idle y posición restaurada:" << pos();
    }
}

void Goku::actualizarAnimacionKamehameha()
{
    if (!animacionKamehamehaActiva) return;

    if (frameKamehamehaActual <= 12) {
        QString spriteKamehameha = "kame" + QString::number(frameKamehamehaActual);
        cambiarSpriteKamehamehaFijo(spriteKamehameha);

        qDebug() << "Animación Kamehameha - frame:" << frameKamehamehaActual << "sprite:" << spriteKamehameha << "posición fija:" << posicionFijaKamehameha;

        // Lanzar el proyectil en el frame 7 (puedes cambiar el número si quieres otro frame)
        if (frameKamehamehaActual == 7) {
            qDebug() << "Lanzando Kamehameha en el frame 7";
            lanzarKamehameha();
        }

        frameKamehamehaActual++;
    } else {
        // Terminar la animación
        animacionKamehamehaActiva = false;
        timerKamehameha->stop();

        setPos(posicionFijaKamehameha.x(), posicionFijaKamehameha.y());
        cambiarSprite("quieto");

        moviendose = false;
        frameActual = 1;
        if (animacionTimer->isActive()) {
            animacionTimer->stop();
        }
        qDebug() << "Animación Kamehameha terminada y posición restaurada a:" << pos();
    }
}
void Goku::lanzarKamehameha()
{
    int costoKi = 30; // Define el costo de ki para Kamehameha
    if (kiActual >= costoKi) {
        kiActual -= costoKi;
        emit kiCambiado(kiActual, kiMaximo); // Actualiza la barra de ki

        qDebug() << "¡Lanzando Kamehameha!";

        // Crear el proyectil Kamehameha
        Kamehameha* kamehameha = new Kamehameha(this);

        // Obtener la posición actual de Goku
        QPointF posicionGoku = pos();

        // Calcular la posición de lanzamiento ajustada para Goku escalado 3.5x
        float posX = posicionGoku.x() + 210;
        float posY = posicionGoku.y() + 140;

        // Determinar dirección (hacia la derecha por defecto)
        float direccionX = 1.0f;
        float direccionY = 0.0f;

        // Configurar el proyectil
        float velocidad = 8.0f;
        float alcance = 600.0f;

        // Crear el proyectil
        kamehameha->crear(posX, posY, direccionX, direccionY, velocidad, alcance);

        // Agregar el proyectil a la escena
        if (scene()) {
            scene()->addItem(kamehameha);
            qDebug() << "Kamehameha agregado a la escena en posición:" << posX << "," << posY;
        } else {
            qDebug() << "Error: No se pudo agregar Kamehameha a la escena (scene es null)";
        }
    } else {
        qDebug() << "No hay suficiente ki para lanzar Kamehameha";
    }
}
// ==================== MÉTODOS DE ANIMACIÓN DE RÁFAGA ====================

void Goku::iniciarAnimacionRafaga()
{
    qDebug() << "Iniciando animación de ráfaga";
    
    // No permitir si ya está en otra animación
    if (animacionEntradaActiva || animacionKiActiva || animacionKamehamehaActiva) {
        qDebug() << "No se puede iniciar ráfaga: otra animación activa";
        return;
    }
    
    // Detener animación idle si está activa
    if (animacionTimer->isActive()) {
        animacionTimer->stop();
    }
    
    // Configurar animación de ráfaga
    animacionRafagaActiva = true;
    frameRafagaActual = 1;
    moviendose = true; // Marcar como en movimiento para evitar idle
    
    // Iniciar con el primer frame
    cambiarSprite("bolas1");
    
    // Iniciar timer
    timerRafaga->start();
    
    qDebug() << "Animación de ráfaga iniciada";
}

void Goku::detenerAnimacionRafaga()
{
    if (!animacionRafagaActiva) {
        return;
    }
    
    qDebug() << "Deteniendo animación de ráfaga";
    
    // Detener timer
    timerRafaga->stop();
    
    // Terminar la animación
    animacionRafagaActiva = false;
     
    // Configurar estado idle
    moviendose = false;
    frameActual = 1;
    
    qDebug() << "Animación de ráfaga detenida";
}

void Goku::actualizarAnimacionRafaga()
{
    if (!animacionRafagaActiva) {
        return;
    }
    
    // Ciclar entre bolas1 a bolas5
    frameRafagaActual++;
    if (frameRafagaActual > 5) {
        frameRafagaActual = 1;
    }
    
    // Cambiar sprite
    QString spriteNombre = QString("bolas%1").arg(frameRafagaActual);
    cambiarSprite(spriteNombre);
    
    // Lanzar proyectiles BlastB en frames específicos
    if (frameRafagaActual == 2 || frameRafagaActual == 3 || frameRafagaActual == 4) {
        lanzarBlastB();
        qDebug() << "Lanzando BlastB en frame:" << frameRafagaActual;
    }
    
    qDebug() << "Animación ráfaga frame:" << frameRafagaActual;
}

void Goku::lanzarBlastB()
{
    int costoKi = 10; // Define el costo de ki para BlastB
    if (kiActual >= costoKi) {
        kiActual -= costoKi;
        emit kiCambiado(kiActual, kiMaximo); // Actualiza la barra de ki

        qDebug() << "¡Lanzando BlastB con trayectoria caótica!";

        // Crear el proyectil BlastB
        BlastB* blastB = new BlastB(this);

        // Obtener la posición actual de Goku
        QPointF posicionGoku = pos();

        // Calcular la posición de lanzamiento ajustada para Goku escalado 3.5x
        float posX = posicionGoku.x() + 140;
        float posY = posicionGoku.y() + 88;

        // Determinar dirección inicial (hacia la derecha por defecto)
        float direccionX = 1.0f;
        float direccionY = 0.0f;

        // Configurar el proyectil con trayectoria caótica
        float velocidad = 6.0f;
        float alcance = 1000.0f;

        // Crear el proyectil
        blastB->crear(posX, posY, direccionX, direccionY, velocidad, alcance);

        // Configurar parámetros caóticos
        blastB->configurarCaos(10.0, 28.0, 8.0/3.0);

        // Agregar el proyectil a la escena
        if (scene()) {
            scene()->addItem(blastB);
            blastB->iniciar(QPointF(posX, posY), QPointF(direccionX, direccionY));
            qDebug() << "BlastB agregado a la escena en posición:" << posX << "," << posY;
        } else {
            qDebug() << "Error: No se pudo agregar BlastB a la escena (scene es null)";
        }
    } else {
        qDebug() << "No hay suficiente ki para lanzar BlastB";
    }
}



void Goku::cambiarSpriteKamehamehaFijo(const QString& direccion)
{
    QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/" + direccion + ".png";
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
        setPixmap(nuevoSprite);

        // Centrar el sprite respecto a su centro (para todos los frames)
        // Esto asegura que el centro del sprite siempre quede en la misma posición

        QGraphicsPixmapItem::setPos(posicionFijaKamehameha);

        qDebug() << "Sprite Kamehameha cambiado a:" << direccion << "- Posición centrada en:" << (posicionFijaKamehameha );
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}


void Goku::morir()
{
    // Detén otras animaciones y movimiento
    if (animacionTimer && animacionTimer->isActive())
        animacionTimer->stop();
    moviendose = false;

    // Inicia la animación de muerte
    frameMuerteActual = 1;
    if (!timerMuerte) {
        timerMuerte = new QTimer(this);
        connect(timerMuerte, &QTimer::timeout, this, [this]() {
            if (frameMuerteActual <= 4) {
                cambiarSprite(QString("muerto%1").arg(frameMuerteActual));
                frameMuerteActual++;
            } else {
                timerMuerte->stop();
                // Aquí puedes dejar el último sprite o hacer más lógica (ej: eliminar personaje)
            }
        });
    }
    timerMuerte->start(200); // Cambia de frame cada 200 ms

    // Opcional: emite señal de muerte
    emit personajeMuerto(this);
}



