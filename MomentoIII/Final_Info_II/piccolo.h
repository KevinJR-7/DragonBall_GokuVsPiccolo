#ifndef PICCOLO_H
#define PICCOLO_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include "personaje.h"

class piccolo : public Personaje
{
    Q_OBJECT
public:
    piccolo(QObject *parent = nullptr);

    void saltar() override;

signals:
};

#endif // PICCOLO_H
