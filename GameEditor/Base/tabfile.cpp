#include "include/tabfile.h"
#include <assert.h>

KTabFile::KTabFile()
{
	m_hFile = NULL;
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

// 确保 szLine 中过滤掉 \r \n
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
		if (*pCur == d || n == nLineLen - 1)
		{
			int nLen = pCur - pLastCur;
			assert(*pCur != '\r');
			assert(*pCur != '\n');
			if (*pCur != d)
			{
				nLen++;
			}

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
		if (*pCur == '\n' || n == nFileSize - 1)
		{
			int nLen = pCur - pLastCur;
			if (*pCur != '\n')	// pCur 不是 \n, 那么长度需要 +1
			{
				nLen++;
			}

#ifdef WIN32
			else if (*(pCur - 1) == '\r')	// 在 windows 下，必须要过滤掉尾部的 \r
			{
				nLen--;
			}
#endif
			
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

void KTabFile::SaveFile()
{
	assert(m_hFile);
	assert(m_nFileHight > 0);
	unsigned long lContentBytes = 0;
	for (int n = 0; n < m_nFileHight; n++)
	{
		struct TabLine* pLine = m_FileContent[n];
		for (int i = 0; i < pLine->nNum; i++)
		{
			lContentBytes += strlen(pLine->lstTab[i]);
		}

		if (n != m_nFileHight - 1)
		{
			lContentBytes += pLine->nNum;	// 每行需要增加 '\t' + '\n'
#ifdef WIN32
			lContentBytes ++;	//每行需要再加 '\r'
#endif
		}
	}

	char* szContent = new char[lContentBytes];
	memset(szContent, 0, lContentBytes);
	char* pCur = szContent;
	for (int n = 0; n < m_nFileHight; n++)
	{
		struct TabLine* pLine = m_FileContent[n];
		for (int i = 0; i < pLine->nNum; i++)
		{
			strcpy(pCur, pLine->lstTab[i]);
			pCur += strlen(pLine->lstTab[i]);
			if (i != pLine->nNum - 1)
			{
				*pCur = '\t';
				pCur++;
			}
		}

		if (n != m_nFileHight - 1)
		{
#ifdef WIN32
			*pCur = '\r';
			pCur++;
#endif
			*pCur = '\n';
		}
		pCur++;
	}

	rewind(m_hFile);
	unsigned long lWrited = (unsigned long)fwrite(szContent, 1, lContentBytes, m_hFile);
	assert(lWrited == lContentBytes);
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

	const char*	pMode = "r+b";
	/*if (WriteSupport)
	{
	if (g_IsFileExist(PathName))
	pMode = "r+b";
	else
	pMode = "a+b";
	}*/
	m_hFile = fopen(szFile, pMode);

	// 大小写
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

bool KTitleTabFile::InitWithPath(const char* szFile)
{
	m_tabFile.InitWithPath(szFile);

	// 初始化 title 和 width
	struct KTabFile::TabLine** pContent = m_tabFile.GetContent();
	m_nWidth =  pContent[0]->nNum;
	for (int n = 0; n < m_nWidth; n++)
	{
		m_lstTitle.push_back(pContent[0]->lstTab[n]);
	}

	// 确保 Tab 文件的每一行都统一
	for (int n = 1; n < m_tabFile.GetFileHight(); n++)
	{
		struct KTabFile::TabLine* pLine = pContent[n];
		if (pLine->nNum != m_nWidth)
			return false;
	}

	return true;
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

void KTitleTabFile::CloseFile()
{
	m_tabFile.CloseFile();
}