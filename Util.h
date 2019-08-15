#pragma once
#include <string>
using namespace std;

namespace g
{
	string getDesktopPath();
	//尾部带 "\\"
	string getExeDir();

	//尾部带"/"
	string getResDir();
}
