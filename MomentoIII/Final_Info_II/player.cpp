#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include "player.h"

player::player(QObject *parent)
    : Personaje(parent)
{
    // Configurar propiedades específicas de Goku
    establecerNombre("Goku");
    establecerCarpetaSprites("goku");
    establecerVida(150); // Goku tiene más vida
    establecerVelocidad(10); // Goku es rápido
    
    // Configurar física del salto específica para Goku
    establecerVelocidadSalto(40.0);      // Goku salta ÉPICAMENTE alto
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia
    
    // Configurar hitbox específica para Goku - más delgada y hacia la izquierda
    establecerHitbox(30, 40, 18, 25); // Hitbox: 30x40 (más delgada) con offset 18,25 (más a la izquierda)
    
    // Inicializar con el primer frame de la animación idle
    QPixmap imagen(":/Goku/Sprites/goku/base1.png");
    if (imagen.isNull()) {
        qDebug() << "No se pudo cargar la imagen base1.png";
    } else {
        setPixmap(imagen);
    }    
    // Iniciar la animación idle
    iniciarAnimacionIdle();
}

void player::detectarColisiones() {
    QList<QGraphicsItem*> colisiones = collidingItems();

    for(QGraphicsItem* item : colisiones)
    {
        ground* g = dynamic_cast<ground*>(item);
        if(g)
        {
            if(fallTimer->isActive()){ fallTimer->stop(); }
            if(jumpTimer->isActive()){ jumpTimer->stop(); }
            falling = false;
            jumping = false;
            posY = this->pos().y();
            posY_s = 0;
            velY = 0;
            tiempo = 0;
            yIn = posY;
        }
    }
}

void player::moverDerecha()
{
    moviendose = true;
    animacionTimer->stop(); // Detener animación idle
    this->setPos(this->pos().x() + velocidadMovimiento, this->pos().y());
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    cambiarSprite("adelante");
}

void player::moverIzquierda()
{
    moviendose = true;
    animacionTimer->stop(); // Detener animación idle
    this->setPos(this->pos().x() - velocidadMovimiento, this->pos().y());
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    cambiarSprite("atras");
}

void player::moverArriba()
{
    moviendose = true;
    animacionTimer->stop(); // Detener animación idle
    this->setPos(this->pos().x(), this->pos().y() - velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    cambiarSprite("adelante");
}

void player::moverAbajo()
{
    moviendose = true;
    animacionTimer->stop(); // Detener animación idle
    this->setPos(this->pos().x(), this->pos().y() + velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    cambiarSprite("adelante");
}

void player::atacar()
{
    if (estaVivo()) {
        qDebug() << nombre << " está atacando con Kamehameha";

        cambiarSprite("atacando"); //
        
        emit personajeAtaco(this);
        
        // Volver a la animación idle después de un tiempo
        QTimer::singleShot(500, this, [this]() {
            if (!moviendose) {
                iniciarAnimacionIdle();
            }
        });
    }
}

void player::recibirDanio(int danio)
{
    if (estaVivo()) {
        vida -= danio;
        if (vida < 0) vida = 0;
        
        qDebug() << nombre << " recibió" << danio << "de daño. Vida restante:" << vida;
        emit vidaCambiada(vida, vidaMaxima);
        
        if (vida <= 0) {
            morir();
        }
    }
}

void player::caer()
{
    posY = yIn + (-velIn * tiempo) + (0.5 * 9.8 * tiempo * tiempo);
    velY = -velIn + 9.8 * tiempo;
    //qDebug() << "posY:" << posY;

    setPos(this->pos().x(), posY);
    tiempo += 0.1;

}

void player::saltar()
{
    posY_s = yIn + (velIn * tiempo) + (0.5 * 20 * tiempo * tiempo);
    velY = velIn + 20 * tiempo;
    setPos(this->pos().x(), posY_s);
    if(!(fallTimer->isActive())){ fallTimer->start(100); }
    tiempo += 0.1;

    // if (posY>viewRect.height()-30||posY<0){
    //     velIn = sqrt(velX * velX + velY * velY)*0.8;



    //     theta = atan2(velY,velX);
    //     qDebug()<<"Choque en Y"<<atan2(velY,velX)<<cos(theta);
    //     velIn = 0.8*velIn;
    //     tiempo = 0;
    //     if(posY<0){
    //         dirY = -1;}
    //     else{dirY = 1;}
    //     yIn = posY-10*dirY;

    //     xIn = posX;

    //     //if (velIn<5){timerMovPar->stop();}
    // }
}
