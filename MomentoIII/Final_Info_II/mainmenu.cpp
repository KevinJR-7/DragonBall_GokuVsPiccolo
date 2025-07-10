#include "mainmenu.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QIcon>
#include <QSpacerItem>

MainMenu::MainMenu(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Dragon Ball - Menú Principal");

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QPixmap fondoPixmap(":/Fondos/Sprites/gui_scenes/menu_bg.png");
    QSize tamañoImagen = fondoPixmap.size();
    QSize tamañoReducido(tamañoImagen.width() / 2, tamañoImagen.height() / 2);
    fondoPixmap = fondoPixmap.scaled(tamañoReducido, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setFixedSize(tamañoReducido);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - tamañoReducido.width()) / 2;
    int y = (screenGeometry.height() - tamañoReducido.height()) / 2;
    move(x, y);

    QLabel *bg = new QLabel(this);
    bg->setPixmap(fondoPixmap);
    bg->setGeometry(0, 0, width(), height());
    bg->setStyleSheet("background: transparent;");
    bg->lower();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);

    // Espaciador más grande para bajar el botón aún más
    layout->addSpacerItem(new QSpacerItem(20, height() * 0.7, QSizePolicy::Minimum, QSizePolicy::Fixed));

    QPushButton *playButton = new QPushButton(this);
    QPixmap botonPixmap(":/Fondos/Sprites/gui_scenes/boton_start.png");
    QSize botonReducido(botonPixmap.width() / 6, botonPixmap.height() / 6); // Más pequeño aún
    botonPixmap = botonPixmap.scaled(botonReducido, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    playButton->setIcon(QIcon(botonPixmap));
    playButton->setIconSize(botonReducido);
    playButton->setFixedSize(botonReducido);
    playButton->setFlat(true);
    playButton->setStyleSheet("background: transparent; border: none;");

    layout->addWidget(playButton, 0, Qt::AlignHCenter);
    layout->addStretch();

    connect(playButton, &QPushButton::clicked, this, &QDialog::accept);
}
