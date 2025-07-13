#ifndef HITBOXATAQUE_H
#define HITBOXATAQUE_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>

class HitboxAtaque : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    HitboxAtaque(qreal x, qreal y, qreal w, qreal h, int daño, int tiempoVidaMs, QObject* parent = nullptr);

protected:
    void advance(int step) override;

private:
    int daño;
    QTimer* timerDestruir;
    bool yaGolpeo = false;


signals:
    void colisionConEnemigo(); // puedes usar esto si quieres efectos visuales

};

#endif // HITBOXATAQUE_H
