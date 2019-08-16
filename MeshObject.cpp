#include "MeshObject.h"
#include "tiny_obj_loader.h"
#include "context.h"
#include "Vec4.h"
#include "QTimer"

float edgeFunction(const Vec4& a, const Vec4& b, const Vec4& c)
{
	return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}

struct Mesh::Impl
{
	tinyobj::attrib_t				 attrib;
	std::vector<tinyobj::shape_t>	shapes;
	std::vector<tinyobj::material_t> materials;
	bool							 triangulate = true;
	string							 filename;
};

struct MeshObject::Impl
{
	Impl(const Mesh& mesh) : mesh_(mesh) {}
	const Mesh& mesh_;

	struct ShaderInfo
	{
		int  x, y;
		Vec3 normal;
		Vec3 cameraSpacePos;
	} shader;

	QTimer timer;
};

Mesh::Mesh(const string& filename) : impl(new Impl)
{
	impl->filename = filename;
	load();
}

Mesh::~Mesh()
{
	delete impl;
}

void Mesh::load()
{
	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&impl->attrib, &impl->shapes, &impl->materials, &warn, &err, impl->filename.c_str(),
								NULL, impl->triangulate);

	if (!ret)
	{
		qDebug() << "load " << impl->filename.c_str() << " fail\n";
	}
}

MeshObject::MeshObject(const Mesh& mesh) : impl(new Impl(mesh))
{
}

MeshObject::~MeshObject()
{
}

void MeshObject::vert_shader(Vec4& v) const
{
	//qDebug() << "AA\t" << VEC4(v);
	Vec4 pos					= v * matrix_ * g::context.viewMatrix_;
	v							= pos * g::context.projectionMatrix_;
	impl->shader.cameraSpacePos = Vec3(pos.x(), pos.y(), pos.z());
}

void MeshObject::frag_shader() const
{
	Matrix mat = Matrix::inverse(matrix_ * g::context.viewMatrix_);
	Matrix trans;
	trans.transpose(mat);

	Impl::ShaderInfo& info   = impl->shader;
	Vec3			  normal = info.normal * trans;
	normal.normalize();

	Vec3& color = g::context.colorBuffer_[info.y][info.x];
	color		= Vec3(.1);

	Vec3 viewDir = info.cameraSpacePos;
	viewDir.normalize();
	Vec3 lightDir(1, 1, 1);
	lightDir.normalize(), lightDir *= -1;

	const Vec3 COLOR = g::White;
	float	  NDotL = normal * lightDir;
	color += COLOR * NDotL;

	Vec3  reflectDir = -lightDir + 2 * normal * NDotL;
	float f			 = reflectDir * viewDir;

	if (f > 0)
	{
		color += std::pow(f, 5) * COLOR;
	}
}

void MeshObject::transform2screen(Vec4& v) const
{
	//qDebug() << "BB\t" << VEC4(v);
	v /= v.w();
	//qDebug() << "CC\t" << VEC4(v);
	v.x() = (v.x() + 1) * .5 * g::context.width_;
	v.y() = (v.y() + 1) * .5 * g::context.height_;
	v.z() = 1 - v.z();
	v.z() = 1 / v.z();
}

//https://blog.csdn.net/xiaobaitu389/article/details/75523018

void MeshObject::draw()
{
	matrix_ = Matrix::rotate(.03, Z_AXIS) * matrix_;

	float		t, u, v;
	const Mesh& mesh = impl->mesh_;

	for (tinyobj::shape_t& shape : impl->mesh_.impl->shapes)
	{
		std::vector<tinyobj::index_t>& index	= shape.mesh.indices;
		std::vector<tinyobj::real_t>&  vertices = mesh.impl->attrib.vertices;
		std::vector<tinyobj::real_t>&  normals  = mesh.impl->attrib.normals;

		for (int i = 0; i < index.size(); i += 3)
		{
			int in = 3 * index[i].vertex_index;

			Vec4 p1(vertices[in], vertices[in + 1], vertices[in + 2], 1);
			in = 3 * index[i + 1].vertex_index;
			Vec4 p2(vertices[in], vertices[in + 1], vertices[in + 2], 1);
			in = 3 * index[i + 2].vertex_index;
			Vec4 p3(vertices[in], vertices[in + 1], vertices[in + 2], 1);

			vert_shader(p1), vert_shader(p2), vert_shader(p3);
			transform2screen(p1), transform2screen(p2), transform2screen(p3);

			int minx = std::min({ p1.x(), p2.x(), p3.x() });
			int maxx = std::max({ p1.x(), p2.x(), p3.x() });
			int miny = std::min({ p1.y(), p2.y(), p3.y() });
			int maxy = std::max({ p1.y(), p2.y(), p3.y() });

			float area = edgeFunction(p1, p2, p3);
			for (int x = minx; x <= maxx; x++)
			{
				for (int y = miny; y <= maxy; y++)
				{
					impl->shader.x = x, impl->shader.y = y;

					Vec4 pixelSample(x + .5, y + .5, 0, 0);

					float w0 = edgeFunction(p2, p3, pixelSample);
					float w1 = edgeFunction(p3, p1, pixelSample);
					float w2 = edgeFunction(p1, p2, pixelSample);

					if (w0 >= 0 && w1 >= 0 && w2 >= 0)
					{
						w0 /= area;
						w1 /= area;
						w2 /= area;
						float oneOverZ = p1.z() * w0 + p2.z() * w1 + p3.z() * w2;

						//qDebug() << "\t\t" << oneOverZ;
						float z = 1 / oneOverZ;
						//qDebug() << z;

						if (z < g::context.depthBuffer_[y][x])
						{
							g::context.colorBuffer_[y][x] = g::White;
							g::context.depthBuffer_[y][x] = z;

							Vec3 n1, n2, n3;
							if (normals.size())
							{
								in = 3 * index[i].normal_index;
								n1 = Vec3(normals[in], normals[in + 1], normals[in + 2]);
								n1 /= p1.z();
								in = index[i + 1].normal_index;
								n2 = Vec3(normals[in], normals[in + 1], normals[in + 2]);
								n2 /= p3.z();
								in = index[i + 2].normal_index;
								n3 = Vec3(normals[in], normals[in + 1], normals[in + 2]);
								n3 /= p3.z();

								impl->shader.normal = n1 * w0 + n2 * w1 + n3 * w2;
								impl->shader.normal *= z;

								this->frag_shader();
							}
						}
					}
				}
			}
		}
	}
}
