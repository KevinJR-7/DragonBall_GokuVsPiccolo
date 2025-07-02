#include "game.h"
#include "ui_game.h"
#include "player.h"
#include "piccolo.h"

game::game(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::game)
{
    ui->setupUi(this);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);

    view->setScene(scene);
    view->resize(1000, 500);
    view->scale(2.0, 2.0);
    scene->setSceneRect(200, 200, 1000, 500);

    p = new player();
    scene->addItem(p);
    p->setPos(300, 300);

    pic = new piccolo();
    scene->addItem(pic);
    pic->setPos(400, 120);
    
    // Configurar límites de escena para las colisiones
    QRectF limitesJuego(200, 200, 1000, 500); // Mismos límites que la escena
    p->establecerLimitesEscena(limitesJuego);
    
    // Opcional: Escalar Goku (1.5 = 150% del tamaño original)
    // p->establecerEscala(1.5);

    // g = new ground();
    // scene->addItem(g);
    // g->setPos(300, 400);

    //p->moverPlayer();

}

game::~game()
{
    delete ui;
}

void game::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_D){ p->moverDerecha(); }
    if(e->key() == Qt::Key_A){ p->moverIzquierda(); }
    if(e->key() == Qt::Key_W){ p->moverArriba(); }
    if(e->key() == Qt::Key_S){ p->moverAbajo(); }
    if(e->key() == Qt::Key_Space && !p->estaSaltando()){ 
        p->saltar(); 
        qDebug() << "Tecla ESPACIO presionada - iniciando salto";
    }
    if(e->key() == Qt::Key_H){ 
        // Alternar visualización de hitbox con tecla H
        if (p->estaHitboxVisible()) {
            p->ocultarHitbox();
            qDebug() << "Hitbox oculta";
        } else {
            p->mostrarHitbox();
            qDebug() << "Hitbox visible";
        }
    }
}

void game::keyReleaseEvent(QKeyEvent *e)
{
    // Cuando se suelta cualquier tecla de movimiento, iniciar animación idle
    if(e->key() == Qt::Key_D || e->key() == Qt::Key_A || 
       e->key() == Qt::Key_W || e->key() == Qt::Key_S) {
        p->iniciarAnimacionIdle();
    }
}
