// GameEditor.cpp : Defines the entry point for the console application.
//

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include "main.h"
#include "../Base/include/tabfile.h"
#include "../Base/include/variant.h"
#include "./Logic/cardhelper.h"

void ProcessCardSetting()
{
	// 读取入 tab 文件并转为 variant
	KTitleTabFile file;
	file.InitWithPath("../../Configure/card.txt");

	KVariant var;
	list<vector<char*>>& content = file.GetContent();
	list<vector<char*>>::iterator it = content.begin();
	vector<char*>& lstTitle = file.GetTitle();
	int i = 0;
	for (; it != content.end(); it++)
	{
		KVariant temp;
		for (int n = 0; n < lstTitle.size(); n ++)
		{
			temp[lstTitle[n]] = (*it)[n];
		}

		var[i] = temp;
		i++;
	}

	var.ShowVariant();

	// variant 传值给一个 cardhelper 对象，并用其创建一个 card 对象
	KCardHelper CardHelper(&var, &lstTitle);

	// 使用 cardhelper 对象，保存数据库，进行增，删，改操作
	CardHelper.ShowCardSettingTable();

	CardHelper.AddOneCardType();
	printf("----------> Add Line \n");
	CardHelper.ShowCardSettingTable();

	CardHelper.DeleteOneCardType();
	printf("----------> Delete Line \n");
	CardHelper.ShowCardSettingTable();

	CardHelper.SaveToTabFile(file);

	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	ProcessCardSetting();

	return 0;
}

