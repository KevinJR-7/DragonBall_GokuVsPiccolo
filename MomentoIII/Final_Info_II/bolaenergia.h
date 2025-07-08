#ifndef BOLAENERGIA_H
#define BOLAENERGIA_H

#include "habilidad.h"
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class BolaEnergia : public Habilidad
{
    Q_OBJECT
    
public:
    explicit BolaEnergia(QObject *parent = nullptr);
    ~BolaEnergia();
    
    // Sistema simple de proyectil
    void crear(float x, float y, float dirX, float dirY, float velocidad, float alcance);
    void mover();
    void actualizar() override;
    void dibujar(QPainter *painter);
    bool estaActivo() const;
    void destruir();
    
    // Implementación de métodos virtuales puros de Habilidad
    void iniciar(QPointF posicionInicial, QPointF direccion) override;
    void detener() override;
    
    // QGraphicsItem implementation
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // Sistema de hitbox
    QRectF getHitbox() const;
    void verificarColisiones();
    bool colisionaCon(QGraphicsItem* item);
    void procesarColision(QGraphicsItem* item);
    
    // Método estático para alternar visualización del hitbox
    static void alternarVisualizacionHitbox();
    
private:
    // Variables básicas del proyectil
    float x, y;
    float dirX, dirY;
    float velocidad;
    float alcance;
    float distanciaRecorrida;
    bool activo;
    QTimer *timer;
    
    // Sprites de la bola de energía
    QPixmap bola1, bola2, bola3, bola4, bola5;
    bool spritesValidos;
    
    // Configuración del proyectil
    int ancho;
    int alto;
    
    // Sistema de animación
    int frameActual;
    QTimer *animacionTimer;
    
    // Variables del hitbox
    QRectF hitbox;
    int daño;
    bool hitboxActivo;
    QList<QGraphicsItem*> objetosGolpeados;
    
    // Variable estática para mostrar hitbox
    static bool mostrarHitbox;
    
    void cargarSprites();
    void iniciarAnimacion();
    void detenerAnimacion();
    
private slots:
    void actualizar_timeout();
    void siguienteFrame();
};

#endif // BOLAENERGIA_H
