#include "MeshObject.h"
#include "tiny_obj_loader.h"
#include "context.h"

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

void MeshObject::vert_shader(Vec3 &v)
{
    v = v * g::context.viewMatrix_ * g::context.projectionMatrix_;
}

void MeshObject::frag_shader()
{

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
			int   in = index[i].vertex_index;
            Vec3 p1(vertices[in * 3], vertices[in * 3 + 1], vertices[in * 3 + 2]);
			in = index[i + 1].vertex_index;
            Vec3 p2(vertices[in * 3], vertices[in * 3 + 1], vertices[in * 3 + 2]);
			in = index[i + 2].vertex_index;
            Vec3 p3(vertices[in * 3], vertices[in * 3 + 1], vertices[in * 3 + 2]);

            vert_shader(p1),  vert_shader(p2), vert_shader(p3);

			Vec3 n1, n2, n3;
			if (normals.size())
			{
				in = index[i].normal_index;
				n1 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
				in = index[i + 1].normal_index;
				n2 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
				in = index[i + 2].normal_index;
				n3 = Vec3(normals[in * 3], normals[in * 3 + 1], normals[in * 3 + 2]);
			}
			
//			if (g::rayTriangleIntersect(ray, p1 * matrix_, p2 * matrix_, p3 * matrix_, t, u, v))
//			{
//				if (t < info.dis && t > .001)
//				{
//					Vec3 v0v1 = p2 - p1;
//					Vec3 v0v2 = p3 - p1;

//					info.setShape(this);
//					info.dis	  = t;
//					info.u		  = u;
//					info.v		  = v;
//					info.hit_pos = ray.distance(t);
//					info.ray	= ray;

//					if (normals.size())
//					{
//						info.normal = n1 * (1 - u - v) + n2 * u + n3 * v;
//					}
//					else
//					{
//						info.normal = v0v1 ^ v0v2;
//					}
//					info.normal.normalize();
//				}
//			}
		}
	}
}
