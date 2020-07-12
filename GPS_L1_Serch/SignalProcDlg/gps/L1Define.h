/*
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
// For high dynamics applications
//#define ARMGPS_HD
//#define SAT_SELECT

#define NUM_THREADS		32      // Number of threads per block.
#define MAX_THREADS		32		// メモリ確保で使用する 最大スレッド数

// Max task number
#define MAX_TASKS	10 

#define MAX_L1_CA_CODE	1023

// Task states
#define DEAD		0
#define READY		1
#define SLEEPING	2

#define CODE_LOCK           0x01UL
#define CARRIER_LOCK        0x02UL
#define BIT_LOCK            0x04UL
#define FRAME_SYNC          0x08UL
#define FRAME_SYNC_AND_TIC  0x10UL
#define RESET_STATUS        0x20UL
#define ALL_LOCKS           0x0fUL

// Critical section
#define ENTER_CRITICAL			(CriticalSection=1)
#define LEAVE_CRITICAL			(CriticalSection=0)

// IO functions
#define outpw(Addr,Data) *(volatile long *)(Addr) = (unsigned long)(Data)
#define inpw(Addr) (*(volatile unsigned long *)(Addr))
#define outpb(Addr,Data) *(volatile char *)(Addr) = (char)(Data)
#define inpb(Addr) (*(volatile char *)(Addr))

// INTC defines
#define INTC_BASE               0xe0006000
#define POLARITY                0xe0006004
#define INT_TRIGGER             0xe000600c
#define RESET_EDGE_TRIGGERS     0xe0006010
#define INT_ENABLE              0xe0006014
#define INT_TYPE                0xe000601c

#define INTC_ACCUM_INT			0x00000002 // channel  1
#define INTC_UART1_RX_FULL		0x00008000 // channel 15
#define INTC_UART1_TX_EMPTY		0x00010000 // channel 16

// Watchdog defines
#define WATCHDOG_RESTART_REG	0xe000400c
#define WATCHDOG_RESTART_KEY	0xecd9f7bd

// OSGPS IO functions
#define CORR_BASE				0x40100000
#define to_gps(Addr,Data) *(volatile short *)(CORR_BASE+((unsigned long)(Addr)<<2)) = (unsigned short)(Data)
#define from_gps(Addr) (*(volatile unsigned short *)(CORR_BASE+((unsigned long)(Addr)<<2)))

// OSGPS Parameters
#define N_CHANNELS				12
#define N_SATELLITES			32

#define IDLE					0

#define	FALSE					0
#define TRUE					1

#define OFF						0
#define ACQUISITION				1
#define CONFIRM					2
#define PULL_IN					3
#define TRACK					4

#define COLD_START				0
#define WARM_START				1
#define HOT_START				2
#define TRACKING				3
#define NAVIGATING				4

#define CARRIER_REF				0x01f7b1b9L
#define CODE_REF				0x016ea4a8L

#define SCALED_PI_ON_2 			25736L
#define SCALED_PI				51472L

#define CC_SCALE				1540

#define NO_CLOCK				0 
#define HOW_CLOCK				1
#define SF1_CLOCK				2
#define FIX_CLOCK				3

// システムクロック
#define SYS_FREQ_RES	(40000000/7)
// キャリア周波数
#define CARR_FREQ_RES	(1405000)
// コード周波数(ハーフチップ)
#define CODE_FREQ_RES	(2046000)

//#define CARR_FREQ_RES			42.574746268e-3 
//#define CODE_FREQ_RES			85.149495e-3

#define NOISE_FLOOR_AVG			6619L
#define SIGNAL_LOSS_AVG			9349L

#define PREAMBLE				0x22c00000L
#define PB1						0xbb1f3480L
#define PB2						0x5d8f9a40L
#define PB3						0xaec7cd00L
#define PB4						0x5763e680L
#define PB5						0x6bb1f340L
#define PB6						0x8b7a89c0L

#define FRAME_BUFF_SIZE			12

#define r_to_d		57.29577951308232
#define PI			3.1415926535898

#define c_2p12		4096.0
#define c_2p4		16.0
#define c_2m5  		0.03125
#define c_2m11 		4.8828125e-4
#define c_2m19 		1.9073486328125e-6
#define c_2m20 		9.5367431640625e-7
#define c_2m21 		4.76837158203125e-7
#define c_2m23 		1.19209289550781e-7
#define c_2m24 		5.96046447753906e-8
#define c_2m27 		7.45058059692383e-9
#define c_2m29 		1.86264514923096e-9
#define c_2m30 		9.31322574615479e-10
#define c_2m31 		4.65661287307739e-10
#define c_2m33 		1.16415321826935E-10
#define c_2m38 		3.63797880709171e-12
#define c_2m43 		1.13686837721616e-13
#define c_2m50 		8.881784197e-16
#define c_2m55 		2.77555756156289e-17

#define SEC_IN_WEEK				604800.0
#define BITS_IN_WEEK			30240000L // 20 ms epoch count in week

#define TIC_PERIOD				0.0999999

#define OMEGA_E		7.2921151467E-5
#define C_LIGHT		2.99792458e8

#define	WGS84_A		6378137.0
#define WGS84_B		6356752.314

#define QUARTER_OF_CHIP			(2.4438e-7) // 1/4 chips in seconds. About 74 meters.

#define INVALID		-1

#define ELEVATION_MASK			10.0

#define LAMBDA					0.190293672798365


