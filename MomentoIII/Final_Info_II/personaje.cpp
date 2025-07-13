#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QPen>
#include <QBrush>

#include "personaje.h"

Personaje::Personaje(QObject *parent)
    : QObject{parent}, QGraphicsPixmapItem()
{
    // Inicializar valores por defecto
    vida = 100;
    vidaMaxima = 100;
    velocidadMovimiento = 8;
    escalaSprite = 1.0;
    saltando = false;
    alturaOriginal = 0;
    moviendose = false;
    frameActual = 1;
    frameMaximo = 6; // Por defecto 6 frames de animación idle

    // Inicializar variables de animación de salto
    frameSaltoActual = 1;
    alturaMaximaAlcanzada = 0.0;

    // Inicializar hitbox (por defecto del tamaño del sprite)
    hitboxAncho = 30;  // Ancho típico de un personaje
    hitboxAlto = 30;   // Alto típico de un personaje
    hitboxOffsetX = -10; // Sin offset por defecto
    hitboxOffsetY = 0; // Sin offset por defecto
    limitesEscena = QRectF(0, 0, 800, 600); // Escena por defecto

    // Inicializar visualización de hitbox
    hitboxVisible = false;
    hitboxVisual = nullptr;

    // Inicializar trayectoria de salto - limitar a 50 puntos para evitar consumo excesivo de memoria
    maxPuntosTrayectoriaSalto = 50;
    trayectoriaSalto.clear();

    // Inicializar variables de física del salto
    velocidadVertical = 0.0;
    aceleracionVertical = 0.0;
    coeficienteResistencia = 0.1; // Resistencia del aire por defecto
    masaPersonaje = 1.0;          // Masa por defecto
    deltaT = 0.016;               // ~60 FPS (16ms)

    // Inicializar variables de movimiento horizontal durante salto
    posicionXSalto = 0.0;
    velocidadHorizontal = 0.0;
    velocidadHorizontalSalto = 0.0; // Nueva variable inicializada
    offsetYSalto = 0.0;
    posicionBaseSalto = 0.0;

    setFlag(QGraphicsItem::ItemIsMovable);

    // Configurar timer para salto (mayor precisión para ecuaciones diferenciales)
    jumpTimer = new QTimer(this);
    jumpTimer->setInterval(16); // 60 FPS para integración suave
    connect(jumpTimer, SIGNAL(timeout()), this, SLOT(actualizarSalto()));

    // Configurar timer para animación idle
    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200); // Cambiar frame cada 200ms
    connect(animacionTimer, SIGNAL(timeout()), this, SLOT(actualizarAnimacion()));
}

Personaje::~Personaje()
{
    if (jumpTimer) {
        jumpTimer->stop();
        delete jumpTimer;
    }
    if (animacionTimer) {
        animacionTimer->stop();
        delete animacionTimer;
    }
    if (hitboxVisual) {
        delete hitboxVisual;
        hitboxVisual = nullptr;
    }
}

void Personaje::saltar()
{
    if (!saltando) {
        saltando = true;
        alturaOriginal = this->pos().y();
        alturaMaximaAlcanzada = alturaOriginal; // Inicializar altura máxima

        // Inicializar sistema de movimiento separado
        posicionXSalto = this->pos().x();  // X inicial del salto
        posicionBaseSalto = this->pos().y(); // Y base del salto
        offsetYSalto = 0.0;                // Sin offset inicial

        // Mantener la velocidad horizontal previamente establecida
        // (no resetear velocidadHorizontal aquí, puede venir de salto direccional)

        // Inicializar condiciones del salto con ecuaciones diferenciales
        velocidadVertical = -velIn; // Velocidad inicial hacia arriba (negativa en Qt)
        aceleracionVertical = 0.0;  // Se calculará en cada frame
        tiempo = 0;
        frameSaltoActual = 1; // Comenzar con salto1.png

        // Aplicar hitbox de salto
        aplicarHitboxSalto();

        // Cambiar inmediatamente al primer sprite de salto
        cambiarSprite("salto1");

        jumpTimer->start();
        qDebug() << nombre << "INICIA SALTO - posX:" << posicionXSalto << "posBase:" << posicionBaseSalto;
    }
}

