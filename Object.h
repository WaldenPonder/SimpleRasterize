#pragma once
#include "Matrix.h"
#include <QDebug>

class Object
{
public:
	Object();
	~Object();

	std::string objectName_;
	std::string tag_;
};
