#include "render.h"
#include <QApplication>
#include "context.h"

namespace g {
   Context context;
}

int main(int argc, char *argv[])
{
    g::context.width_ = 1200,  g::context.height_ = 800;
    const float width = g::context.width_, height = g::context.height_;

    g::context.projectionMatrix_.makePerspective(60, width / height, 1, 10000);
    //Y向上
    g::context.viewMatrix_.lookAt(Vec3(), Vec3(0, 0, -1), Vec3(0, 1, 0));

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