void Personaje::cambiarSprite(const QString& direccion)
{
    // 1. Guardar el centro actual del sprite
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

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
        setPixmap(nuevoSprite);

        // 2. Recalcular la posición para mantener el centro
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}

void Personaje::cambiarSpriteCentrado(const QString& direccion)
{
    // Guardar la posición central actual
    QPointF centroActual = pos() + QPointF(pixmap().width() / 2.0, pixmap().height() / 2.0);

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

        // Calcular nueva posición para mantener el centro
        QPointF nuevaPos = centroActual - QPointF(nuevoSprite.width() / 2.0, nuevoSprite.height() / 2.0);
        QGraphicsPixmapItem::setPos(nuevaPos);

        qDebug() << "Sprite centrado cambiado a:" << direccion << "- Nueva pos:" << nuevaPos;
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}

void Personaje::iniciarAnimacionIdle()
{
    moviendose = false;
    frameActual = 1; // Reiniciar desde el primer frame
    if (estaVivo()) { // Solo iniciar si no está saltando y está vivo
        animacionTimer->start();
    }
}

void Personaje::establecerEscala(qreal escala)
{
    escalaSprite = escala;
    // Aplicar la escala al sprite actual si existe
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
    vida = 0;
    animacionTimer->stop();
    jumpTimer->stop();
    moviendose = false;
    saltando = false;
    velocidadHorizontal = 0.0; // Limpiar velocidad horizontal

    // Restaurar hitbox normal si estaba saltando
    restaurarHitboxNormal();

    emit personajeMuerto(this);
    emit vidaCambiada(vida, vidaMaxima);
}

void Personaje::establecerVida(int vidaMaxima)
{
    this->vidaMaxima = vidaMaxima;
    this->vida = vidaMaxima;
    emit vidaCambiada(vida, this->vidaMaxima);
}

void Personaje::establecerVelocidad(qreal velocidad)
{
    this->velocidadMovimiento = velocidad;
}

void Personaje::establecerNombre(const QString& nombre)
{
    this->nombre = nombre;
}

void Personaje::establecerCarpetaSprites(const QString& carpeta)
{
    this->carpetaSprites = carpeta;
}

void Personaje::establecerFisicaSalto(qreal masa, qreal resistencia)
{
    masaPersonaje = masa;
    coeficienteResistencia = resistencia;
    qDebug() << nombre << "física configurada - Masa:" << masa << "Resistencia:" << resistencia;
}

void Personaje::establecerVelocidadSalto(qreal velocidadInicial)
{
    velIn = velocidadInicial;
    qDebug() << nombre << "velocidad de salto establecida en:" << velocidadInicial;
}

void Personaje::establecerVelocidadHorizontalSalto(qreal velocidadH)
{
    velocidadHorizontal = velocidadH;
    qDebug() << nombre << "velocidad horizontal del salto establecida en:" << velocidadH;
}

void Personaje::aplicarMovimientoHorizontal(qreal deltaX)
{
    qDebug() << "aplicarMovimientoHorizontal - deltaX:" << deltaX << "saltando:" << saltando;
    if (saltando) {
        qreal posicionAnterior = posicionXSalto;
        posicionXSalto += deltaX;

        // Aplicar límites de pantalla para movimiento horizontal
        if (posicionXSalto < limitesEscena.left()) {
            posicionXSalto = limitesEscena.left();
        } else if (posicionXSalto > limitesEscena.right() - hitboxAncho) {
            posicionXSalto = limitesEscena.right() - hitboxAncho;
        }

        qDebug() << nombre << "movimiento horizontal aplicado - de" << posicionAnterior << "a" << posicionXSalto;
    } else {
        qDebug() << "No se aplica movimiento horizontal - no está saltando";
    }
}

void Personaje::setPos(qreal x, qreal y)
{
    static int setPosCalls = 0;
    setPosCalls++;
    if (saltando && setPosCalls % 10 == 0) { // Debug cada 10 llamadas durante salto
        qDebug() << "*** setPos llamado #" << setPosCalls << "- X:" << x << "Y:" << y << "desde:" << ((QObject*)this)->metaObject()->className();
    }
    QGraphicsPixmapItem::setPos(x, y);
}

void Personaje::pausarTimerSalto()
{
    if (jumpTimer && jumpTimer->isActive()) {
        jumpTimer->stop();
        qDebug() << "Timer de salto PAUSADO";
    }
}

