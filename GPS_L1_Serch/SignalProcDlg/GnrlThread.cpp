// **********************************************************************
// �X���b�h�쐬�A�Ǘ��A�I������
//
// GnrlThread.cpp
// 
// 2013/06/31  Createed
// 2019/12/22  J�C��
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
//  �X���b�h���N�����܂��B
//  �X���b�h�N����ň����̎w��͂ł��܂���B
//  �p�����[�^��n���ꍇ�́A�O���[�o���ϐ��œn�����A�܂��́AGnrlThread���p���܂��̓R���{���V�������āA�N���X�̃����o�ōs���ĉ������B
//  �����@Parm�@NULL�F�@���̊֐������s����O�ɁA�\�߃X���b�h�̃X�^�[�g�A�h���X���ݒ肳��Ă���K�v������܂��B
//�@�@�@�@�@�@�X���b�h�̃X�^�[�g�A�h���X�F
// **********************************************************************
VOID*  GnrlThread::start(bool Suspend)
{
	int32_t creationflag = 0;

	if(Suspend){
		creationflag |= CREATE_SUSPENDED;
	}

	if(isQuit(0)==FALSE)
		return NULL;			// ���ɃX���b�h�N����

	m_LIFE = TRUE;				// �r���ׁ̈Am_LIFE �� TRUE�ɂ���̂͂��������ɂ��悤�B
	m_hThread = ::CreateThread(0, 0,
		(LPTHREAD_START_ROUTINE)s_callRoutine,
		dynamic_cast<VOID*>(this), creationflag, (unsigned long*)&m_ThreadID);

	if(m_hThread == NULL){
		m_LIFE = FALSE;
	}

	return m_hThread;
}

// **********************************************************************
//  �X���b�h���I��(�V�O�i����Ԃ�)����B
//  ���삷��e�X���b�h���Ă�ŉ������B
// **********************************************************************
void GnrlThread::quit()
{
	m_LIFE = FALSE;		// �p�����quit()���`�����ꍇ�́A�p����Ŗ{�֐����Ă�ŉ������B
}

// **********************************************************************
// �X���b�h���I�������Ȃ�TRUE��Ԃ��܂�
//�@�X���b�h���I�����Ă��Ȃ��ꍇ��FALSE��Ԃ��܂�
// ����0�͑҂��Ȃ��A0�ȊO�͏I������܂Ń~���b�ҋ@���܂��B
// **********************************************************************
bool GnrlThread::isQuit(uint32_t dwMilliseconds)
{
#if 0	// m_hThread�n���h���������̏ꍇ�AWaitForSingleObject()�ł͏I�������f�ł��Ȃ��H
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
// �h�����vRoutine()���Ăяo���A���̊֐����ŁA
// ���[�J�[�X���b�h�����삵�܂��B
// �h����N���X�ɁA�����o�[���쐬���āA�g�p���ĉ������B
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

const int GnrlMutex::m_Busy = -1;	// �g�p���̏ꍇ��-1��Ԃ�

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
//�@���ݏ������Ă���n���h����߂��܂��B
//�@�߂�l�@==NUNN �n���h���擾���s�܂��͖��擾
// **********************************************************************

// **********************************************************************
// �߂�l ==0 ���b�N����
//		  !=0 ���b�N���s
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
// �u���b�L���O���s��Ȃ����b�N���s��
//  �߂�l
//    == 0�@�u���b�L���O����
//    == -1 ���b�N�Ɏ��s�����ꍇ�i�������g�p���̏ꍇ�j
//     ���̒l�@���s
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

#if 0	// 20121008 �g�p����\��͖����̂Ń\�[�X�A�E�g
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
