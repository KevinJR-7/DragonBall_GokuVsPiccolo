#ifndef BLASTB_H
#define BLASTB_H

#include "habilidad.h"
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <cmath>

class BlastB : public Habilidad
{
    Q_OBJECT

public:
    enum TipoTrayectoria { LORENZ, ESPIRAL };

    explicit BlastB(QObject *parent = nullptr);
    virtual ~BlastB();

    // Implementación de métodos de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void actualizar() override;
    void detener() override;
    void destruir();

    // Configuración de física y trayectoria
    void crear(qreal x, qreal y, qreal dirX, qreal dirY, qreal velocidad, qreal alcance);
    void configurarCaos(qreal sigma = 10.0, qreal rho = 28.0, qreal beta = 8.0 / 3.0);
    void setTipoTrayectoria(TipoTrayectoria tipo);
    void setCarpetaSprites(const QString& carpeta);

    // Hitbox
    void verificarColisiones();
    void actualizarHitbox();
    QRectF obtenerHitbox() const;
    static void alternarVisualizacionHitbox();

    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    //Auxiliares
    void cargarSprites();

private slots:
    void actualizarAnimacion();
    void actualizarFisica() override;

private:
    // Animación
    QPixmap spriteBlastB1, spriteBlastB2, spriteBlastB3, spriteBlastB4, spriteActual;
    QTimer* timerAnimacion;
    int frameActual;

    // Física y trayectoria
    QPointF posicionBase, direccionBase;
    qreal tiempoVida, tiempoMaximo;
    qreal velocidad, alcance;

    // Lorenz
    qreal x, y, z;
    qreal dx, dy, dz;
    qreal sigma, rho, beta;
    qreal dt, escala;

    // Trayectoria visual
    QList<QPointF> trayectoria;
    int maxPuntosTrayectoria;
    TipoTrayectoria tipoTrayectoria;

    // Hitbox
    QRectF hitbox;
    int daño;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados;
    static bool mostrarHitbox;

    // Sprites
    QString carpetaSprites;

    // Auxiliares

    void calcularSiguientePosicion();
    void actualizarAtractorLorenz();
    QPointF obtenerPosicionCaotica();
};

#endif // BLASTB_H