void Personaje::reanudarTimerSalto()
{
    if (jumpTimer && saltando) {
        jumpTimer->start();
        qDebug() << "Timer de salto REANUDADO";
    }
}

// ==================== MÉTODOS DE SALTO ====================

void Personaje::actualizarSalto()
{
    if (saltando) {
        tiempo += deltaT;

        // Constantes físicas - gravedad muy reducida para salto épico de Goku
        const qreal gravedad = 9.8 * 15; // Gravedad aún más baja para más tiempo en el aire

        // ECUACIÓN DIFERENCIAL VERTICAL: d²y/dt² = g + k*v*|v|/m
        // donde k es coeficiente resistencia, v es velocidad, m es masa

        // Calcular aceleración vertical actual (incluyendo gravedad y resistencia del aire)
        qreal fuerzaGravedad = gravedad;
        qreal fuerzaResistenciaVertical = coeficienteResistencia * velocidadVertical * qAbs(velocidadVertical) / masaPersonaje;

        // La resistencia siempre opone al movimiento vertical
        if (velocidadVertical < 0) { // Subiendo
            fuerzaResistenciaVertical = -fuerzaResistenciaVertical; // Resistencia hacia abajo
        }

        aceleracionVertical = fuerzaGravedad + fuerzaResistenciaVertical;

        // INTEGRACIÓN NUMÉRICA VERTICAL (Método de Euler mejorado)
        // v(t+dt) = v(t) + a(t)*dt
        velocidadVertical += aceleracionVertical * deltaT;

        // y(t+dt) = y(t) + v(t)*dt + 0.5*a(t)*dt²
        qreal deltaY = velocidadVertical * deltaT + 0.5 * aceleracionVertical * deltaT * deltaT;

        // Actualizar el offset Y (movimiento vertical)
        offsetYSalto += deltaY;

        // MOVIMIENTO HORIZONTAL PARABÓLICO
        // Aplicar resistencia del aire también al movimiento horizontal
        qreal fuerzaResistenciaHorizontal = coeficienteResistencia * velocidadHorizontal * qAbs(velocidadHorizontal) / masaPersonaje;

        // La resistencia horizontal siempre opone al movimiento
        if (velocidadHorizontal > 0) { // Moviendo a la derecha
            fuerzaResistenciaHorizontal = -fuerzaResistenciaHorizontal;
        } else if (velocidadHorizontal < 0) { // Moviendo a la izquierda
            fuerzaResistenciaHorizontal = -fuerzaResistenciaHorizontal;
        }

        // Actualizar velocidad horizontal con resistencia del aire
        velocidadHorizontal += fuerzaResistenciaHorizontal * deltaT;

        // Calcular desplazamiento horizontal
        qreal deltaX = velocidadHorizontal * deltaT;

        // Actualizar posición X del salto
        posicionXSalto += deltaX;

        // Aplicar límites de pantalla
        if (posicionXSalto < limitesEscena.left()) {
            posicionXSalto = limitesEscena.left();
            velocidadHorizontal = 0; // Detener movimiento horizontal al chocar
        } else if (posicionXSalto > limitesEscena.right() - hitboxAncho) {
            posicionXSalto = limitesEscena.right() - hitboxAncho;
            velocidadHorizontal = 0; // Detener movimiento horizontal al chocar
        }

        // Calcular posición final
        qreal posicionFinalX = posicionXSalto;
        qreal posicionFinalY = posicionBaseSalto + offsetYSalto;

        // Actualizar posición completa
        QGraphicsPixmapItem::setPos(posicionFinalX, posicionFinalY);

        // Agregar punto a la trayectoria para visualización
        trayectoriaSalto.append(QPointF(posicionFinalX, posicionFinalY));

        // Limitar el tamaño de la trayectoria
        if (trayectoriaSalto.size() > maxPuntosTrayectoriaSalto) {
            trayectoriaSalto.removeFirst();
        }

        // Debug menos frecuente
        static int debugCounter = 0;
        if (debugCounter % 60 == 0) { // Debug cada 60 frames (~1 segundo)
            qDebug() << "actualizarSalto - X:" << posicionFinalX << "Y:" << posicionFinalY << "velH:" << velocidadHorizontal << "velV:" << velocidadVertical;
        }
        debugCounter++;

        // Actualizar visualización de hitbox durante el salto
        actualizarVisualizacionHitbox();

        // Actualizar altura máxima alcanzada
        if (posicionFinalY < alturaMaximaAlcanzada) {
            alturaMaximaAlcanzada = posicionFinalY;
        }

        // LÓGICA DE ANIMACIÓN DEL SALTO (7 sprites)
        // Sistema basado en tiempo con sprite 6 un poco más corto
        int nuevoFrameSalto = frameSaltoActual;

        // Duración ajustada: sprite 6 más corto, sprite 7 más largo

        if (tiempo <= 0.077) {
            // Frame 1: Primer séptimo del salto (impulso inicial)
            nuevoFrameSalto = 1;
        } else if (tiempo <= 0.154) {
            // Frame 2: Segundo séptimo (subiendo fuerte)
            nuevoFrameSalto = 2;
        } else if (tiempo <= 0.231) {
            // Frame 3: Tercer séptimo (subiendo moderado)
            nuevoFrameSalto = 3;
        } else if (tiempo <= 0.308) {
            // Frame 4: Cuarto séptimo (punto más alto)
            nuevoFrameSalto = 4;
        } else if (tiempo <= 0.385) {
            // Frame 5: Quinto séptimo (comenzando a bajar)
            nuevoFrameSalto = 5;
        } else if (tiempo <= 0.440) {
            // Frame 6: Sexto séptimo - MÁS CORTO (0.055 segundos en lugar de 0.077)
            nuevoFrameSalto = 6;
        } else {
            // Frame 7: Último séptimo - MÁS LARGO (aterrizaje extendido)
            nuevoFrameSalto = 7;
        }

        // Debug reducido - solo cuando cambia el sprite
        // qDebug() << "velV:" << velocidadVertical << "velIn:" << velIn << "ratio:" << (velocidadVertical/velIn) << "distSuelo:" << distanciaAlSuelo << "frame:" << nuevoFrameSalto;

        // Cambiar sprite solo si es necesario
        if (nuevoFrameSalto != frameSaltoActual) {
            frameSaltoActual = nuevoFrameSalto;
            cambiarSprite("salto" + QString::number(frameSaltoActual));
            qDebug() << nombre << "cambia a sprite salto" << frameSaltoActual << "- tiempo:" << tiempo << "segundos";
        }

        // Verificar si ha tocado el suelo
        if (posicionFinalY >= alturaOriginal && velocidadVertical >= 0) {
            // Aterrizaje con amortiguación - usar posición X final del salto
            setPos(posicionXSalto, alturaOriginal);

            qDebug() << nombre << "aterriza después de" << tiempo << "segundos en posición X:" << posicionXSalto;

            saltando = false;
            velocidadVertical = 0.0;
            aceleracionVertical = 0.0;
            velocidadHorizontal = 0.0; // Limpiar velocidad horizontal
            jumpTimer->stop();
            tiempo = 0;
            frameSaltoActual = 1;
            alturaMaximaAlcanzada = 0.0;

            // Restaurar hitbox normal al aterrizar
            restaurarHitboxNormal();

            // Limpiar trayectoria del salto al aterrizar
            trayectoriaSalto.clear();

            // Cambiar inmediatamente al sprite base1 al aterrizar
            cambiarSprite("base1");

            // Emitir señal de aterrizaje para verificar movimiento continuo
            emit personajeAterrizo();

            // Reiniciar animación idle si no se está moviendo
            if (!moviendose) {
                iniciarAnimacionIdle();
            }
        }

        // Debug habilitado para ver la física
        qDebug() << "t:" << tiempo << "y:" << posicionFinalY << "yOriginal:" << alturaOriginal << "v:" << velocidadVertical << "frame:" << frameSaltoActual;
    }
}

