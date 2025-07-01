#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

#include "goku.h"

namespace Ui { class game; }

class game : public QMainWindow
{

public:

    Goku* p;

    game(QWidget *parent = nullptr);
    ~game();

    QGraphicsView *view;
    QGraphicsScene *scene;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    
    // Método para salto direccional
    void saltoDireccional();
    
    // Método para verificar y aplicar movimiento continuo
    void verificarMovimientoContinuo();

private slots:
    void actualizarMovimiento();

private:
    Ui::game *ui;
    
    // Timer para movimiento continuo
    QTimer* movimientoTimer;
    
    // Variables para rastrear teclas presionadas
    bool teclaD_presionada = false;
    bool teclaA_presionada = false;
    bool teclaW_presionada = false;
    bool teclaS_presionada = false;

signals:

};

#endif // GAME_H
