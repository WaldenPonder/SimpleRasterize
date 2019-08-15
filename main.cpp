#include "render.h"
#include <QApplication>
#include "context.h"
#include "MeshObject.h"
#include "Util.h"

namespace g
{
	Context context;
}

int main(int argc, char* argv[])
{
	//-------------------------scene------------------------
	//Mesh cube(g::getResDir() + "cube.obj");
	//MeshObject mo(cube);
	//mo.matrix_ = Matrix::translate(-.5, -.5, -5);
	
	//Mesh	   teapot(g::getResDir() + "teapot.obj");
	//MeshObject mo2(teapot);
	//mo2.matrix_ = Matrix::rotate(PI / 2, X_AXIS) * Matrix::rotate(-PI / 2, Z_AXIS) * Matrix::translate(-.5, -.5, -5);

	Mesh Buddha(g::getResDir() + "Buddha.obj");
	MeshObject mo3(Buddha);
	mo3.matrix_ = Matrix::rotate(PI / 2, Z_AXIS) * Matrix::translate(-.5, -.5, -3);

	QApplication a(argc, argv);

	Render w;
	w.setGeometry(300, 200, 800, 800);
	w.show();

	return a.exec();
}