void Personaje::actualizarAnimacion()
{
    if (!moviendose && !saltando && estaVivo()) {
        // Ciclar entre los frames de la animación idle
        frameActual++;
        if (frameActual > frameMaximo) {
            frameActual = 1;
        }

        QString rutaSprite = ":/Goku/Sprites/" + carpetaSprites + "/base" + QString::number(frameActual) + ".png";
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
            setPixmap(nuevoSprite);
        } else {
            qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
        }
    }
}

void Personaje::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Dibujar el sprite normalmente
    QGraphicsPixmapItem::paint(painter, option, widget);

    // Visualizar la trayectoria del salto si está habilitada la visualización de hitbox
    if (hitboxVisible && !trayectoriaSalto.isEmpty() && trayectoriaSalto.size() > 1) {
        // Configurar el pincel para la línea de trayectoria del salto
        QPen penTrayectoria;
        penTrayectoria.setColor(Qt::blue);
        penTrayectoria.setWidth(2);
        penTrayectoria.setStyle(Qt::DashLine);
        painter->setPen(penTrayectoria);

        // Dibujar líneas conectando todos los puntos de la trayectoria
        for (int i = 0; i < trayectoriaSalto.size() - 1; ++i) {
            QPointF puntoInicio = mapFromScene(trayectoriaSalto[i]);
            QPointF puntoFin = mapFromScene(trayectoriaSalto[i + 1]);
            painter->drawLine(puntoInicio, puntoFin);
        }

        // Conectar el último punto de la trayectoria con la posición actual
        if (!trayectoriaSalto.isEmpty()) {
            QPointF ultimoPunto = mapFromScene(trayectoriaSalto.last());
            QPointF posicionActual = QPointF(0, 0); // Posición relativa al item
            painter->drawLine(ultimoPunto, posicionActual);
        }
    }
}

