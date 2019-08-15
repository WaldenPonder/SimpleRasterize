#include "Util.h"
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "shlobj.h" 
#include <fstream>
#include <corecrt_io.h>

namespace g {
	string getDesktopPath()
	{
		LPITEMIDLIST pidl;
		LPMALLOC	 pShellMalloc;
		char		 szDir[200];
		if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
			{
				// 如果成功返回true
				SHGetPathFromIDListA(pidl, szDir);
				pShellMalloc->Free(pidl);
			}
			pShellMalloc->Release();
		}

		return string(szDir);
	}

	std::string getExeDir()
	{
		char* str = 0;
		_get_pgmptr(&str);
		string ss(str);
		ss = ss.substr(0, ss.find_last_of("\\") + 1);
		return ss;
	}

	std::string getResDir()
	{
		string s = getExeDir() + "res/";
		if (!_access(s.c_str(), 0))
			return s;
		return getExeDir() + "../res/";
	}
}