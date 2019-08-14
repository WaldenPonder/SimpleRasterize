#include "MeshObject.h"
#include "tiny_obj_loader.h"
#include "context.h"

float min3(const float &a, const float &b, const float &c)
{
	return std::min(a, std::min(b, c));
}

float max3(const float &a, const float &b, const float &c)
{
	return std::max(a, std::max(b, c));
}

float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
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
		cout << "load " << impl->filename << " fail\n";
	}
}

MeshObject::MeshObject(const Mesh& mesh) : impl(new Impl(mesh))
{

}

MeshObject::~MeshObject()
{
}

void MeshObject::vert_shader(Vec4 &v) const
{
    v = v * g::context.viewMatrix_ * g::context.projectionMatrix_;
}

void MeshObject::frag_shader()
{

}

void MeshObject::transform2screen(Vec4& v) const
{
	v /= v.w();

	v.x() = v.x() * .5 * g::context.width_;
	v.y() = v.y() * .5 * g::context.height_;
	v.z() = -v.z();
}

//https://blog.csdn.net/xiaobaitu389/article/details/75523018

void MeshObject::draw()
{
	float		t, u, v;
	const Mesh& mesh = impl->mesh_;

	for (tinyobj::shape_t& shape : impl->mesh_.impl->shapes)
	{
		std::vector<tinyobj::index_t>& index	= shape.mesh.indices;
		std::vector<tinyobj::real_t>&  vertices = mesh.impl->attrib.vertices;
		std::vector<tinyobj::real_t>&  normals  = mesh.impl->attrib.normals;

		for (int i = 0; i < index.size(); i += 3)
		{
			int   in = 3 * index[i].vertex_index;
            
			Vec4 p1(vertices[in], vertices[in + 1], vertices[in + 2], 1);
			in = 3 * index[i + 1].vertex_index;
            Vec4 p2(vertices[in], vertices[in + 1], vertices[in + 2], 1);
			in = 3 * index[i + 2].vertex_index;
            Vec4 p3(vertices[in], vertices[in + 1], vertices[in + 2], 1);

            vert_shader(p1),  vert_shader(p2), vert_shader(p3);
			transform2screen(p1), transform2screen(p2), transform2screen(p3);
			



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
