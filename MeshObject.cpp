#include "MeshObject.h"
#include "tiny_obj_loader.h"
#include "context.h"
#include "Vec4.h"
#include "QTimer"

float edgeFunction(const Vec4& a, const Vec4& b, const Vec4& c)
{
	return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}

namespace g
{
	Vec3Array colors{ Red, Green, Blue, Yellow, Brown, Cyan, SpicyPink, BlueViolet };
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

//http://www.songho.ca/opengl/gl_projectionmatrix.html
void MeshObject::vert_shader(Vec4& v, bool& bOutCVV) const
{
	bOutCVV = false;

	Vec4 cameraPos = v * matrix_ * g::context.viewMatrix_;

	impl->shader.cameraSpacePos = Vec3(cameraPos.x(), cameraPos.y(), cameraPos.z());

#if 0
	v = cameraPos;

	// calculate the appropriate left, right etc.
	float tan_fovy	= tan(g::DegreesToRadians(60 * 0.5));
	float aspectRatio = g::context.width_ / g::context.height_;
	float n			  = .1;
	float f			  = 10000;
	float r			  = tan_fovy * aspectRatio * n;
	float l			  = -r;
	float t			  = tan_fovy * n;
	float b			  = -t;

	//screen space
	Vec2 screenPos;
	screenPos.x() = n * cameraPos.x() / -cameraPos.z();
	screenPos.y() = n * cameraPos.y() / -cameraPos.z();

	//screen pos to ndc
	Vec2 ndc;
	ndc.x() = 2 * screenPos.x() / (r - l) - (r + l) / (r - l);
	ndc.y() = 2 * screenPos.y() / (t - b) - (t + b) / (t - b);

	//raster space
	v.x() = (ndc.x() + 1) * .5 * g::context.width_;
	v.y() = (1 - ndc.y()) * .5 * g::context.height_;
	v.z() = -1 / cameraPos.z();

#else
	v		= cameraPos * g::context.projectionMatrix_;
	float w = v.w();
	if (v.x() < -w || v.x() > w || v.y() < -w || v.y() > w || v.z() < 0 || v.z() > w)
	{
		bOutCVV = true;
	}

	//	qDebug() << T_VEC4(v);

	v /= v.w();

	v.x() = (v.x() + 1) * .5 * g::context.width_;
	v.y() = (1 - v.y()) * .5 * g::context.height_;
	v.z() = 1 / -cameraPos.z();
#endif
}

void MeshObject::frag_shader() const
{
	Impl::ShaderInfo& info   = impl->shader;
	Vec3			  normal = info.normal;
	normal.normalize();

	Vec3& color = g::context.colorBuffer_[info.y][info.x];
	color		= Vec3(.0);

	Vec3 viewDir = info.cameraSpacePos;
	viewDir.normalize();
	Vec3 lightDir(1, 1, 1);
	lightDir.normalize(), lightDir *= -1;

	const Vec3 COLOR = g::White * .7;
	float	  NDotL = std::abs(lightDir * normal);
	color += COLOR * NDotL;

	Vec3  reflectDir = -lightDir + 2 * normal * NDotL;
	float f			 = reflectDir * viewDir;

	if (f > 0)
	{
		// color += std::pow(f, 5) * COLOR;
	}

	color.max_to_one();
}

//https://blog.csdn.net/xiaobaitu389/article/details/75523018
void MeshObject::draw()
{
	Vec3Array axis{ X_AXIS, Y_AXIS, Z_AXIS };

	matrix_ = Matrix::rotate(.03, axis[g::rotation_axis % 3]) * matrix_;

	float		t, u, v;
	const Mesh& mesh = impl->mesh_;

	auto get_color = [](Vec4 p) {
		int colorIndex = std::pow(2, 2) * p.x() + std::pow(2, 1) * p.y() + std::pow(2, 0) * p.z();
		return g::colors[colorIndex % 8];
	};

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

			Vec4 modelP1 = p1, modelP2 = p2, modelP3 = p3;
			bool bOutCVV1, bOutCVV2, bOutCVV3;
			vert_shader(p1, bOutCVV1), vert_shader(p2, bOutCVV2), vert_shader(p3, bOutCVV3);

			if (bOutCVV1 && bOutCVV2 && bOutCVV3) continue;
	
			int minx = std::min({ p1.x(), p2.x(), p3.x() });
			int maxx = std::max({ p1.x(), p2.x(), p3.x() });
			int miny = std::min({ p1.y(), p2.y(), p3.y() });
			int maxy = std::max({ p1.y(), p2.y(), p3.y() });

			float area = edgeFunction(p1, p2, p3);
			if (area < 0) continue; // || minx < -1000 || miny < -1000 || maxx > g::context.width_ || maxy > g::context.height_) continue;

			for (int x = minx; x <= maxx; x++)
			{
				for (int y = miny; y <= maxy; y++)
				{
					if (x < 0 || x >= g::context.width_ || y < 0 || y >= g::context.height_)
						continue;

					impl->shader.x = x, impl->shader.y = y;

					Vec4 pixelSample(x + .5, y + .5, 0, 0);

					float w0 = edgeFunction(p2, p3, pixelSample);
					float w1 = edgeFunction(p3, p1, pixelSample);
					float w2 = edgeFunction(p1, p2, pixelSample);
					//qDebug() << "\t\t\t area" << area;
					if (w0 >= 0 && w1 >= 0 && w2 >= 0)
					{
						w0 /= area, w1 /= area, w2 /= area;

						float oneOverZ = p1.z() * w0 + p2.z() * w1 + p3.z() * w2;
						float z		   = 1 / oneOverZ;
					//	qDebug() << "\t\t\t oneOverZ" << oneOverZ;

						if (z < g::context.depthBuffer_[y][x])
						{
							g::context.colorBuffer_[y][x] = g::White;
							g::context.depthBuffer_[y][x] = z;

							Vec3 n1, n2, n3;
							if (normals.size())
							{
								in = 3 * index[i].normal_index;
								n1 = Vec3(normals[in], normals[in + 1], normals[in + 2]);

								in = 3 * index[i + 1].normal_index;
								n2 = Vec3(normals[in], normals[in + 1], normals[in + 2]);

								in = 3 * index[i + 2].normal_index;
								n3 = Vec3(normals[in], normals[in + 1], normals[in + 2]);

								//ÄæµÄ×ªÖÃ
								Matrix mat = Matrix::inverse(matrix_ * g::context.viewMatrix_);
								Matrix trans;
								trans.transpose(mat);
								n1 = n1 * trans;
								n2 = n2 * trans;
								n3 = n3 * trans;

								n1 *= p1.z();
								n2 *= p2.z();
								n3 *= p3.z();
								impl->shader.normal = n1 * w0 + n2 * w1 + n3 * w2;
								impl->shader.normal *= z;

								this->frag_shader();
							}
							else
							{
								Vec3 color1 = get_color(modelP1);
								Vec3 color2 = get_color(modelP2);
								Vec3 color3 = get_color(modelP3);

								color1 *= p1.z();
								color2 *= p2.z();
								color3 *= p3.z();

								Impl::ShaderInfo& info  = impl->shader;
								Vec3&			  color = g::context.colorBuffer_[info.y][info.x];
								color					= color1 * w0 + color2 * w1 + color3 * w2;
								color *= z;
							}
						}
					}
				}
			}
		}
	}
}