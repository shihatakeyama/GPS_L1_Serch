// **********************************************************************
// スレッド作成、管理、終了する
//
// GnrlThread.cpp
// 
// 2013/06/31  Createed
// 2019/12/22  J修正
// **********************************************************************

#include "stdafx.h"

#include "GnrlDefine.h"
#include "GnrlThread.h"



GnrlThread::GnrlThread()
 :m_LIFE(FALSE)
 ,m_ThreadID(0)
 ,m_hThread(NULL)
{}

GnrlThread::~GnrlThread()
{
	quit();
}

// **********************************************************************
//  スレッドを起動します。
//  スレッド起動先で引数の指定はできません。
//  パラメータを渡す場合は、グローバル変数で渡すか、または、GnrlThreadを継承またはコンボじションして、クラスのメンバで行って下さい。
//  引数　Parm　NULL：　この関数を実行する前に、予めスレッドのスタートアドレスが設定されている必要があります。
//　　　　　　スレッドのスタートアドレス：
// **********************************************************************
VOID*  GnrlThread::start(bool Suspend)
{
	int32_t creationflag = 0;

	if(Suspend){
		creationflag |= CREATE_SUSPENDED;
	}

	if(isQuit(0)==FALSE)
		return NULL;			// 既にスレッド起動中

	m_LIFE = TRUE;				// 排他の為、m_LIFE を TRUEにするのはここだけにしよう。
	m_hThread = ::CreateThread(0, 0,
		(LPTHREAD_START_ROUTINE)s_callRoutine,
		dynamic_cast<VOID*>(this), creationflag, (unsigned long*)&m_ThreadID);

	if(m_hThread == NULL){
		m_LIFE = FALSE;
	}

	return m_hThread;
}

// **********************************************************************
//  スレッドを終了(シグナル状態に)する。
//  操作する親スレッドが呼んで下さい。
// **********************************************************************
void GnrlThread::quit()
{
	m_LIFE = FALSE;		// 継承先でquit()を定義した場合は、継承先で本関数を呼んで下さい。
}

// **********************************************************************
// スレッドが終了したならTRUEを返します
//　スレッドが終了していない場合はFALSEを返します
// 引数0は待たない、0以外は終了するまでミリ秒待機します。
// **********************************************************************
bool GnrlThread::isQuit(uint32_t dwMilliseconds)
{
#if 0	// m_hThreadハンドルが無効の場合、WaitForSingleObject()では終了が判断できない？
	if(WaitForSingleObject((void*)m_hThread, dwMilliseconds) == WAIT_TIMEOUT)
		return FALSE;
#else
	if(m_hThread != nullptr){
		return FALSE;
	}
#endif
	return TRUE;
}

// **********************************************************************
// 派生先のvRoutine()を呼び出し、この関数内で、
// ワーカースレッドが動作します。
// 派生先クラスに、メンバーを作成して、使用して下さい。
// **********************************************************************
void GnrlThread::s_callRoutine(void *Arg)
{
	int32_t ret;

	GnrlThread *gt = static_cast<GnrlThread*>(Arg);
	ret = gt->run();

	gt->quit();
	CloseHandle(gt->m_hThread);

	gt->m_ThreadID= 0;
	gt->m_hThread = nullptr;
}


// ----------------- GeneralThread / GeneralMutex -------------------------------

const int GnrlMutex::m_Busy = -1;	// 使用中の場合は-1を返す

GnrlMutex::GnrlMutex()
: theMutex(NULL)
, m_State(-1)
{
#ifdef WINDOWS_INTERFACE
  theMutex = (void *)(CreateMutex(NULL, FALSE, NULL));
#elif defined(POSIX_INTERFACE)
  pthread_mutex_t *mtx = new pthread_mutex_t;
  if (mtx)
  {
    if (pthread_mutex_init(mtx, NULL)) delete mtx;
    else theMutex = mtx;
  }
#elif defined(SOLARIS_INTERFACE)
  mutex_t *mtx = new mutex_t;
  if (mtx)
  {
    if (mutex_init(mtx, USYNC_THREAD, NULL)) delete mtx;
    else theMutex = mtx;
  }
#else
#endif
}


