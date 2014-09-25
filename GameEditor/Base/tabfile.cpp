#include "include/tabfile.h"
#include <assert.h>

KTabFile::KTabFile()
{

}

KTabFile::~KTabFile()
{
	// CloseFileStream();
	CloseFile();

	for (int n = 0; n < m_nFileHight; n++)
	{
		for (int i = 0; i < m_FileContent[n]->nNum; i++)
		{
			delete m_FileContent[n]->lstTab[i];
		}

		delete m_FileContent[n];
	}
}

/*void KTabFile::OpenFileStream(const char* szFile)
{
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
	m_FileStream.close();

	m_FileStream.open(szFile);
}

void KTabFile::CloseFileStream()
{
	m_FileStream.close();
}*/

struct KTabFile::TabLine* KTabFile::CreateTabLine(const char* szLine, size_t nLineLen)
{
	struct TabLine* pRet = new struct TabLine;
	pRet->nNum = 0;

	char d = '\t';
	if (nLineLen == 0)
	{
		nLineLen = strlen(szLine);
	}

	if (nLineLen == 0)
	{
		assert(0);
		return NULL;
	}

	// 遍历第一遍，找出多少个 \t
	for (int n = 0; n < nLineLen; n++)
	{
		if (szLine[n] == d || n == nLineLen - 1)	// 当到达最后一个时，即便不是 \t 同样也要算上
		{
			pRet->nNum++;
		}
	}
	
	if (pRet->nNum == 0)
	{
		return pRet;
	}

	// 第二次遍历
	pRet->lstTab = new char*[pRet->nNum];
	const char* pCur = szLine;
	const char* pLastCur = pCur;
	int nTabCur = 0;
	for (int n = 0; n < nLineLen; n++)
	{
		if (szLine[n] == d || n == nLineLen - 1)
		{
			int nLen = pCur - pLastCur;
			if (nLen > 0)
			{
				char* pTemp = new char[nLen + 1];
				pTemp[nLen] = '\0';
				memcpy(pTemp, pLastCur, nLen);
				pRet->lstTab[nTabCur] = pTemp;
			}
			else
			{
				pRet->lstTab[nTabCur] = "";
			}

			nTabCur++;
			pCur++;
			pLastCur = pCur;	// last cur 要跳过 \t
		} 
		else 
		{
			pCur++;
		}
	}

	return pRet;
}

void KTabFile::InitWithPath(const char* szFile)
{
	// 读到文件，获得文件句柄
	if (OpenFile(szFile) == false)
	{
		assert(0);
		return;
	}

	long Offset = ftell(m_hFile);
	fseek(m_hFile, 0, SEEK_END);
	int nFileSize = ftell(m_hFile);
	fseek(m_hFile, Offset, SEEK_SET);

	if (nFileSize == 0)
	{
		assert(0);
		return;
	}

	// 读取出文件所有字符串
	char * szFileContent = new char[nFileSize + 1];
	szFileContent[nFileSize] = 0;
	long lBytesReaded = (unsigned long)fread(szFileContent, 1, nFileSize, m_hFile);
	if (lBytesReaded != nFileSize)
	{
		assert(0);
		return;
	}

	m_nFileHight = 0;
	// 第一次遍历，得出多少行
	for (int n = 0; n < nFileSize; n++)
	{
		if (szFileContent[n] == '\n' || n == nFileSize - 1)
		{
			m_nFileHight++;
		}
	}

	// 第二次组合出 TabLine
	m_FileContent = new struct TabLine*[m_nFileHight];
	int nFileCur = 0;
	char* pCur = szFileContent;
	char* pLastCur = pCur;
	for (int n = 0; n < nFileSize; n++)
	{
		if (szFileContent[n] == '\n' || n == nFileSize - 1)
		{
			int nLen = pCur - pLastCur;
			if (n == nFileSize - 1)
			{
				nLen++;
			}

			m_FileContent[nFileCur] = CreateTabLine(pLastCur, nLen);

			nFileCur++;
			pCur++;
			pLastCur = pCur;
		}
		else
		{
			pCur++;
		}
	}

	delete szFileContent;
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

void KTabFile::CloseFile()
{
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

KTitleTabFile::KTitleTabFile()
{

}

KTitleTabFile::~KTitleTabFile()
{
	vector<char*>::iterator it = m_lstTitle.begin();
	for (;it != m_lstTitle.end(); it++)
	{
		delete[] *it;
	}

	m_lstTitle.clear();
}

void KTitleTabFile::InitWithPath(const char* szFile)
{
	m_tabFile.InitWithPath(szFile);

	// 初始化 title 和 width
	struct KTabFile::TabLine** pContent = m_tabFile.GetContent();
	m_nWidth =  pContent[0]->nNum;
	for (int n = 0; n < m_nWidth; n++)
	{
		m_lstTitle.push_back(pContent[0]->lstTab[n]);
	}
}

list<vector<char*>> KTitleTabFile::GetContent()
{
	list<vector<char*>> lstContent;
	assert(m_tabFile.GetFileHight() > 0);

	// 需要去掉第一行
	struct KTabFile::TabLine** pContent = m_tabFile.GetContent();
	for (int n = 1; n < m_tabFile.GetFileHight(); n ++)
	{
		vector<char*> vecLine;
		struct KTabFile::TabLine* pLine = pContent[n];
		for (int i = 0; i < pLine->nNum; i++)
		{
			vecLine.push_back(pLine->lstTab[i]);
		}

		lstContent.push_back(vecLine);
	}

	return lstContent;
}