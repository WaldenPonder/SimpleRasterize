#pragma once
#include "Vec3.h"


class Object
{
public:
	Object();
	~Object();

	std::string objectName_;
	std::string tag_;
};

class Context : public Object
{
public:
    __class__(Context)

    vector<Vec3Array> colorBuffer_;
    vector<FloatArray> depthBuffer_;
};

namespace g {
   extern Context context;
}
