#include <QApplication>
#include "game.h"
#include "mainmenu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainMenu menu;
    if (menu.exec() == QDialog::Accepted) {
        game g;
        g.show();
        return a.exec();
    }
    return 0;
}
