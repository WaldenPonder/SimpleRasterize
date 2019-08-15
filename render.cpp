#include "render.h"
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QTimer>
#include "MeshObject.h"
#include "context.h"
#include "Util.h"
#include "QDateTime"

struct Render::Impl
{
	clock_t		pre = 0;
	QLabel* lable;
};

//画一个透视旋转三角形
Render::Render(QWidget* parent) : QWidget(parent), impl(new Impl)
{
	impl->lable = new QLabel(this);
	impl->lable->setPalette(QPalette(Qt::red));
	impl->lable->setGeometry(20, 20, 60, 30);
	impl->lable->setStyleSheet("color:red;");
}

Render::~Render()
{
	delete impl;
}

void Render::paintEvent(QPaintEvent*)
{
	//--------------------prepare-----------------------------
	float t = clock() - impl->pre;
	impl->lable->setText(QString::number(1000.0 / t, 'f', 2) + " FPS");
	impl->pre = clock();

	QRgb* pixels = new QRgb[width() * height()];
	for (int x = 0; x < width(); ++x)
	{
		for (int y = 0; y < height(); ++y)
		{
			g::context.colorBuffer_[y][x] = g::context.clearColor_;
		}
	}

	//--------------------draw-----------------------------
	for (MeshObject* mo : MeshObject::pool())
		mo->draw();

	//-------------------------------------------------
	QPainter painter(this);
	QImage   image(( uchar* )pixels, width(), height(), QImage::Format_ARGB32);
	for (int x = 0; x < width(); ++x)
	{
		for (int y = 0; y < height(); ++y)
		{

			Vec3 c					 = g::context.colorBuffer_[y][x] * 255;
			pixels[x * height() + y] = c.to_color();
		}
	}
	painter.drawImage(0, 0, image);

	delete[] pixels;
}

void Render::resizeEvent(QResizeEvent* event)
{
	g::context.width_ = event->size().width(), g::context.height_ = event->size().height();
	const float width = g::context.width_, height = g::context.height_;

	g::context.projectionMatrix_.makePerspective(60, width / height, .1, 10000);
	//Y向上
	g::context.viewMatrix_.lookAt(Vec3(), Vec3(0, 0, -1), Vec3(0, 1, 0));

	Vec3Array c(width, Vec3());
	g::context.colorBuffer_ = vector<Vec3Array>(height, c);

	FloatArray f(width, FLT_MAX);
	g::context.depthBuffer_ = vector<FloatArray>(height, f);
}
