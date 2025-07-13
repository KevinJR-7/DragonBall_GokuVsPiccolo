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
    //Cambiar fisica según nivel
    enum TipoTrayectoria { LORENZ, ESPIRAL };

    BlastB(QObject *parent = nullptr);
    virtual ~BlastB();
    
    // Implementación de métodos virtuales puros de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void actualizar() override;
    void detener() override;
    void destruir();
    
    // Implementación de métodos virtuales puros de QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // Método para crear el proyectil con parámetros específicos
    void crear(qreal x, qreal y, qreal dirX, qreal dirY, qreal velocidad, qreal alcance);
    
    // Configurar parámetros del atractor de Lorenz
    void configurarCaos(qreal sigma = 10.0, qreal rho = 28.0, qreal beta = 8.0/3.0);
    
    // Método estático para alternar visualización del hitbox
    static void alternarVisualizacionHitbox();
    
    // Métodos de colisión
    void verificarColisiones();
    bool colisionaCon(QGraphicsItem* item);
    void procesarColision(QGraphicsItem* item);
    void actualizarHitbox();
    QRectF obtenerHitbox() const;
    
    // Cambio de nivel
    void setCarpetaSprites(const QString& carpeta);
    void setTipoTrayectoria(TipoTrayectoria tipo);
    //Metodos auxiliares
    void cargarSprites();


private slots:
    void actualizarAnimacion();
    void actualizarFisica() override;
    
private:
    // Sprites para animación
    QPixmap spriteBlastB1;
    QPixmap spriteBlastB2;
    QPixmap spriteBlastB3;
    QPixmap spriteBlastB4;
    QPixmap spriteActual;
    
    // Animación
    QTimer* timerAnimacion;
    int frameActual;
    
    // Variables para trayectoria caótica (Atractor de Lorenz)
    qreal x, y, z;           // Coordenadas en el espacio del atractor
    qreal dx, dy, dz;        // Derivadas
    qreal sigma, rho, beta;  // Parámetros del atractor de Lorenz
    qreal dt;                // Paso de tiempo para integración
    qreal escala;            // Escala para convertir coordenadas del atractor a pantalla
    
    // Posición base del proyectil
    QPointF posicionBase;
    QPointF direccionBase;
    
    // Tiempo de vida
    qreal tiempoVida;
    qreal tiempoMaximo;
    
    // Variables del hitbox
    QRectF hitbox;
    int daño;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados; // Para evitar múltiples hits del mismo objeto
    
    // Variable estática para mostrar hitbox (controlada por tecla H)
    static bool mostrarHitbox;
    
    // Variables para trazado de trayectoria
    QList<QPointF> trayectoria; // Lista de puntos de la trayectoria
    int maxPuntosTrayectoria; // Máximo número de puntos a guardar
    
    // Métodos auxiliares

    void calcularSiguientePosicion();
    void actualizarAtractorLorenz();
    QPointF obtenerPosicionCaotica();

    //Cambio de nivel
    QString carpetaSprites = "goku";
    TipoTrayectoria tipoTrayectoria = LORENZ;



};

#endif // BLASTB_H
