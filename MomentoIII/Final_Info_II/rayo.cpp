#include "rayo.h"
#include "goku.h" //Para que verifique colision
#include <QGraphicsScene>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>

// Definir la variable estática
bool Rayo::mostrarHitbox = false;

Rayo::Rayo(QObject *parent)
    : Habilidad(parent)
    , x(0), y(0)
    , dirX(-1), dirY(0)
    , velocidad(15.0)  // Velocidad por defecto más rápida
    , alcance(800.0)
    , distanciaRecorrida(0)
    , activo(false)
    , spritesValidos(false)
    , anchoTotal(150)
    , altoTotal(50)
    , usarHameha(false)
    , contadorAnimacion(0)
    , daño(1)
    , hitboxActivo(false)
{
    transform.scale(-1, 1); // Refleja sobre el eje X

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Rayo::actualizar_timeout);

    // Timer para la animación intercalada
    animacionTimer = new QTimer(this);
    animacionTimer->setInterval(200); // Cambiar cada 200ms (más lento)
    connect(animacionTimer, &QTimer::timeout, this, &Rayo::alternarSprites);

    // Inicializar trayectoria - limitar a 100 puntos para evitar consumo excesivo de memoria
    maxPuntosTrayectoria = 100;
    trayectoria.clear();

    cargarSprites();

    // Hacer el Rayo más pequeño (2.33x - reducido 1/3 del tamaño anterior)
    setScale(5);

    qDebug() << "Rayo creado como proyectil simple";
}

Rayo::~Rayo()
{
    if (timer) {
        timer->stop();
    }
    qDebug() << "Rayo destruido";
}

void Rayo::cargarSprites()
{
    // Cargar sprites del Rayo (set 1)
    hame1 = QPixmap(":/Piccolo/Sprites/piccolo/hame3.png");
    hame2 = QPixmap(":/Piccolo/Sprites/piccolo/hame2.png");
    hame3 = QPixmap(":/Piccolo/Sprites/piccolo/hame1.png");
    hame1 = hame1.transformed(transform);
    hame2 = hame2.transformed(transform);
    hame3 = hame3.transformed(transform);

    // Cargar sprites del Rayo (set 2)
    hameha1 = QPixmap(":/Piccolo/Sprites/piccolo/hameha3.png");
    hameha2 = QPixmap(":/Piccolo/Sprites/piccolo/hameha2.png");
    hameha3 = QPixmap(":/Piccolo/Sprites/piccolo/hameha1.png");
    hameha1 = hameha1.transformed(transform);
    hameha2 = hameha2.transformed(transform);
    hameha3 = hameha3.transformed(transform);

    // Verificar si los sprites se cargaron correctamente
    bool hameValidos = !hame1.isNull() && !hame2.isNull() && !hame3.isNull();
    bool hamehaValidos = !hameha1.isNull() && !hameha2.isNull() && !hameha3.isNull();

    if (!hameValidos || !hamehaValidos) {
        qDebug() << "Advertencia: Algunos sprites del Rayo no se cargaron, usando placeholders";

        // Placeholders para hame
        if (hame1.isNull()) {
            hame1 = QPixmap(50, 25);
            hame1.fill(Qt::blue);
        }
        if (hame2.isNull()) {
            hame2 = QPixmap(40, 25);
            hame2.fill(Qt::yellow);
        }
        if (hame3.isNull()) {
            hame3 = QPixmap(60, 25);
            hame3.fill(Qt::red);
        }

        // Placeholders para hameha
        if (hameha1.isNull()) {
            hameha1 = QPixmap(50, 25);
            hameha1.fill(Qt::cyan);
        }
        if (hameha2.isNull()) {
            hameha2 = QPixmap(40, 25);
            hameha2.fill(Qt::magenta);
        }
        if (hameha3.isNull()) {
            hameha3 = QPixmap(60, 25);
            hameha3.fill(Qt::green);
        }

        spritesValidos = false;
    } else {
        spritesValidos = true;
        qDebug() << "Sprites del Rayo (hame y hameha) cargados correctamente";
    }

    // Calcular dimensiones del rayo compuesto con factor de escala
    float factorEscala = 1.0f; // Hacer el Rayo aún más pequeño
    anchoTotal = (hame1.width() + hame2.width() + hame3.width()) * factorEscala;
    altoTotal = qMax(hame1.height(), qMax(hame2.height(), hame3.height())) * factorEscala;
}

