#pragma once
#include "Matrix.h"
#include "Object.h"

class Mesh
{
 public:
    __class__(Mesh)
	Mesh(const string& filename);
	~Mesh();

 private:
	void load();

	struct Impl;
	Impl* impl;
	friend class MeshObject;
};

class MeshObject : public Object, public InstancePool<MeshObject>
{
 public:
    __class__(MeshObject)
	MeshObject(const Mesh& mesh);
	virtual ~MeshObject();

    inline void vert_shader(Vec3& v);
    inline void frag_shader();

    void draw();

	Matrix matrix_;

 private:
	struct Impl;
	Impl* impl;
};
