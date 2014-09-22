#include "cardhelper.h"

KCardHelper::KCardHelper(KVariant* pCardSetting, vector<char*>* plstTitle)
{
	if (pCardSetting)
	{
		m_pCardSetting = pCardSetting;
	} else {
		m_pCardSetting = NULL;
	}

	m_plstTitle = plstTitle;
}

KCardHelper::~KCardHelper()
{

}

void KCardHelper::ShowCardSettingTable()
{
	if ( m_pCardSetting == NULL )
		printf("card setting table is empty\n");

	// Show Title
	vector<char*>::iterator it = m_plstTitle->begin();
	for (; it != m_plstTitle->end(); it++)
	{
		printf("%s\t", *it);
	}
	printf("\n");

	// 找到数组
	int nSum = m_pCardSetting->GetSum();
	for (int i = 0; i != nSum; i++)
	{
		//printf("show line %d", i);
		KVariant& line = (*m_pCardSetting)[i];
		//line.ShowVariant();

		// 依据 title 来输出
		vector<char*>::iterator it = m_plstTitle->begin();
		for (; it != m_plstTitle->end(); it++)
		{
			//line[*it].ShowVariant();
			//printf("%s: %s\t", *it, line[*it].GetString());
			printf("%s\t", line[*it].GetString());
		}
		printf("\n");
	}
}