void Rayo::crear(float x, float y, float dirX, float dirY, float velocidad, float alcance)
{
    this->x = x;
    this->y = y;
    this->dirX = dirX;
    this->dirY = dirY;
    this->velocidad = velocidad;
    this->alcance = alcance;
    this->distanciaRecorrida = 0;
    this->activo = true;

    // Activar el hitbox
    hitboxActivo = true;
    objetosGolpeados.clear();

    // Posicionar el proyectil
    setPos(x, y);
    setZValue(100);

    // Iniciar el timer de actualización
    timer->start(32); // 30 FPS (más lento que 60 FPS)

    // Iniciar la animación intercalada
    iniciarAnimacion();

    qDebug() << "Rayo creado en posición:" << x << "," << y
             << "dirección:" << dirX << "," << dirY
             << "velocidad:" << velocidad;
}

void Rayo::mover()
{
    if (!activo) return;

    // Mover el proyectil
    x += dirX * velocidad;
    y += dirY * velocidad;

    // Actualizar posición en la escena
    setPos(x, y);

    // Actualizar distancia recorrida
    distanciaRecorrida += velocidad;

    // Verificar colisiones si el hitbox está activo
    if (hitboxActivo) {
        verificarColisiones();
    }

    // Verificar si ha alcanzado el alcance máximo
    if (distanciaRecorrida >= alcance) {
        destruir();
        return;
    }

    // Verificar límites de pantalla (opcional)
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenRect = screen->geometry();
        if (x < 0 || x > screenRect.width() || y < 0 || y > screenRect.height()) {
            destruir();
            return;
        }
    }
}

void Rayo::actualizar()
{
    mover();
}

void Rayo::actualizar_timeout()
{
    actualizar();
}

void Rayo::dibujar(QPainter *painter)
{
    if (!activo) return;

    construirRayo();
}

void Rayo::construirRayo()
{
    // Este método se encarga de construir el rayo compuesto
    // La lógica de dibujo está en paint()
}

bool Rayo::estaActivo() const
{
    return activo;
}

void Rayo::destruir()
{
    activo = false;
    timer->stop();

    // Detener la animación intercalada
    detenerAnimacion();

    // Remover de la escena
    if (scene()) {
        scene()->removeItem(this);
    }

    qDebug() << "Rayo destruido";
}

QRectF Rayo::boundingRect() const
{
    // Usar las dimensiones calculadas del rayo compuesto
    return QRectF(0, 0, anchoTotal, altoTotal);
}

