#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "player.h"
#include "piccolo.h"

namespace Ui { class game; }

class game : public QMainWindow
{

public:

    player* p;
    piccolo* pic;

    game(QWidget *parent = nullptr);
    ~game();

    QGraphicsView *view;
    QGraphicsScene *scene;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private:
    Ui::game *ui;

signals:

};

#endif // GAME_H
