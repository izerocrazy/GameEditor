// GameEditor.cpp : Defines the entry point for the console application.
//

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include "main.h"
#include "../Base/include/tabfile.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// 读取入 tab 文件并转为 variant
	KTabFile file;
	file.InitWithPath("test.txt");

	// variant 传值给一个 cardhelper 对象，并用其创建一个 card 对象

	// 使用 cardhelper 对象，保存数据库

	return 0;
}

