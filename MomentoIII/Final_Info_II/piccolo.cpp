#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include "piccolo.h"

piccolo::piccolo(QObject *parent)
    : Personaje(parent)
{
    // Configurar propiedades específicas de Goku
    establecerNombre("Piccolo");
    establecerCarpetaSprites("piccolo");
    establecerVida(150); // Goku tiene más vida
    establecerVelocidad(10); // Goku es rápido

    // Configurar física del salto específica para Goku
    establecerVelocidadSalto(40.0);      // Goku salta ÉPICAMENTE alto
    establecerFisicaSalto(0.7, 0.03);    // Aún más ligero con menos resistencia

    // Configurar hitbox específica para Goku - más delgada y hacia la izquierda
    establecerHitbox(30, 40, 18, 25); // Hitbox: 30x40 (más delgada) con offset 18,25 (más a la izquierda)

    // Inicializar con el primer frame de la animación idle
    QPixmap imagen(":/Piccolo/Sprites/piccolo/0.png");
    if (imagen.isNull()) {
        qDebug() << "No se pudo cargar la imagen 0.png";
    } else {
        imagen = imagen.scaled(128*2, 128*2, Qt::KeepAspectRatio, Qt::FastTransformation);
        setPixmap(imagen);
    }
    // Iniciar la animación idle
    //iniciarAnimacionIdle();
}

void piccolo::saltar()
{
    if (!saltando) {
        saltando = true;
        alturaOriginal = this->pos().y();
        alturaMaximaAlcanzada = alturaOriginal; // Inicializar altura máxima

        // Inicializar condiciones del salto con ecuaciones diferenciales
        velocidadVertical = -velIn; // Velocidad inicial hacia arriba (negativa en Qt)
        aceleracionVertical = 0.0;  // Se calculará en cada frame
        tiempo = 0;
        frameSaltoActual = 1; // Comenzar con salto1.png

        // Cambiar inmediatamente al primer sprite de salto
        cambiarSprite("salto1");

        jumpTimer->start();
        qDebug() << nombre << "INICIA SALTO - velIn:" << velIn << "velocidadVertical inicial:" << velocidadVertical << "alturaOriginal:" << alturaOriginal;
    }
}
