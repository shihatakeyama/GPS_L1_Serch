// **********************************************************************
// COM からのデータを待ち受けるスレッド
//
// GLThread.cpp
// 
// 2013/06/31  Createed
// 2019/12/22  J修正
// **********************************************************************

#include "stdafx.h"

#include "GnrlDefine.h"
#include "GnrlThread.h"

#include "GLThread.h"

void runGraphicsTest(int argc, char **argv);

GLThread::GLThread()
: m_Wnd(NULL),m_PramTrig(0)
{};

void GLThread::setWnd(CWnd *Wnd)
{
	m_Wnd = Wnd;
}

// Com受信ルーチン
int32_t GLThread::run()
{
	uint8_t	lastdata[2];
	int		recvlen;
	time_t	lasttime, elaptime;

//--	ASSERT(m_Wnd);			// メッセージ通知先ウインドウを

///--	lasttime = time(NULL);	// 秒単位

//--	while (isLife()){
//--	}

	char *c=nullptr;
	runGraphicsTest(0, &c);


	return 0;
}

