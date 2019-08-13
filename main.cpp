#include "render.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Render w;
    w.setGeometry(300, 200, 1200, 800);
    w.show();

    return a.exec();
}