// ==================== MÉTODOS DE HITBOX ====================

void Personaje::establecerHitbox(qreal ancho, qreal alto, qreal offsetX, qreal offsetY)
{
    hitboxAncho = ancho;
    hitboxAlto = alto;
    hitboxOffsetX = offsetX;
    hitboxOffsetY = offsetY;

    // Guardar como hitbox normal (respaldo)
    hitboxNormalAncho = ancho;
    hitboxNormalAlto = alto;
    hitboxNormalOffsetX = offsetX;
    hitboxNormalOffsetY = offsetY;

    qDebug() << nombre << "hitbox establecida:" << ancho << "x" << alto << "offset(" << offsetX << "," << offsetY << ")";
}

void Personaje::establecerHitboxSalto(qreal ancho, qreal alto, qreal offsetX, qreal offsetY)
{
    hitboxSaltoAncho = ancho;
    hitboxSaltoAlto = alto;
    hitboxSaltoOffsetX = offsetX;
    hitboxSaltoOffsetY = offsetY;
    qDebug() << nombre << "hitbox de salto configurada:" << ancho << "x" << alto << "offset(" << offsetX << "," << offsetY << ")";
}

void Personaje::aplicarHitboxSalto()
{
    if (saltando) {
        hitboxAncho = hitboxSaltoAncho;
        hitboxAlto = hitboxSaltoAlto;
        hitboxOffsetX = hitboxSaltoOffsetX;
        hitboxOffsetY = hitboxSaltoOffsetY;
        actualizarVisualizacionHitbox();
        qDebug() << nombre << "aplicando hitbox de salto";
    }
}

void Personaje::restaurarHitboxNormal()
{
    hitboxAncho = hitboxNormalAncho;
    hitboxAlto = hitboxNormalAlto;
    hitboxOffsetX = hitboxNormalOffsetX;
    hitboxOffsetY = hitboxNormalOffsetY;
    actualizarVisualizacionHitbox();
    qDebug() << nombre << "restaurando hitbox normal";
}

QRectF Personaje::obtenerHitbox() const
{
    // Hitbox relativa al personaje (posición local)
    return QRectF(hitboxOffsetX, hitboxOffsetY, hitboxAncho, hitboxAlto);
}

QRectF Personaje::obtenerHitboxGlobal() const
{
    // Hitbox en coordenadas globales de la escena
    QPointF posicion = this->pos();
    return QRectF(posicion.x() + hitboxOffsetX,
                  posicion.y() + hitboxOffsetY,
                  hitboxAncho,
                  hitboxAlto);
}

bool Personaje::colisionaCon(Personaje* otroPersonaje) const
{
    if (!otroPersonaje || otroPersonaje == this) {
        return false;
    }

    QRectF miHitbox = obtenerHitboxGlobal();
    QRectF otraHitbox = otroPersonaje->obtenerHitboxGlobal();

    bool colision = miHitbox.intersects(otraHitbox);

    if (colision) {
        qDebug() << nombre << "colisiona con" << otroPersonaje->getNombre();
    }

    return colision;
}

