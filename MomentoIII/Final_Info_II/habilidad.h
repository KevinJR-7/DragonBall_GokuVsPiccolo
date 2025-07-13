#ifndef HABILIDAD_H
#define HABILIDAD_H

#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include <QPointF>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class Habilidad : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    
public:
    explicit Habilidad(QObject *parent = nullptr);
    virtual ~Habilidad();
    
    // Métodos virtuales que deben implementar las habilidades específicas
    virtual void iniciar(QPointF posicionInicial, QPointF direccion) = 0;
    virtual void actualizar() = 0;
    virtual void detener() = 0;
    
    // Propiedades generales
    void establecerVelocidad(qreal velocidad);
    void establecerDano(int dano);
    void establecerAlcance(qreal alcance);
    void establecerEscena(QGraphicsScene* escena);
    
    // Getters
    qreal obtenerVelocidad() const { return velocidadHabilidad; }
    int obtenerDano() const { return danoHabilidad; }
    qreal obtenerAlcance() const { return alcanceMaximo; }
    bool estaActiva() const { return activa; }
    
    // Métodos virtuales de QGraphicsItem
    virtual QRectF boundingRect() const override = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override = 0;
    
protected:
    // Propiedades físicas
    qreal velocidadHabilidad;
    int danoHabilidad;
    qreal alcanceMaximo;
    qreal distanciaRecorrida;
    
    // Estado
    bool activa;
    QPointF posicionInicial;
    QPointF direccionMovimiento;
    
    // Timer para actualización física
    QTimer* timerActualizacion;
    QGraphicsScene* escenaJuego;
    
    // Métodos auxiliares
    void verificarLimites();
    
signals:
    void habilidadTerminada(Habilidad* habilidad);
    void colisionDetectada(Habilidad* habilidad, QGraphicsItem* objetivo);
    
protected slots:
    virtual void actualizarFisica();
};

#endif // HABILIDAD_H