void Rayo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (!activo) {
        return;
    }

    float factorEscala = 1.0f; // Hacer el Rayo aún más pequeño
    qreal posX = 0;
    qreal centroY = altoTotal / 2.0; // Centro vertical del rayo

    // Seleccionar qué set de sprites usar (hame o hameha)
    QPixmap sprite1 = usarHameha ? hameha1 : hame1;
    QPixmap sprite2 = usarHameha ? hameha2 : hame2;
    QPixmap sprite3 = usarHameha ? hameha3 : hame3;

    // Dibujar sprite1 (inicio del rayo)
    if (!sprite1.isNull()) {
        QPixmap sprite1Scaled = sprite1.scaled(sprite1.width() * factorEscala, sprite1.height() * factorEscala, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qreal y = centroY - sprite1Scaled.height() / 2.0; // Centrar verticalmente
        painter->drawPixmap(posX, y, sprite1Scaled);
        posX += sprite1Scaled.width();
    } else {
        qreal anchoPlaceholder = 50 * factorEscala;
        qreal altoPlaceholder = 25 * factorEscala;
        qreal y = centroY - altoPlaceholder / 2.0; // Centrar placeholder
        QColor colorPlaceholder = usarHameha ? Qt::cyan : Qt::blue;
        painter->fillRect(posX, y, anchoPlaceholder, altoPlaceholder, colorPlaceholder);
        posX += anchoPlaceholder;
    }

    // Dibujar sprite2 (cuerpo del rayo)
    if (!sprite2.isNull()) {
        QPixmap sprite2Scaled = sprite2.scaled(sprite2.width() * factorEscala, sprite2.height() * factorEscala, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qreal y = centroY - sprite2Scaled.height() / 2.0; // Centrar verticalmente
        painter->drawPixmap(posX, y, sprite2Scaled);
        posX += sprite2Scaled.width();
    } else {
        qreal anchoPlaceholder = 40 * factorEscala;
        qreal altoPlaceholder = 25 * factorEscala;
        qreal y = centroY - altoPlaceholder / 2.0; // Centrar placeholder
        QColor colorPlaceholder = usarHameha ? Qt::magenta : Qt::yellow;
        painter->fillRect(posX, y, anchoPlaceholder, altoPlaceholder, colorPlaceholder);
        posX += anchoPlaceholder;
    }

    // Dibujar sprite3 (final del rayo)
    if (!sprite3.isNull()) {
        QPixmap sprite3Scaled = sprite3.scaled(sprite3.width() * factorEscala, sprite3.height() * factorEscala, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qreal y = centroY - sprite3Scaled.height() / 2.0; // Centrar verticalmente
        painter->drawPixmap(posX, y, sprite3Scaled);
    } else {
        qreal anchoPlaceholder = 60 * factorEscala;
        qreal altoPlaceholder = 25 * factorEscala;
        qreal y = centroY - altoPlaceholder / 2.0; // Centrar placeholder
        QColor colorPlaceholder = usarHameha ? Qt::green : Qt::red;
        painter->fillRect(posX, y, anchoPlaceholder, altoPlaceholder, colorPlaceholder);
    }

    // Dibujar el hitbox si está habilitado con la tecla H
    if (mostrarHitbox && hitboxActivo) {
        QPen penHitbox(Qt::red, 2, Qt::DashLine);
        painter->setPen(penHitbox);
        painter->setBrush(Qt::NoBrush);

        float ancho = anchoTotal;
        float alto = altoTotal * 0.85f + 1.0f; // Aumentar altura 1 píxel más hacia abajo igual que en getHitbox()
        float offsetY = altoTotal * 0.15f - 3.0f; // Subir hitbox 3 píxeles igual que en getHitbox()

        painter->drawRect(0, offsetY, ancho, alto);
        qDebug() << "Hitbox dibujado - visible con tecla H (1 píxel más alto hacia abajo)";
    }

    qDebug() << "Rayo dibujado con sprites" << (usarHameha ? "hameha" : "hame") << "- ancho total:" << anchoTotal << "alto total:" << altoTotal;
}

void Rayo::iniciarAnimacion()
{
    usarHameha = false; // Empezar con hame
    contadorAnimacion = 0;
    animacionTimer->start();
    qDebug() << "Animación intercalada iniciada";
}

void Rayo::detenerAnimacion()
{
    if (animacionTimer->isActive()) {
        animacionTimer->stop();
        qDebug() << "Animación intercalada detenida";
    }
}

void Rayo::alternarSprites()
{
    usarHameha = !usarHameha; // Alternar entre hame y hameha
    contadorAnimacion++;

    // Forzar una actualización visual
    update();

    qDebug() << "Sprites alternados - usando" << (usarHameha ? "hameha" : "hame") << "frame:" << contadorAnimacion;
}

void Rayo::iniciar(QPointF posicionInicial, QPointF direccion)
{
    qDebug() << "Rayo::iniciar llamado con posición:" << posicionInicial << "dirección:" << direccion;

    // Convertir a nuestro sistema de coordenadas
    float x = posicionInicial.x();
    float y = posicionInicial.y();

    // Normalizar la dirección
    float magnitude = sqrt(direccion.x() * direccion.x() + direccion.y() * direccion.y());
    float dirX = (magnitude > 0) ? direccion.x() / magnitude : 1.0f;
    float dirY = (magnitude > 0) ? direccion.y() / magnitude : 0.0f;

    // Usar valores por defecto para velocidad y alcance
    float velocidad = 15.0f;  // Velocidad más rápida
    float alcance = 600.0f;

    // Llamar al método crear existente
    crear(x, y, dirX, dirY, velocidad, alcance);
}

void Rayo::detener()
{
    qDebug() << "Rayo::detener llamado";
    destruir();
}

// ==================== SISTEMA DE HITBOX ====================

QRectF Rayo::getHitbox() const
{
    if (!activo || !hitboxActivo) {
        return QRectF();
    }

    // El hitbox cubre todo el área del Rayo con altura aumentada
    float factorEscala = 1.0f; // Usar el mismo factor de escala que los sprites
    float ancho = anchoTotal;
    float alto = altoTotal * 0.85f + 1.0f; // Aumentar altura 1 píxel más hacia abajo

    // Posición global del hitbox (movido un poco menos hacia abajo)
    QPointF posicionGlobal = pos();
    float offsetY = altoTotal * 0.15f - 3.0f; // Subir hitbox 3 píxeles más hacia arriba

    return QRectF(posicionGlobal.x(), posicionGlobal.y() + offsetY, ancho, alto);
}

void Rayo::verificarColisiones()
{
    if (!scene() || !hitboxActivo) return;

    // Obtener el hitbox del Rayo
    QRectF hitboxRayo = getHitbox();

    // Obtener todos los items en la escena que colisionan con el hitbox
    QList<QGraphicsItem*> itemsColisionando = scene()->items(hitboxRayo);

    for (QGraphicsItem* item : itemsColisionando) {
        // No colisionar consigo mismo
        if (item == this) continue;

        // No golpear objetos que ya fueron golpeados
        if (objetosGolpeados.contains(item)) continue;

        // Verificar si realmente colisiona
        if (colisionaCon(item)) {
            procesarColision(item);
            objetosGolpeados.append(item);
        }
    }
}

bool Rayo::colisionaCon(QGraphicsItem* item)
{
    if (!item || !hitboxActivo) return false;

    // Obtener los rectángulos de colisión
    QRectF hitboxRayo = getHitbox();
    QRectF boundingRectItem = item->sceneBoundingRect();

    // Verificar intersección
    return hitboxRayo.intersects(boundingRectItem);
}

void Rayo::procesarColision(QGraphicsItem* item)
{
    qDebug() << "¡Rayo colisionó con un objeto!";

    // Si el item es Goku, aplicar daño
    if (Goku* goku = dynamic_cast<Goku*>(item)) {
        goku->recibirDanio(daño); // Usa el valor de daño del rayo
        qDebug() << "Rayo causó" << daño << "puntos de daño a Goku";
        destruir(); // Opcional: destruye el rayo tras golpear
        return;
    }

    // Puedes agregar más lógica para otros tipos de objetos aquí

    // Efectos visuales, sonidos, etc.
    qDebug() << "Procesando colisión con objeto en posición:" << item->pos();
}

void Rayo::alternarVisualizacionHitbox()
{
    mostrarHitbox = !mostrarHitbox;
    qDebug() << "Visualización de hitbox del Rayo:" << (mostrarHitbox ? "ACTIVADA" : "DESACTIVADA");
}
