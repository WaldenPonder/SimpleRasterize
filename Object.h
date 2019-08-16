#pragma once
#include "Matrix.h"
#include <QDebug>
#include <QObject>

class Object : public QObject
{
public:
	Object();
	~Object();

	std::string objectName_;
	std::string tag_;
};
