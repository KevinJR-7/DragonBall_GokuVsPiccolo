#include <QBrush>
#include <QPixmap>
#include <QPainter>

#include "ground.h"

ground::ground(QObject *parent)
    : QObject{parent}, QGraphicsRectItem(0, 0, 500, 50)
{
    // setPen(Qt::NoPen); // oculta el borde del rectángulo
    // QPixmap imagen(":/Goku/Sprites/goku/quieto.png");
    // if (imagen.isNull()) {
    //     qDebug() << "No se pudo cargar la imagen";
    // }

    //imagen = imagen.scaled(200, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    setBrush(Qt::darkGray);
    //setFlag(QGraphicsItem::ItemIsMovable);
}
