#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

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

public slots:
    //Actualizar barra vida
    void actualizarBarraVida(int vidaActual, int vidaMaxima);
    void actualizarBarraVida2(int vidaActual, int vidaMaxima);
    // Actualizar barra ki
    void actualizarBarraKi(int kiActual, int kiMaximo);

private slots:
    void actualizarMovimiento();
    void piccoloActualizarMovimiento();

private:
    Ui::game *ui;
    //Manejo de niveles
    int nivelActual = 1;
    bool piccoloDerrotado = false;
    void manejarDerrotaPiccolo();
    void manejarDerrotaGoku();

    // Timer para movimiento continuo
    QTimer* movimientoTimer;
    QTimer* piccoloMovTimer;
    QTimer* piccoloIATimer;

    unsigned int cntPiccolo;
    // Variable para girar en x
    QTransform transform;

    qreal POSICION_ORIGINAL_X = 700;
    qreal POSICION_ORIGINAL_Y = 260;
    
    // Variables para rastrear teclas presionadas
    bool teclaD_presionada = false;
    bool teclaA_presionada = false;
    bool teclaW_presionada = false;
    bool teclaS_presionada = false;
    bool teclaK_presionada = false;
    bool teclaJ_presionada = false;
    bool teclaL_presionada = false;

    // Simular teclas presionadas Piccolo
    bool piccoloD_presionada = false;
    bool piccoloA_presionada = false;
    bool piccoloW_presionada = false;
    bool piccoloS_presionada = false;
    bool piccoloK_presionada = false;
    bool piccoloJ_presionada = false;
    bool piccoloL_presionada = false;
    
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

    // Metodo para crear la barra de vida, ki, y personaje
    QGraphicsPixmapItem* barraVida;
    QGraphicsPixmapItem* carapersonaje;
    QGraphicsPixmapItem* barraKi;

    // Metodo para crear la barra de vida, ki, y personaje
    QGraphicsPixmapItem* barraVida2;
    QGraphicsPixmapItem* carapersonaje2;
    QGraphicsPixmapItem* barraKi2;

    //Musica
    QMediaPlayer* musicPlayer;
    QAudioOutput* audioOutput;

signals:

};

#endif // GAME_H
