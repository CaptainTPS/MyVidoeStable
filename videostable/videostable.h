
// videostable.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
//���ӵ�ͷ�ļ� by TPS

// CvideostableApp:
// �йش����ʵ�֣������ videostable.cpp
//

class CvideostableApp : public CWinApp
{
public:
	CvideostableApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CvideostableApp theApp;