bool Personaje::colisionaCon(const QRectF& rectangulo) const
{
    QRectF miHitbox = obtenerHitboxGlobal();
    return miHitbox.intersects(rectangulo);
}

void Personaje::verificarLimitesPantalla(const QRectF& limitesEscena)
{
    QPointF posicionActual = this->pos();
    QRectF hitboxGlobal = obtenerHitboxGlobal();

    qreal nuevaX = posicionActual.x();
    qreal nuevaY = posicionActual.y();

    // Verificar límite izquierdo
    if (hitboxGlobal.left() < limitesEscena.left()) {
        nuevaX = limitesEscena.left() - hitboxOffsetX;
    }

    // Verificar límite derecho
    if (hitboxGlobal.right() > limitesEscena.right()) {
        nuevaX = limitesEscena.right() - hitboxAncho - hitboxOffsetX;
    }

    // Verificar límite superior
    if (hitboxGlobal.top() < limitesEscena.top()) {
        nuevaY = limitesEscena.top() - hitboxOffsetY;
    }

    // Verificar límite inferior (solo si no está saltando)
    if (!saltando && hitboxGlobal.bottom() > limitesEscena.bottom()) {
        nuevaY = limitesEscena.bottom() - hitboxAlto - hitboxOffsetY;
    }

    // Aplicar nueva posición si cambió
    if (nuevaX != posicionActual.x() || nuevaY != posicionActual.y()) {
        setPos(nuevaX, nuevaY);
    }
}

void Personaje::establecerLimitesEscena(const QRectF& limites)
{
    limitesEscena = limites;
    qDebug() << nombre << "límites de escena establecidos:" << limites;
}

// ==================== MÉTODOS DE VISUALIZACIÓN DE HITBOX ====================

void Personaje::mostrarHitbox(bool mostrar)
{
    hitboxVisible = mostrar;

    if (mostrar) {
        // Crear o actualizar la visualización de hitbox
        if (!hitboxVisual) {
            hitboxVisual = new QGraphicsRectItem();

            // Configurar apariencia del hitbox - más sutil
            QPen pen(Qt::red, 1); // Línea roja de 1 píxel (más fina)
            pen.setStyle(Qt::DashLine); // Línea punteada
            hitboxVisual->setPen(pen);

            // Sin relleno para que sea solo el borde
            hitboxVisual->setBrush(Qt::NoBrush);

            // Agregar a la escena si el personaje ya está en una escena
            if (this->scene()) {
                this->scene()->addItem(hitboxVisual);
            }
        }

        // Actualizar posición y tamaño del hitbox visual
        actualizarVisualizacionHitbox();
        hitboxVisual->setVisible(true);

        qDebug() << nombre << "hitbox visible activada";
    } else {
        // Ocultar hitbox
        if (hitboxVisual) {
            hitboxVisual->setVisible(false);
        }
        qDebug() << nombre << "hitbox visible desactivada";
    }
}

void Personaje::ocultarHitbox()
{
    mostrarHitbox(false);
}

void Personaje::actualizarVisualizacionHitbox()
{
    if (hitboxVisual && hitboxVisible) {
        // Actualizar posición y tamaño del rectángulo visual
        QRectF hitboxGlobal = obtenerHitboxGlobal();
        hitboxVisual->setRect(hitboxGlobal);

        // Asegurar que esté en el nivel correcto (por encima del personaje)
        hitboxVisual->setZValue(this->zValue() + 1);
    }
}

void Personaje::cambiarSpriteConOffset(const QString& direccion, qreal offsetX, qreal offsetY)
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

        // Cambiar el sprite SIN mover la posición
        setPixmap(nuevoSprite);

        qDebug() << "Sprite con offset cambiado a:" << direccion << "- Offset especificado:" << offsetX << "," << offsetY << "(posición sin cambiar)";
    } else {
        qDebug() << "No se pudo cargar el sprite:" << rutaSprite;
    }
}

void Personaje::moverDerecha(){}
void Personaje::moverIzquierda(){}
void Personaje::moverArriba(){}
void Personaje::moverAbajo(){}
void Personaje::atacar(){}


void Personaje::recibirDanio(int danio){
    vida -= danio;
    if (vida < 0) vida = 0;
    emit vidaCambiada(vida, vidaMaxima);
}
