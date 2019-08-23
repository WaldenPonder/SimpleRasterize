#include "render.h"
#include <QApplication>
#include "context.h"
#include "MeshObject.h"
#include "Util.h"

namespace g
{
	Context context;
	int		rotation_axis = 0;
	bool enable_rotation = true;
	QImage image;
}  // namespace g

#define SCENE1                        1

//纹理
//支持任意比例窗口
int main(int argc, char* argv[])
{
	//-------------------------scene------------------------
#if SCENE1

	Mesh	   cube(g::getResDir() + "cube2.obj");
	MeshObject mo(cube);
	mo.orgin_matrix_ = mo.matrix_ = Matrix::scale(15, 25, 15) * Matrix::translate(-.5, -.5, -30);

#elif SCENE2

	Mesh	   teapot(g::getResDir() + "teapot.obj");
	MeshObject mo2(teapot);
	mo2.orgin_matrix_ = mo2.matrix_ = Matrix::scale(2) * Matrix::rotate(PI / 2, X_AXIS) * Matrix::rotate(-PI / 2, Z_AXIS) * Matrix::translate(-.4, -.4, -3);

	MeshObject mo3(teapot);
	mo3.orgin_matrix_ = mo3.matrix_ = Matrix::scale(1) * Matrix::rotate(PI / 2, X_AXIS) * Matrix::rotate(-PI / 2, Z_AXIS) * Matrix::translate(-.4, -2, -3);

#elif SCENE3

	Mesh	   Buddha(g::getResDir() + "Buddha.obj");
	MeshObject mo3(Buddha);
	mo3.orgin_matrix_ = mo3.matrix_ = Matrix::rotate(PI / 2, Z_AXIS) * Matrix::translate(-.5, -.5, -3);
#endif

	QApplication a(argc, argv);

	Render w;
	w.setGeometry(300, 200, 800, 800);
	w.show();

	return a.exec();
}
