/********************************************************************
	Copyright (c) 2014-2014 uloveto.com

	http://www.uloveto.com
	
	created:	2014/09/05
	created:	5:9:2014   10:45
	author:		zero
	
	purpose:	
*********************************************************************/

#ifndef __cardtemplate_h__
#define __cardtemplate_h__

#include "../../Base/include/variant.h"

class KCardHelper
{
public:
	KCardHelper();
	~KCardHelper();

private:
	KVariant m_CardSetting;
};

KCardHelper::KCardHelper()
{
}

KCardHelper::~KCardHelper()
{
}
#endif // __cardtemplate_h__