GnrlMutex::~GnrlMutex()
{
#ifdef WINDOWS_INTERFACE
  CloseHandle((HANDLE)theMutex);
#elif defined(POSIX_INTERFACE)
  if (theMutex) pthread_mutex_destroy(OFthread_cast(pthread_mutex_t *, theMutex));
  delete OFthread_cast(pthread_mutex_t *, theMutex);
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) mutex_destroy(OFthread_cast(mutex_t *, theMutex));
  delete OFthread_cast(mutex_t *, theMutex);
#else
#endif
}

// **********************************************************************
//　現在所持しているハンドルを戻します。
//　戻り値　==NUNN ハンドル取得失敗または未取得
// **********************************************************************

// **********************************************************************
// 戻り値 ==0 ロック成功
//		  !=0 ロック失敗
// **********************************************************************
int32_t GnrlMutex::lock()
{
#ifdef WINDOWS_INTERFACE
  if (WaitForSingleObject((HANDLE)theMutex, INFINITE) == WAIT_OBJECT_0)
  {
	  return 0;
  }
  else
  {
	  m_State = (int32_t)GetLastError();
	  return m_State;
  }
#elif defined(POSIX_INTERFACE)
  if (theMutex)
	  return pthread_mutex_lock(OFthread_cast(pthread_mutex_t *, theMutex));
  else
	  return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theMutex)
	  return mutex_lock(OFthread_cast(mutex_t *, theMutex));
  else
	  return EINVAL;
#else
  return -1;
#endif
}

// **********************************************************************
// ブロッキングを行わないロックを行う
//  戻り値
//    == 0　ブロッキング成功
//    == -1 ロックに失敗した場合（資源が使用中の場合）
//     他の値　失敗
// **********************************************************************
int32_t GnrlMutex::trylock()
{
#ifdef WINDOWS_INTERFACE
	int32_t ret;
	ret = WaitForSingleObject((HANDLE)theMutex, 0);
	if(ret == WAIT_OBJECT_0)
		return 0;
	else
		if(ret == WAIT_TIMEOUT)
			return GnrlMutex::m_Busy;
	else
	{
		m_State = (int32_t)GetLastError();
		return m_State;
	}
#elif defined(POSIX_INTERFACE)
  if (theMutex) return pthread_mutex_trylock(OFthread_cast(pthread_mutex_t *, theMutex)); else return EINVAL; // may return EBUSY.
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) return mutex_trylock(OFthread_cast(mutex_t *, theMutex)); else return EINVAL; // may return EBUSY.
#else
  return -1;
#endif
}

int32_t GnrlMutex::unlock()
{
#ifdef WINDOWS_INTERFACE
  if (ReleaseMutex((HANDLE)theMutex))
	  return 0;
  else
  {
	  m_State = (int32_t)GetLastError();
	  return m_State;
  }
#elif defined(POSIX_INTERFACE)
  if (theMutex) return pthread_mutex_unlock(OFthread_cast(pthread_mutex_t *, theMutex)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) return mutex_unlock(OFthread_cast(mutex_t *, theMutex)); else return EINVAL;
#else
  return -1;
#endif
}

// ----------------- GnrlMutex / GnrlLockedMutex -------------------------------

GnrlLockedMutex::GnrlLockedMutex(GnrlMutex &MutexObj)
:m_Mutex(MutexObj)
//,m_Locked(FALSE)
{
//	lock();
	m_State = m_Mutex.lock();
}

#if 0	// 20121008 使用する予定は無いのでソースアウト
GnrlLockedMutex::GnrlLockedMutex(GnrlMutex &MutexObj, bool Lock)
:m_Mutex(MutexObj)
,m_Locked(Lock)
{
	if(Lock)
		lock();
}

int32_t	GnrlLockedMutex::lock()
{
	if(m_Mutex.lock()==0)
	{
		m_Locked = true;
		return 1;
	}else
	{
		return 0;
	}
}

int32_t	GnrlLockedMutex::unlock()
{
	if(m_Mutex.unlock()==0)
	{
		m_Locked = false;
		return 1;
	}else
	{
		return 0;	
	}
}
#endif

GnrlLockedMutex::~GnrlLockedMutex()
{
	m_Mutex.unlock();
}
