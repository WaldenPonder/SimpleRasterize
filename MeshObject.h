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
	   
    void draw();

	Matrix matrix_;
	Matrix orgin_matrix_;

 private:
	 inline void vert_shader(Vec4& v, bool& bOutCVV) const;
	 inline void frag_shader() const;

	struct Impl;
	Impl* impl;
};
