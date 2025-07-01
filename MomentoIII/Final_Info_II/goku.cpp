#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include "goku.h"

Goku::Goku(QObject *parent)
    : Personaje(parent)
{
    // Inicializar dirección horizontal
    ultimaDireccionHorizontal = "";
    
    // Configurar propiedades específicas de Goku
    establecerNombre("Goku");
    establecerCarpetaSprites("goku");
    establecerVida(150); // Goku tiene más vida
    establecerVelocidad(10); // Goku es rápido
    
    // Configurar física del salto específica para Goku
    establecerVelocidadSalto(65.0);      // Goku salta SÚPER alto (aumentado de 50 a 65)
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia
    
    // Configurar hitbox específica para Goku - más delgada y hacia la izquierda
    establecerHitbox(20, 40, 12, 25); // Hitbox: 20x40 (1/3 más delgada) con offset 12,25 (aún más a la izquierda)
    
    // Configurar hitbox de salto - menos largo, más a la izquierda y más abajo
    establecerHitboxSalto(22, 20, 9, 20); // Hitbox de salto: más ancha (22), menos largo (20), más a la izquierda (9) y más abajo (20)
    
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

void Goku::moverDerecha()
{
    qDebug() << "moverDerecha() - estaSaltando:" << estaSaltando() << "pos actual:" << this->pos().x();
    moviendose = true;
    ultimaDireccionHorizontal = "adelante"; // Rastrear dirección horizontal
    
    if (estaSaltando()) {
        // Durante el salto, usar el sistema de movimiento horizontal
        qreal velocidadSalto = velocidadMovimiento * 3; // 3x más rápido en el aire
        qDebug() << "Aplicando movimiento horizontal durante salto:" << velocidadSalto;
        aplicarMovimientoHorizontal(velocidadSalto);
    } else {
        // Movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior + velocidadMovimiento;
        qDebug() << "Movimiento suelo: de" << posicionAnterior << "a" << nuevaX;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("adelante");
    }
    
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Goku::moverIzquierda()
{
    qDebug() << "moverIzquierda() - estaSaltando:" << estaSaltando() << "pos actual:" << this->pos().x();
    moviendose = true;
    ultimaDireccionHorizontal = "atras"; // Rastrear dirección horizontal
    
    if (estaSaltando()) {
        // Durante el salto, usar el sistema de movimiento horizontal
        qreal velocidadSalto = velocidadMovimiento * 3; // 3x más rápido en el aire
        qDebug() << "Aplicando movimiento horizontal durante salto:" << -velocidadSalto;
        aplicarMovimientoHorizontal(-velocidadSalto);
    } else {
        // Movimiento normal en el suelo
        qreal posicionAnterior = this->pos().x();
        qreal nuevaX = posicionAnterior - velocidadMovimiento;
        qDebug() << "Movimiento suelo: de" << posicionAnterior << "a" << nuevaX;
        QGraphicsPixmapItem::setPos(nuevaX, this->pos().y());
        animacionTimer->stop();
        cambiarSprite("atras");
    }
    
    verificarLimitesPantalla(limitesEscena);
    actualizarVisualizacionHitbox();
}

void Goku::moverArriba()
{
    moviendose = true;
    
    // Solo detener animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop(); // Detener animación idle
    }
    
    this->setPos(this->pos().x(), this->pos().y() - velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    
    // Solo cambiar sprite si no está saltando (mantener animación de salto)
    if (!estaSaltando()) {
        // Usar la dirección horizontal si existe, sino "adelante" por defecto
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

void Goku::moverAbajo()
{
    moviendose = true;
    
    // Solo detener animación idle si no está saltando
    if (!estaSaltando()) {
        animacionTimer->stop(); // Detener animación idle
    }
    
    this->setPos(this->pos().x(), this->pos().y() + velocidadMovimiento);
    verificarLimitesPantalla(limitesEscena); // Verificar límites después del movimiento
    actualizarVisualizacionHitbox(); // Actualizar hitbox visual
    
    // Solo cambiar sprite si no está saltando (mantener animación de salto)
    if (!estaSaltando()) {
        // Usar la dirección horizontal si existe, sino "adelante" por defecto
        if (!ultimaDireccionHorizontal.isEmpty()) {
            cambiarSprite(ultimaDireccionHorizontal);
        } else {
            cambiarSprite("adelante");
        }
    }
}

void Goku::atacar()
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

void Goku::recibirDanio(int danio)
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

void Goku::iniciarAnimacionIdle()
{
    // Limpiar dirección horizontal al entrar en idle
    ultimaDireccionHorizontal = "";
    
    // Llamar al método de la clase padre
    Personaje::iniciarAnimacionIdle();
}

// void Goku::saltar()
// {
//     posY = yIn + (-velIn * tiempo) + (0.5 * 9.8 * tiempo * tiempo);
//     velY = -velIn + 9.8 * tiempo;
//     setPos(this->pos().x(), posY);
//     tiempo += 0.1;

//     // if (posY>viewRect.height()-30||posY<0){
//     //     velIn = sqrt(velX * velX + velY * velY)*0.8;



//     //     theta = atan2(velY,velX);
//     //     qDebug()<<"Choque en Y"<<atan2(velY,velX)<<cos(theta);
//     //     velIn = 0.8*velIn;
//     //     tiempo = 0;
//     //     if(posY<0){
//     //         dirY = -1;}
//     //     else{dirY = 1;}
//     //     yIn = posY-10*dirY;

//     //     xIn = posX;

//     //     //if (velIn<5){timerMovPar->stop();}
//     // }
// }
