// **********************************************************************
// �X���b�h�쐬�A�Ǘ��A�I������
//
// GnrlThread.h
// 
// 2013/06/31  Createed
// 2019/12/22  J�C��
// **********************************************************************

#ifndef GENERALTHREAD_H
#define GENERALTHREAD_H



class GnrlThread
{
public:

	typedef enum{					// ���[�J�[�X���b�h�̏��
		EWS_normal			= 0,	// �ʏ탏�[�N
		EWS_onetimestop		= 1,	// �ꎞ��~
		EWS_discontinuance	= 2		// ���~, ��~
	} E_WorkState;

	GnrlThread();
	virtual ~GnrlThread();

	void*	start(bool Suspend = 0);
	int32_t	resumeThread()		{ return ResumeThread(m_hThread); }	// �֐�����������ƁA�֐��Ăяo���O�̃T�X�y���h�J�E���g���Ԃ�܂��B�֐������s����ƁA0xFFFFFFFF ���Ԃ�܂��B	beginThread();
	virtual void	quit();											// �O������X���b�h���I��������B ��  endThread()
	bool	isQuit(uint32_t dwMilliseconds = 10000);					// �X���b�h���I���������m�F
	bool	isLife() const		{ return m_LIFE;	}				// �X���b�h�I����Ԏ擾
	void	setLife(bool Life)	{ m_LIFE = Life;	}				// Life���Z�b�g
	virtual int32_t run() = 0;										// �����́A�p����̃����o�ϐ��Őݒ肵�܂�		�� VRoutine()

private:
	bool	m_LIFE;													// FALS�ɂȂ�����A�����Ƀ��[�v���I�����A���[�J�[�X���b�h���I�������ĉ������B
	static	void s_callRoutine(void *Arg);							// BeginThread()���ĂԈׂ̊֐�
	uint32_t m_ThreadID;												//	m_lpStartAddress = (LPTHREAD_START_ROUTINE)&CAboutDlg::test;�@��staic�錾�̊֐�
	void*	m_hThread;
};

#endif //GENERALTHREAD

// ----------------- GeneralThread / Mutex -------------------------------

// **********************************************************************
// �~���[�e�N�X���b�N
// ���̃N���X�̓V���O���^�X�N�̂ݗL���ł�
//  C++
// **********************************************************************

#ifndef GENERALMUTEX_H
#define GENERALMUTEX_H

class GnrlMutex
{
public:

	GnrlMutex();
	~GnrlMutex();

	//�@���ݏ������Ă���n���h����߂��܂��B
	//�@�߂�l�@==NUNN �n���h���擾���s�܂��͖��擾

#ifdef HAVE_CXX_VOLATILE
	volatile
#endif
	const void *getHandle() const{
		return theMutex;
	}

	int32_t lock();		// ���������0��Ԃ��@�ȊO�ُ͈�
	int32_t trylock();	// ���������0��Ԃ��@�ȊO�ُ͈�
	int32_t unlock();	// ���������0��Ԃ��@�ȊO�ُ͈�

	static const int32_t m_Busy;

private:
	GnrlMutex(const GnrlMutex& arg);

	int32_t m_State;		// GetLastError();�̖߂�l

#ifdef HAVE_CXX_VOLATILE
	volatile
#endif
	void *theMutex;
};

// **********************************************************************
// �����ϐ��錾�p�~���[�e�N�X���b�N�N���X
// �������X�R�[�v���O�ꂽ����unlock()�������ꍇ�Ɏg�p����B
// **********************************************************************
class GnrlLockedMutex
{
public:
	explicit GnrlLockedMutex(GnrlMutex &MutexObj);				// �R���X�g���N�^��Lock���܂��B

	~GnrlLockedMutex();

	// �߂�l ==0 ����
	int32_t getStare()	{	return m_State;	}

private:
	GnrlMutex	&m_Mutex;
	int32_t		m_State;
};

#endif //GENERALMUTEX_H

