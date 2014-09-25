/********************************************************************
	Copyright (c) 2014-2014 uloveto.com

	http://www.uloveto.com
	
	created:	2014/09/09
	created:	9:9:2014   14:18
	author:		zero
	
	purpose:	Read File 
*********************************************************************/

#ifndef __File_h__
#define __File_h__

#include <vector>
#include <list>
#include <fstream>

using namespace std;

class KTabFile
{
public:
	KTabFile();
	~KTabFile();

	void InitWithPath(const char* szFile);

	int GetFileHight()
	{
		return m_nFileHight;
	}
	
	struct TabLine
	{
		char**	lstTab;
		int nNum;
	};

	struct KTabFile::TabLine** GetContent()
	{
		return m_FileContent;
	}

private:
	struct KTabFile::TabLine* CreateTabLine(const char* szLine, size_t nLineLen = 0);

	int						m_nFileHight;
	struct TabLine**		m_FileContent;

public:
	bool OpenFile(const char* szFile);
	void CloseFile();
	void SaveFile();
private:
 	FILE*					m_hFile;

/*private:
	ifstream				m_FileStream;
public:
	void OpenFileStream(const char* szFile);
	void CloseFileStream();*/
};


class KTitleTabFile
{
public:
	KTitleTabFile();
	~KTitleTabFile();

	vector<char*>&	GetTitle()
	{
		return m_lstTitle;
	}

	void InitWithPath(const char* szFile);
	list<vector<char*>>	GetContent();
	void SaveFile()
	{
		m_tabFile.SaveFile();
	}
	void CloseFile();

private:
	vector<char*>			m_lstTitle;
	int						m_nWidth;

	KTabFile				m_tabFile;
};

#endif // __File_h__
