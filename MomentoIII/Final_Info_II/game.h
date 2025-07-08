#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

#include "piccolo.h"
#include "goku.h"

namespace Ui { class game; }

class game : public QMainWindow
{

public:

    Piccolo* pic;
    Goku* p;

    game(QWidget *parent = nullptr);
    ~game();

    QGraphicsView *view;
    QGraphicsScene *scene;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *event) override; // Para manejar redimensionamiento
    
    // Método para salto direccional
    void saltoDireccional();
    
    // Método para verificar y aplicar movimiento continuo
    void verificarMovimientoContinuo();
    
    // Método para debug de posicionamiento
    void agregarGrillaDebug();
    void alternarGrillaDebug();

private slots:
    void actualizarMovimiento();
    void piccoloActualizarMovimiento();

private:
    Ui::game *ui;
    
    // Timer para movimiento continuo
    QTimer* movimientoTimer;
    QTimer* piccoloMovTimer;
    QTimer* piccoloIATimer;
    
    // Variables para rastrear teclas presionadas
    bool teclaD_presionada = false;
    bool teclaA_presionada = false;
    bool teclaW_presionada = false;
    bool teclaS_presionada = false;
    bool teclaK_presionada = false; // Para recarga de ki
    bool teclaJ_presionada = false; // Para Kamehameha
<<<<<<< HEAD
    bool teclaL_presionada = false; // Para animación de ráfaga
=======

    bool piccoloD_presionada = false;
    bool piccoloA_presionada = false;
    bool piccoloW_presionada = false;
    bool piccoloS_presionada = false;
    bool piccoloK_presionada = false; // Para recarga de ki
    bool piccoloJ_presionada = false; // Para Kamehameha
>>>>>>> 8452abc3670219a93dcd4466d3c018a592ec334b
    
    // Variables para grilla de debug
    QList<QGraphicsLineItem*> lineasGrilla;
    bool grillaVisible = true;
    
    // Variables para fondo
    QGraphicsPixmapItem* fondoItem;
    int fondoActual; // Índice del fondo actual
    QStringList fondosDisponibles; // Lista de fondos disponibles
    
    // Método para configurar el fondo
    void configurarFondo();
    
    // Método para cambiar al siguiente fondo
    void cambiarFondo();
    
    // Método para alternar hitbox específicamente del Kamehameha
    void alternarHitboxKamehameha();

signals:

};

#endif // GAME_H
