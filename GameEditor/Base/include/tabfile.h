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
protected:
	bool OpenFile(const char* szFile);
	void OpenFileStream(const char* szFile);
	void CloseFile();

private:
	FILE*					m_hFile;
	ifstream				m_FileStream;

	int						m_nFileHight;
	int						m_nWidth;

	vector<char*>			m_lstTitle;
	list<vector<char*>>		m_FileContent;
};

#endif // __File_h__
