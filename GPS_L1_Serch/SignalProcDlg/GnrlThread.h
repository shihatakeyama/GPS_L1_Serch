// **********************************************************************
// スレッド作成、管理、終了する
//
// GnrlThread.h
// 
// 2013/06/31  Createed
// 2019/12/22  J修正
// **********************************************************************

#ifndef GENERALTHREAD_H
#define GENERALTHREAD_H



class GnrlThread
{
public:

	typedef enum{					// ワーカースレッドの状態
		EWS_normal			= 0,	// 通常ワーク
		EWS_onetimestop		= 1,	// 一時停止
		EWS_discontinuance	= 2		// 中止, 停止
	} E_WorkState;

	GnrlThread();
	virtual ~GnrlThread();

	void*	start(bool Suspend = 0);
	int32_t	resumeThread()		{ return ResumeThread(m_hThread); }	// 関数が成功すると、関数呼び出し前のサスペンドカウントが返ります。関数が失敗すると、0xFFFFFFFF が返ります。	beginThread();
	virtual void	quit();											// 外部からスレッドを終了させる。 旧  endThread()
	bool	isQuit(uint32_t dwMilliseconds = 10000);					// スレッドが終了したか確認
	bool	isLife() const		{ return m_LIFE;	}				// スレッド終了状態取得
	void	setLife(bool Life)	{ m_LIFE = Life;	}				// Lifeをセット
	virtual int32_t run() = 0;										// 引数は、継承先のメンバ変数で設定します		旧 VRoutine()

private:
	bool	m_LIFE;													// FALSになったら、直ちにループを終了し、ワーカースレッドを終了させて下さい。
	static	void s_callRoutine(void *Arg);							// BeginThread()が呼ぶ為の関数
	uint32_t m_ThreadID;												//	m_lpStartAddress = (LPTHREAD_START_ROUTINE)&CAboutDlg::test;　←staic宣言の関数
	void*	m_hThread;
};

#endif //GENERALTHREAD

// ----------------- GeneralThread / Mutex -------------------------------

// **********************************************************************
// ミューテクスロック
// このクラスはシングルタスクのみ有効です
//  C++
// **********************************************************************

#ifndef GENERALMUTEX_H
#define GENERALMUTEX_H

class GnrlMutex
{
public:

	GnrlMutex();
	~GnrlMutex();

	//　現在所持しているハンドルを戻します。
	//　戻り値　==NUNN ハンドル取得失敗または未取得

#ifdef HAVE_CXX_VOLATILE
	volatile
#endif
	const void *getHandle() const{
		return theMutex;
	}

	int32_t lock();		// 成功すると0を返す　以外は異常
	int32_t trylock();	// 成功すると0を返す　以外は異常
	int32_t unlock();	// 成功すると0を返す　以外は異常

	static const int32_t m_Busy;

private:
	GnrlMutex(const GnrlMutex& arg);

	int32_t m_State;		// GetLastError();の戻り値

#ifdef HAVE_CXX_VOLATILE
	volatile
#endif
	void *theMutex;
};

// **********************************************************************
// 自動変数宣言用ミューテクスロッククラス
// 処理がスコープを外れた時にunlock()したい場合に使用する。
// **********************************************************************
class GnrlLockedMutex
{
public:
	explicit GnrlLockedMutex(GnrlMutex &MutexObj);				// コンストラクタでLockします。

	~GnrlLockedMutex();

	// 戻り値 ==0 成功
	int32_t getStare()	{	return m_State;	}

private:
	GnrlMutex	&m_Mutex;
	int32_t		m_State;
};

#endif //GENERALMUTEX_H

