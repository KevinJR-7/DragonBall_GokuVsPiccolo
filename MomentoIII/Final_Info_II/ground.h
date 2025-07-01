#ifndef GROUND_H
#define GROUND_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>

class ground : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    ground(QObject *parent = nullptr);

signals:
};

#endif // GROUND_H
