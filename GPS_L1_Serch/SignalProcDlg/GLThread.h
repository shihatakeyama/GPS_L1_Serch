// **********************************************************************
// COM ����̃f�[�^��҂��󂯂�X���b�h
//
// ComRecvThread.h
// 
// 2013/06/31  Createed
// 2019/12/22  J�C��
// **********************************************************************

#ifndef GLTHREAD_H
#define GLTHREAD_H

#include "stdafx.h"
//--#include "Resource.h"

#include "GnrlDefine.h"
#include "GnrlThread.h"

//--#include "SignalProcDlgDlg.h"

//--#include "SP_Global.h"

class GLThread :
	public GnrlThread{

public:

	GLThread();

	void setWnd(CWnd *Wnd);


	// Com��M���[�`��
	virtual int32_t run();

	int8_t m_PramTrig;

private:
	CWnd	*m_Wnd;
//--	CString m_ReadLine;
};


#endif // #ifndef GLTHREAD_H
