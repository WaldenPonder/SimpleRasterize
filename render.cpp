#include "render.h"
#include <QPainter>
#include <QImage>
#include "MeshObject.h"
#include "context.h"

//画一个透视旋转三角形
Render::Render(QWidget *parent) :
    QWidget(parent)
{

}

Render::~Render()
{

}

void Render::paintEvent(QPaintEvent *)
{
    for(MeshObject* mo : MeshObject::pool())
        mo->draw();

    QRgb* pixels = new QRgb[width()*height()];
    for (int x = 0; x < width(); ++x) {
        for (int y = 0; y < height(); ++y) {
              pixels[ x * height() + y] = g::context.clearColor_.to_color();
        }
    }

    QPainter painter(this);
    QImage image((uchar*)pixels, width(), height(), QImage::Format_ARGB32);
    for (int x = 0; x < width(); ++x) {
        for (int y = 0; y < height(); ++y) {

            Vec3 c = g::context.colorBuffer_[x][y];
            pixels[ x * height() + y] = c.to_color();
        }
    }
    painter.drawImage(0, 0, image);

    delete[] pixels;
}
