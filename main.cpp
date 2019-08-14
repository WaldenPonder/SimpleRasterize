#include "render.h"
#include <QApplication>
#include "Object.h"

namespace g {
   Context context;
}

int main(int argc, char *argv[])
{
    static const int width = 1200, height = 800;

    Vec3Array c(width, Vec3());
    g::context.colorBuffer_ = vector<Vec3Array>(height, c);

    FloatArray f(width, 0);
    g::context.depthBuffer_ = vector<FloatArray>(height, f);

    QApplication a(argc, argv);
    Render w;

    w.setGeometry(300, 200, width, height);
    w.show();

    return a.exec();
}
