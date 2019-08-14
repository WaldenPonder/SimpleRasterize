#pragma once
#include "Matrix.h"
#include "Vec3.h"

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

class MeshObject : public Shape
{
 public:
    __class__(MeshObject)
	MeshObject(const Mesh& mesh);
	virtual ~MeshObject();

    void draw();

	Matrix matrix_;

 private:
	struct Impl;
	Impl* impl;
};
