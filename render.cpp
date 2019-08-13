#include "render.h"
#include <QPainter>
#include <QImage>

Render::Render(QWidget *parent) :
    QWidget(parent)
{

}

Render::~Render()
{

}

void Render::paintEvent(QPaintEvent *)
{
    QRgb* pixels = new QRgb[width()*height()];

    QPainter painter(this);
    QImage image((uchar*)pixels, width(), height(), QImage::Format_ARGB32);
    for (int x = 0; x < width(); ++x) {
        for (int y = 0; y < height(); ++y) {

            pixels[ x * height() + y] = qRgb(255, 0, 0);
        }
    }
    painter.drawImage(0, 0, image);

    delete[] pixels;
}
