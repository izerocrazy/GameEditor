#include "include/tabfile.h"
#include <assert.h>

KTabFile::KTabFile()
{

}

KTabFile::~KTabFile()
{
	CloseFile();

	vector<char*>::iterator it = m_lstTitle.begin();
	for (;it != m_lstTitle.end(); it++)
	{
		delete[] *it;
	}

	list<vector<char*>>::iterator it2 = m_FileContent.begin();
	for (; it2 != m_FileContent.end(); it++)
	{
		it = (*it2).begin();
		for (; it != (*it2).end(); it++)
		{
			delete[] *it;
		}
	}
}

bool KTabFile::OpenFile(const char* szFile)
{
	/*char PathName[MAX_PATH];
	g_GetFullPath(PathName, FileName);*/

#ifdef __linux
	{
		char *ptr = PathName;
		while(*ptr)
		{
			if (*ptr == '\\')
				*ptr = '/';
			ptr++;
		}
	}
#endif	// #ifdef __linux

	const char*	pMode = "rb";
	/*if (WriteSupport)
	{
		if (g_IsFileExist(PathName))
			pMode = "r+b";
		else
			pMode = "a+b";
	}*/
	m_hFile = fopen(szFile, pMode);

#ifdef __linux
	{
		if (m_hFile == NULL)
		{
			g_StrLower(PathName);
			m_hFile = fopen(PathName, pMode);
		}
	}
#endif	// #ifdef __linux

	/*if (WriteSupport && m_hFile)
		CONTENT_BUFFER_NOT_SUPPORT;*/

	return (m_hFile != NULL);
}

void KTabFile::OpenFileStream(const char* szFile)
{
	m_FileStream.close();

	m_FileStream.open(szFile);
}

void KTabFile::CloseFile()
{
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
}

void KTabFile::InitWithPath(const char* szFile)
{
	// 读到文件，获得文件句柄
	/*if (OpenFile(szFile) == false)
	{
		assert(0);
		return;
	}*/
	OpenFileStream(szFile);
	if (m_FileStream.good() == false)
	{
		assert(0);
		return;
	}

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	// 按 line 读
	// 读取第一行作为 lstTitle，确定好 vectory 长度，做一个简单优化
	m_FileStream.getline(szTemp, 1024);
	const char* d = "\t";	// tab 键
	char* p = strtok(szTemp, d);
	m_nWidth = 0;
	while (p)
	{
		printf("%s \n", p);
		char * pNew = new char[128];
		strcpy(pNew, p);
		m_lstTitle.push_back(pNew);

		p = strtok(NULL, d);

		m_nWidth++;
	}
	
	// 读取其余行为 FileContent
	while ((m_FileStream.rdstate() & std::ifstream::eofbit) == 0)
	{
		memset(szTemp, 0, 1024);
		m_FileStream.getline(szTemp, 1024);
		const char* d = "\t";
		char* p = strtok(szTemp, d);
		vector<char*> line;
		
		while (p)
		{
			printf("%s \n", p);
			char * pNew = new char[128];
			strcpy(pNew, p);
			line.push_back(pNew);

			p = strtok(NULL, d);
		}
		m_FileContent.push_back(line);
	}
}