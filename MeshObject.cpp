#include "MeshObject.h"
#include "tiny_obj_loader.h"
#include "context.h"
#include "Vec4.h"

float min3(const float& a, const float& b, const float& c)
{
	return std::min(a, std::min(b, c));
}

float max3(const float& a, const float& b, const float& c)
{
	return std::max(a, std::max(b, c));
}

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

	bool uesExtent = true;
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
	//qDebug() << "AA " << VEC4(v) << endl;
	v = v * matrix_ * g::context.viewMatrix_ * g::context.projectionMatrix_;
	//qDebug() << "BB " << VEC4(v) << endl;
}

void MeshObject::frag_shader()
{
}

void MeshObject::transform2screen(Vec4& v) const
{
	v /= v.w();
	//qDebug() << "#VV " << VEC4(v);

	v.x() = (v.x() + 1) * .5 * g::context.width_;
	v.y() = (v.y() + 1) * .5 * g::context.height_;
	v.z() = -v.z();

	//qDebug() << "VV " << VEC4(v);
}

//https://blog.csdn.net/xiaobaitu389/article/details/75523018

void MeshObject::draw()
{
	matrix_ = Matrix::rotate(.05, Y_AXIS) * matrix_;

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

			int minx = min3(p1.x(), p2.x(), p3.x());
			int maxx = max3(p1.x(), p2.x(), p3.x());
			int miny = min3(p1.y(), p2.y(), p3.y());
			int maxy = max3(p1.y(), p2.y(), p3.y());

			float area = edgeFunction(p1, p2, p3);
			for (int x = minx; x <= maxx; x++)
			{
				for (int y = miny; y <= maxy; y++)
				{
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

						float z = 1 / oneOverZ;

						//if (z < g::context.depthBuffer_[y][x])
						{
							g::context.colorBuffer_[y][x] = g::Red;
						}
					}
				}
			}

			//Vec3 n1, n2, n3;
			//if (normals.size())
			//{
			//	in = index[i].normal_index;
			//	n1 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
			//	in = index[i + 1].normal_index;
			//	n2 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
			//	in = index[i + 2].normal_index;
			//	n3 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
			//}
		}
	}
}
